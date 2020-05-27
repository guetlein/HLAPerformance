
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

#ifndef FEDAMB_H_
#define FEDAMB_H_

#include <stdlib.h>
#include <string>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <ostream>

#include "RTI/NullFederateAmbassador.h"
#include "RTI/encoding/BasicDataElements.h"
#include "RTI/time/HLAfloat64Time.h"

using namespace rti1516e;
using namespace std;

class FedAmb : public NullFederateAmbassador {
   
   public:

    double federateTime;
    double federateLookahead;
    bool isRegulating;
    bool isConstrained;
    bool isAdvancing;
    bool isAnnounced;
    bool isSynced;
    bool isSyncedAnnounced;
    bool isReadyToRun;

    ObjectClassHandle testcaseObjectClassHandle;  //for rtt and objectupdate
    AttributeHandle testcaseAttributeHandle;

    ObjectInstanceHandle testcaseRTTObjectHandle;  // for rtt
    volatile bool receivedTestcaseRTTObjectHandle;

    InteractionClassHandle testcaseInteractionHandle;  //for interaction
    ParameterHandle testcaseInteractionParamHandle;

    volatile bool owner;
    volatile bool divestRequest;
    ObjectInstanceHandle target;
    AttributeHandleSet targetAtt;
    int lastValue;
    ObjectClassHandle HLAfederateHandle;
    AttributeHandle HLAfederateTypeHandle;
    int joinedFederates;

    FedAmb();
    virtual ~FedAmb() throw();

    virtual void synchronizationPointRegistrationSucceeded(const std::wstring& label) throw(FederateInternalError);

    virtual void
    synchronizationPointRegistrationFailed(const std::wstring& label,
                                           SynchronizationPointFailureReason reason) throw(FederateInternalError);

    virtual void announceSynchronizationPoint(const std::wstring& label,
                                              const VariableLengthData& tag) throw(FederateInternalError);

    virtual void federationSynchronized(const std::wstring& label,
                                        const FederateHandleSet& failedSet) throw(FederateInternalError);

    virtual void timeRegulationEnabled(const LogicalTime& theFederateTime) throw(FederateInternalError);

    virtual void timeConstrainedEnabled(const LogicalTime& theFederateTime) throw(FederateInternalError);

    virtual void timeAdvanceGrant(const LogicalTime& theFederateTime) throw(FederateInternalError);

    virtual void discoverObjectInstance(ObjectInstanceHandle theObject,
                                        ObjectClassHandle theObjectClass,
                                        const std::wstring& theObjectName) throw(FederateInternalError);

    virtual void discoverObjectInstance(ObjectInstanceHandle theObject,
                                        ObjectClassHandle theObjectClass,
                                        const std::wstring& theObjectName,
                                        FederateHandle producingFederate) throw(FederateInternalError);

    virtual void reflectAttributeValues(ObjectInstanceHandle theObject,
                                        const AttributeHandleValueMap& theAttributes,
                                        const VariableLengthData& tag,
                                        OrderType sentOrder,
                                        TransportationType theType,
                                        SupplementalReflectInfo theReflectInfo) throw(FederateInternalError);

    virtual void reflectAttributeValues(ObjectInstanceHandle theObject,
                                        const AttributeHandleValueMap& theAttributes,
                                        const VariableLengthData& tag,
                                        OrderType sentOrder,
                                        TransportationType theType,
                                        LogicalTime const& theTime,
                                        OrderType receivedOrder,
                                        SupplementalReflectInfo theReflectInfo) throw(FederateInternalError);

    virtual void reflectAttributeValues(ObjectInstanceHandle theObject,
                                        const AttributeHandleValueMap& theAttributes,
                                        const VariableLengthData& tag,
                                        OrderType sentOrder,
                                        TransportationType theType,
                                        const LogicalTime& theTime,
                                        OrderType receivedOrder,
                                        MessageRetractionHandle theHandle,
                                        SupplementalReflectInfo theReflectInfo) throw(FederateInternalError);

    virtual void receiveInteraction(InteractionClassHandle theInteraction,
                                    const ParameterHandleValueMap& theParameters,
                                    const VariableLengthData& tag,
                                    OrderType sentOrder,
                                    TransportationType theType,
                                    SupplementalReceiveInfo theReceiveInfo) throw(FederateInternalError);

    virtual void receiveInteraction(InteractionClassHandle theInteraction,
                                    const ParameterHandleValueMap& theParameters,
                                    const VariableLengthData& tag,
                                    OrderType sentOrder,
                                    TransportationType theType,
                                    const LogicalTime& theTime,
                                    OrderType receivedOrder,
                                    SupplementalReceiveInfo theReceiveInfo) throw(FederateInternalError);

    virtual void receiveInteraction(InteractionClassHandle theInteraction,
                                    const ParameterHandleValueMap& theParameters,
                                    const VariableLengthData& tag,
                                    OrderType sentOrder,
                                    TransportationType theType,
                                    const LogicalTime& theTime,
                                    OrderType receivedOrder,
                                    MessageRetractionHandle theHandle,
                                    SupplementalReceiveInfo theReceiveInfo) throw(FederateInternalError);

    virtual void removeObjectInstance(ObjectInstanceHandle theObject,
                                      const VariableLengthData& tag,
                                      OrderType sentOrder,
                                      SupplementalRemoveInfo theRemoveInfo) throw(FederateInternalError);

    virtual void removeObjectInstance(ObjectInstanceHandle theObject,
                                      const VariableLengthData& tag,
                                      OrderType sentOrder,
                                      const LogicalTime& theTime,
                                      OrderType receivedOrder,
                                      SupplementalRemoveInfo theRemoveInfo) throw(FederateInternalError);

    virtual void removeObjectInstance(ObjectInstanceHandle theObject,
                                      const VariableLengthData& tag,
                                      OrderType sentOrder,
                                      const LogicalTime& theTime,
                                      OrderType receivedOrder,
                                      MessageRetractionHandle theHandle,
                                      SupplementalRemoveInfo theRemoveInfo) throw(FederateInternalError);

    virtual void requestAttributeOwnershipRelease(
        ObjectInstanceHandle theObject,
        AttributeHandleSet const& candidateAttributes,
        VariableLengthData const& theUserSuppliedTag) throw(rti1516e::FederateInternalError);

    virtual void attributeOwnershipAcquisitionNotification(
        ObjectInstanceHandle theObject,
        AttributeHandleSet const& securedAttributes,
        VariableLengthData const& theUserSuppliedTag) throw(rti1516e::FederateInternalError);

    virtual void attributeOwnershipUnavailable(
        ObjectInstanceHandle theObject,
        AttributeHandleSet const& securedAttributes) throw(rti1516e::FederateInternalError);

    double convertTime(const LogicalTime& theTime);

};

#endif
