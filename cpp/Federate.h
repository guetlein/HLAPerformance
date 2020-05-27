
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

#ifndef FEDERATE_H_
#define FEDERATE_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <list>
#include <ostream>
#include <sstream>
#include <string>
#include <string.h>

#include "RTI/RTI1516.h"
#include "RTI/encoding/BasicDataElements.h"
#include "RTI/time/HLAfloat64Interval.h"
#include "RTI/time/HLAfloat64Time.h"

#include "FedAmb.h"

#include "Testcase.h"
#include "TestcaseInteraction.h"
#include "TestcaseObjectUpdate.h"
#include "TestcaseRTT.h"
#include "TestcaseTimeOnly.h"

using namespace rti1516e;
using namespace std;

#define READY_TO_RUN L"ReadyToRun"
#define READY_TO_SYNC L"ReadyToSync"
#define DEFAULT_STEPLENGTH 1L
#define NUM_FEDERATES 2
#define FEDERATION_NAME L"MeineFederation1516e"
#define FOM_PATH L"PerformanceEvaluationDSRT20.xml"

enum TestcaseEnum {
    Testcase_TimeOnly,
    Testcase_Interaction,
    Testcase_ObjectUpdate,
    Testcase_RTT
};
class Testcase;

class Federate {
   public:
    RTIambassador *rtiamb;
    FedAmb *fedamb;

    Federate(std::string fname, TestcaseEnum experiment, int noIteraions, double min, double max, int paramSize);
    virtual ~Federate();
    void run();

    void initializeHandles();
    void waitForUser();
    void waitForFederates();
    void enableTimePolicy();
    void publishAndSubscribe();

    void evoke();
    void advanceTime(double timestep);
    void advanceTime();
    ObjectInstanceHandle registerObject(std::wstring);
    void deleteObject(ObjectInstanceHandle objectHandle);
    void responseToOwnershipReleaseRequest(ObjectInstanceHandle theObject, AttributeHandleSet const &candidateAttributes);
    VariableLengthData generateTag();

    void runTestcase(Testcase &testcase);
    void timerStart();
    void timerStop();

   protected:
    std::wstring federateName;
    std::string federateNameStr;
    TestcaseEnum experiment;
    int noIterations;
    double evokeMin;
    double evokeMax;
    int interactionParamSize;
    
    struct timespec t;  
    std::list<timespec> startTimes;
    std::list<timespec> endTimes;
};

#endif