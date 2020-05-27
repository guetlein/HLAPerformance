
/*******************************************************************************
 * Copyright (C) 2020 Moritz Gütlein, Chair of Computer Science 7,
 * FAU Erlangen-Nuernberg
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Basic Federate 
 * Inspired by 
 * * IEEE 1516-2010 Standard
 * * https://github.com/openlvc/portico @CDDL
 * * http://git.savannah.gnu.org/cgit/certi.git @GPL/LGPL
 * 
 ******************************************************************************/

#include "Federate.h"

Federate::Federate(std::string fname, TestcaseEnum experiment, int noIteraions, double min, double max, int paramSize) {
    this.experiment = experiment;
    this.federateNameStr = fname;
    this.federateName.assign(fname.begin(), fname.end());
    this.noIterations = noIterations;
    this.evokeMin = min;
    this.evokeMax = max;
    this.interactionParamSize = paramSize;
}

Federate::~Federate() {
    if (this->fedamb)
        delete this->fedamb;
}

void Federate::runFederate() {
    wcout << L"Running with testcase=" << experiment << L", iter=" << noIterations << L", evokeMin=" << evokeMin << L" evokeMax=" << evokeMax << L", intSize=" << interactionParamSize << endl;

    // Set up
    RTIambassadorFactory factory = RTIambassadorFactory();
    this->rtiamb = factory.createRTIambassador().release();
    federateNameStr.assign(federateName.begin(), federateName.end());
    this->fedamb = new FedAmb();

    //Connect
    try {
        rtiamb->connect(*this->fedamb, HLA_EVOKED);
    } catch (ConnectionFailed &e) {
        wcout << L"Connection error: " << e.what() << endl;
    } catch (InvalidLocalSettingsDesignator &e) {
        wcout << L"Connection error: " << e.what() << endl;
    } catch (UnsupportedCallbackModel &e) {
        wcout << L"Connection error: " << e.what() << endl;
    } catch (AlreadyConnected &e) {
        wcout << L"Connection error: " << e.what() << endl;
    } catch (RTIinternalError &e) {
        wcout << L"Connection error: " << e.what() << endl;
    }

    //Create and join
    try {
        rtiamb->createFederationExecution(FEDERATION_NAME, FOM_PATH);
        wcout << L"Created federation" << endl;
    } catch (FederationExecutionAlreadyExists &exists) {
        wcout << L"Federation is already there" << endl;
    }
    try {
        rtiamb->joinFederationExecution(federateName, federateName, FEDERATION_NAME);
        wcout << L"Joined federation as " << federateName << endl;
    } catch (Exception &e) {
        wcout << L"Joining error: " << e.what() << endl;
    }

    initializeHandles();
    rtiamb->enableAsynchronousDelivery();

    //Sync
    try {
        rtiamb->registerFederationSynchronizationPoint(READY_TO_RUN, generateTag());
        wcout << L"Registered synchronization point: " << READY_TO_RUN << endl;
    } catch (Exception &e) {
        wcout << L"Register synchronization point error: " << e.what() << endl;
    }
    while (fedamb->isAnnounced == false) {
        rtiamb->evokeMultipleCallbacks(evokeMin, evokeMax);
    }

    waitForUser();  //or waitForFederates();

    rtiamb->synchronizationPointAchieved(READY_TO_RUN);
    wcout << L"Achieved synchronization point: " << READY_TO_RUN << endl;
    while (fedamb->isReadyToRun == false) {
        rtiamb->evokeMultipleCallbacks(evokeMin, evokeMax);
    }

    //almost ready
    enableTimePolicy();
    publishAndSubscribe();

    //start test
    if (experiment == Testcase_TimeOnly) {
        TestcaseTimeOnly t(federateNameStr, noIterations, this, fedamb);
        runTestcase(t);
    } else if (experiment == Testcase_Interaction) {
        TestcaseInteraction t(federateNameStr, noIterations, this, fedamb);
        runTestcase(t);
    } else if (experiment == Testcase_ObjectUpdate) {
        TestcaseObjectUpdate t(federateNameStr, noIterations, this, fedamb);
        runTestcase(t);
    } else if (experiment == Testcase_RTT) {
        TestcaseRTT t(federateNameStr, noIterations, this, fedamb);
        runTestcase(t);
    }

    //clean up
    rtiamb->resignFederationExecution(NO_ACTION);
    try {
        rtiamb->destroyFederationExecution(FEDERATION_NAME);
        wcout << L"Destroyed Federation" << endl;
    } catch (FederationExecutionDoesNotExist &dne) {
        wcout << L"Federation is already gone" << endl;
    } catch (FederatesCurrentlyJoined &fcj) {
        wcout << L"Didn't destroy federation, federates still joined" << endl;
    }

    this->rtiamb->disconnect();
    delete this->rtiamb;
}

void Federate::initializeHandles() {
    fedamb->testcaseObjectClassHandle = rtiamb->getObjectClassHandle(L"HLAobjectRoot.TestcaseObject");
    fedamb->testcaseAttributeHandle = rtiamb->getAttributeHandle(fedamb->testcaseObjectClassHandle, L"TestcaseObjectAttribute");
    fedamb->testcaseInteractionHandle = rtiamb->getInteractionClassHandle(L"HLAinteractionRoot.TestcaseInteraction");
    fedamb->testcaseInteractionParamHandle = rtiamb->getParameterHandle(fedamb->testcaseInteractionHandle, L"Payload");
}

void Federate::waitForUser() {
    wcout << L"Press Enter to continue"<< endl;
    string line;
    getline(cin, line);
}

void Federate::waitForFederates() {
    wcout << L"Waiting for discovery of other federates" << endl;
    while (fedamb->joinedFederates < NUM_FEDERATES) {
        evoke();
        AttributeHandleSet attributes;
        attributes.insert(fedamb->HLAfederateTypeHandle);
        rtiamb->requestAttributeValueUpdate(fedamb->HLAfederateHandle, attributes, generateTag());
    }
}

VariableLengthData Federate::generateTag() {
    VariableLengthData tag((void *)"", 1);
    return tag;
}

void Federate::enableTimePolicy() {
    double lookahead = fedamb->federateLookahead;
    auto_ptr<HLAfloat64Interval> interval(new HLAfloat64Interval(lookahead));
    rtiamb->enableTimeRegulation(*interval);

    while (fedamb->isRegulating == false)
        rtiamb->evokeMultipleCallbacks(evokeMin, evokeMax);

    rtiamb->enableTimeConstrained();

    while (fedamb->isConstrained == false)
        rtiamb->evokeMultipleCallbacks(evokeMin, evokeMax);
}

void Federate::evoke() {
    rtiamb->evokeMultipleCallbacks(evokeMin, evokeMax);
}

void Federate::publishAndSubscribe() {
    AttributeHandleSet RTTAttributes;
    RTTAttributes.insert(fedamb->testcaseAttributeHandle);
    rtiamb->publishObjectClassAttributes(fedamb->testcaseObjectClassHandle, RTTAttributes);
    rtiamb->subscribeObjectClassAttributes(fedamb->testcaseObjectClassHandle, RTTAttributes);
    rtiamb->publishInteractionClass(fedamb->testcaseInteractionHandle);
    rtiamb->subscribeInteractionClass(fedamb->testcaseInteractionHandle);
}

ObjectInstanceHandle Federate::registerObject(std::wstring identifier) {
    ObjectClassHandle oh = rtiamb->getObjectClassHandle(identifier);
    ObjectInstanceHandle ih = rtiamb->registerObjectInstance(oh);
    wcout << ih.toString() << endl;
    if (!ih.isValid()) {
        std::wcout << L"Can't register object instance\n"
                   << std::endl;
    }
    return ih;
}

void Federate::advanceTime(double timestep) {
    fedamb->isAdvancing = true;
    auto_ptr<HLAfloat64Time> newTime(new HLAfloat64Time(fedamb->federateTime + timestep));
    rtiamb->timeAdvanceRequest(*newTime);
    while (fedamb->isAdvancing) {
        rtiamb->evokeMultipleCallbacks(evokeMin, evokeMax);
    }
}

void Federate::advanceTime() {
    advanceTime(DEFAULT_STEPLENGTH);
}

void Federate::deleteObject(ObjectInstanceHandle objectHandle) {
    rtiamb->deleteObjectInstance(objectHandle, generateTag());
}

void Federate::timerStart() {
    clock_gettime(CLOCK_REALTIME, &t);
    startTimes.push_back(t);
}

void Federate::timerStop() {
    clock_gettime(CLOCK_REALTIME, &t);
    endTimes.push_back(t);
}

void Federate::runTestcase(Testcase &testcase) {
    wcout << L"Calling testcase.init(). my name is " << federateName << ", my role is " testcase.role << endl;
    testcase.init();
    for (int i = 0; i < noIterations; i++) {
        timerStart();
        testcase.step(i);
        timerStop();
    }
    wcout << L"Did " << noIterations << L" iterations, calling testcase.finish()" << endl;
    testcase.finish();
}

void Federate::ownershipReleaseRequest(
    ObjectInstanceHandle theObject,
    AttributeHandleSet const &candidateAttributes) {
    try {
        rtiamb->unconditionalAttributeOwnershipDivestiture(theObject, candidateAttributes);
        fedamb->currentlyOwning = false;
        fedamb->shouldDivest = false;
    } catch (Exception &e) {
        wcout << L"Failed to divest ownership" << endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 6) {
        wcout << L"Usage: bin testcase iterations evokemin evokemax [paramsize] " << endl;
        exit(EXIT_FAILURE);
    }

    int interactionParamSize = 4;
    std::string federateName = argv[1];
    TestcaseEnum experiment = argv[2];
    int noIterations = atoi(argv[3]);
    float evokeMin = atof(argv[4]);
    float evokeMax = atof(argv[5]);
    if (argc >= 7)
        interactionParamSize = atoi(argv[6]);

    Federate *federate = new Federate(federateName, experiment, noIterations, evokeMin, evokeMax, interactionParamSize);
    fed->run();
    delete federate;
    return 0;
}