
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
 * Basic Federate Ambassador
 * Inspired by 
 * * IEEE 1516-2010 Standard
 * * https://github.com/openlvc/portico @CDDL
 * * http://git.savannah.gnu.org/cgit/certi.git @GPL/LGPL
 * 
 ******************************************************************************/

#include "FedAmb.h"

FedAmb::FedAmb() {
    federateTime = 0.0;
    federateLookahead = 1.0;
    isRegulating = false;
    isConstrained = false;
    isAdvancing = false;
    isAnnounced = false;
    isReadyToRun = false;
    owner = false;
    divestRequest = false;
    receivedTestcaseRTTObjectHandle = false;
    lastValue = 0;
    joinedFederates = 0;
    isSynced = false;
    isSyncedAnnounced = false;
}

FedAmb::~FedAmb() throw() {
}

double FedAmb::convertTime(const LogicalTime &theTime) {
    const HLAfloat64Time &castTime = dynamic_cast<const HLAfloat64Time &>(theTime);
    return castTime.getTime();
}

void FedAmb::synchronizationPointRegistrationSucceeded(const std::wstring &label) throw(FederateInternalError) {
    wcout << L"Successfully registered sync point: " << label << endl;
}

void FedAmb::synchronizationPointRegistrationFailed(const std::wstring &label,
                                                    SynchronizationPointFailureReason reason) throw(FederateInternalError) {
    wcout << L"Failed to register sync point: " << label << endl;
}

void FedAmb::announceSynchronizationPoint(const std::wstring &label,
                                          const VariableLengthData &tag) throw(FederateInternalError) {
    wcout << L"Synchronization point announced: " << label << endl;
    if (label.compare(L"ReadyToRun") == 0)
        isAnnounced = true;
    if (label.compare(L"ReadyToSync") == 0)
        isSyncedAnnounced = true;
}

void FedAmb::federationSynchronized(const std::wstring &label,
                                    const FederateHandleSet &failedSet) throw(FederateInternalError) {
    wcout << L"Federation Synchronized: " << label << endl;
    if (label.compare(L"ReadyToRun") == 0)
        isReadyToRun = true;
    if (label.compare(L"ReadyToSync") == 0)
        isSynced = true;
}

void FedAmb::timeRegulationEnabled(const LogicalTime &theFederateTime) throw(FederateInternalError) {
    isRegulating = true;
    federateTime = convertTime(theFederateTime);
}

void FedAmb::timeConstrainedEnabled(const LogicalTime &theFederateTime) throw(FederateInternalError) {
    isConstrained = true;
    federateTime = convertTime(theFederateTime);
}

void FedAmb::timeAdvanceGrant(const LogicalTime &theFederateTime) throw(FederateInternalError) {
    isAdvancing = false;
    federateTime = convertTime(theFederateTime);
}

void FedAmb::discoverObjectInstance(ObjectInstanceHandle theObject,
                                    ObjectClassHandle theObjectClass,
                                    const std::wstring &theObjectName) throw(FederateInternalError) {
    wcout << L"Discoverd Object: handle=" << theObject
          << L", classHandle=" << theObjectClass
          << L", name=" << theObjectName << endl;

    if (!receivedTestcaseRTTObjectHandle && theObjectClass == testcaseObjectClassHandle) {
        testcaseRTTObjectHandle = theObject;
        receivedTestcaseRTTObjectHandle = true;
        wcout << L"Received Object handle:" << testcaseRTTObjectHandle.toString() << endl;
    } else if (theObjectClass == HLAfederateHandle) {
        wcout << L"Discovered a new federate!" << endl;
        joinedFederates++;
    } else {
        wcout << L"Dont know new object of type=" << theObjectClass.toString() << endl;
    }
}

void FedAmb::discoverObjectInstance(ObjectInstanceHandle theObject,
                                    ObjectClassHandle theObjectClass,
                                    const std::wstring &theObjectName,
                                    FederateHandle producingFederate) throw(FederateInternalError) {
    wcout << L"Discoverd Object: handle=" << theObject
          << L", classHandle=" << theObjectClass
          << L", name=" << theObjectName
          << L", createdBy=" << producingFederate << endl;

    if (!receivedTestcaseRTTObjectHandle && theObjectClass == testcaseObjectClassHandle) {
        testcaseRTTObjectHandle = theObject;
        receivedTestcaseRTTObjectHandle = true;
        wcout << L"Received Object handle:" << testcaseRTTObjectHandle.toString() << endl;
    } 
    else if (theObjectClass == HLAfederateHandle) {
        wcout << L"Discovered a new federate!" << endl;
        joinedFederates++;
    } else {
        wcout << L"Dont know new object of type=" << theObjectClass.toString() << endl;
    }
}

void FedAmb::reflectAttributeValues(ObjectInstanceHandle theObject,
                                    const AttributeHandleValueMap &theAttributes,
                                    const VariableLengthData &tag,
                                    OrderType sentOrder,
                                    TransportationType theType,
                                    SupplementalReflectInfo theReflectInfo) throw(FederateInternalError) {
    AttributeHandleValueMap::const_iterator it = theAttributes.begin();
    if (it != theAttributes.end() && (it->first) == testcaseAttributeHandle && it->second.size() > 0) {
        memcpy(&lastValue, (it->second).data(), (it->second).size());
    } 
/*     else if ((it->first) == HLAfederateTypeHandle) {
        wcout << L"received HLAfederateTypeHandle=" << (it->second).data() << endl;
    } */
}

void FedAmb::reflectAttributeValues(ObjectInstanceHandle theObject,
                                    const AttributeHandleValueMap &theAttributes,
                                    const VariableLengthData &tag,
                                    OrderType sentOrder,
                                    TransportationType theType,
                                    const LogicalTime &theTime,
                                    OrderType receivedOrder,
                                    SupplementalReflectInfo theReflectInfo) throw(FederateInternalError) {
}

void FedAmb::reflectAttributeValues(ObjectInstanceHandle theObject,
                                    const AttributeHandleValueMap &theAttributes,
                                    const VariableLengthData &tag,
                                    OrderType sentOrder,
                                    TransportationType theType,
                                    const LogicalTime &theTime,
                                    OrderType receivedOrder,
                                    MessageRetractionHandle theHandle,
                                    SupplementalReflectInfo theReflectInfo) throw(FederateInternalError) {
}

void FedAmb::receiveInteraction(InteractionClassHandle theInteraction,
                                const ParameterHandleValueMap &theParameters,
                                const VariableLengthData &tag,
                                OrderType sentOrder,
                                TransportationType theType,
                                SupplementalReceiveInfo theReceiveInfo) throw(FederateInternalError) {
    /* 	wcout << L"Received interaction" << endl;
	ParameterHandleValueMap::const_iterator it = theParameters.begin();
    if (it != theParameters.end())
    {
		wcout << L"Size of param=" << it->second.size() << endl;
	}  */
}

void FedAmb::receiveInteraction(InteractionClassHandle theInteraction,
                                const ParameterHandleValueMap &theParameters,
                                const VariableLengthData &tag,
                                OrderType sentOrder,
                                TransportationType theType,
                                const LogicalTime &theTime,
                                OrderType receivedOrder,
                                SupplementalReceiveInfo theReceiveInfo) throw(FederateInternalError) {
    /* 		wcout << L"Received interaction" << endl;
	ParameterHandleValueMap::const_iterator it = theParameters.begin();
    if (it != theParameters.end())
    {
		wcout << L"Size of param=" << it->second.size() << endl;
	}  */
}

void FedAmb::receiveInteraction(InteractionClassHandle theInteraction,
                                const ParameterHandleValueMap &theParameters,
                                const VariableLengthData &tag,
                                OrderType sentOrder,
                                TransportationType theType,
                                const LogicalTime &theTime,
                                OrderType receivedOrder,
                                MessageRetractionHandle retractionHandle,
                                SupplementalReceiveInfo theReceiveInfo) throw(FederateInternalError) {
    receiveInteraction(theInteraction,
                       theParameters,
                       tag,
                       sentOrder,
                       theType,
                       theTime,
                       receivedOrder,
                       theReceiveInfo);
}

void FedAmb::removeObjectInstance(ObjectInstanceHandle theObject,
                                  const VariableLengthData &tag,
                                  OrderType sentOrder,
                                  SupplementalRemoveInfo theRemoveInfo) throw(FederateInternalError) {
}

void FedAmb::removeObjectInstance(ObjectInstanceHandle theObject,
                                  const VariableLengthData &tag,
                                  OrderType sentOrder,
                                  const LogicalTime &theTime,
                                  OrderType receivedOrder,
                                  SupplementalRemoveInfo theRemoveInfo) throw(FederateInternalError) {
}

void FedAmb::removeObjectInstance(ObjectInstanceHandle theObject,
                                  const VariableLengthData &tag,
                                  OrderType sentOrder,
                                  const LogicalTime &theTime,
                                  OrderType receivedOrder,
                                  MessageRetractionHandle theHandle,
                                  SupplementalRemoveInfo theRemoveInfo) throw(FederateInternalError) {
    removeObjectInstance(theObject, tag, sentOrder, theTime, receivedOrder, theRemoveInfo);
}

void FedAmb::requestAttributeOwnershipRelease(
    ObjectInstanceHandle theObject,
    AttributeHandleSet const &candidateAttributes,
    VariableLengthData const &theUserSuppliedTag) throw(rti1516e::FederateInternalError) {
    target = theObject;
    targetAtt = candidateAttributes;
    divestRequest = true;
}

void FedAmb::attributeOwnershipAcquisitionNotification(
    ObjectInstanceHandle theObject,
    AttributeHandleSet const &securedAttributes,
    VariableLengthData const &theUserSuppliedTag) throw(rti1516e::FederateInternalError) {
    owner = true;
}

void FedAmb::attributeOwnershipUnavailable(
    ObjectInstanceHandle theObject,
    AttributeHandleSet const &securedAttributes) throw(rti1516e::FederateInternalError) {
    wcout << L"Error: attribute ownership is unavailable" << endl;
}
