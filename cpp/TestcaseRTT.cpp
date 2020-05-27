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
 * Switch ownership of an object back and forth and increment its attribute.
 *
 ******************************************************************************/

#include "TestcaseRTT.h"
TestcaseRTT::TestcaseRTT(std::string n, int role, int i, Federate* f, FedAmb* a) : Testcase(n, role, i, f, a) {
    wcout << L"This is TestcaseRTT" << endl;
}

void TestcaseRTT::init(int role) {
    fed->advanceTime(1);

    if (federatename == "first") {
        amb->testcaseRTTObjectHandle = fed->registerObject(L"HLAobjectRoot.TestcaseObject");
        AttributeHandleValueMap attributes;
        int val = 0;
        VariableLengthData encodedValue(&val, sizeof(val));
        attributes.insert(std::pair<AttributeHandle, VariableLengthData>(amb->testcaseAttributeHandle, encodedValue));
        fed->rtiamb->updateAttributeValues(amb->testcaseRTTObjectHandle, attributes, fed->generateTag());
        amb->owner = true;
    } else {
        amb->owner = false;
        while (!amb->receivedTestcaseRTTObjectHandle) {  //might take some time and exceptions to find handle in first iteration
            fed->evoke();
        }
    }
    fed->advanceTime(1);
}

void TestcaseRTT::step(int role) {
    //case 1: the other federate is the owner, in this round i am going to request ownership
    if (amb->owner == false) {
        // in case 1, the object need to be acquired first
        try {
            AttributeHandleSet desiredAttributes;
            desiredAttributes.insert(amb->testcaseAttributeHandle);
            fed->rtiamb->attributeOwnershipAcquisition(amb->testcaseRTTObjectHandle, desiredAttributes, fed->generateTag());

            while (amb->owner == false) {  //or: while(!fed->rtiamb->isAttributeOwnedByFederate(  amb->testcaseRTTObjectHandle,amb->testcaseAttributeHandle)){
                fed->evoke();
            }
        } catch (Exception& e) {
            wcout << L"Failed to aquire ownership" << endl;
        }
    }
    //case 2: if have the ownership, i will change something before releasing the ownership
    else {
        try {
            AttributeHandleValueMap attributes;
            int val = amb->lastValue + 1;
            VariableLengthData encodedValue(&val, sizeof(val));
            attributes.insert(std::pair<AttributeHandle, VariableLengthData>(amb->testcaseAttributeHandle, encodedValue));
            fed->rtiamb->updateAttributeValues(amb->testcaseRTTObjectHandle, attributes, fed->generateTag());
        } catch (Exception& e) {
            wcout << L"Failed to update object attribute" << endl;
        }

        // wait for divest request
        while (amb->divestRequest == false) {
            fed->evoke();
        }
        fed->ownershipReleaseRequest(amb->target, amb->targetAtt);
    }

    //both roles will advance time
    fed->advanceTime(1);
}

void TestcaseRTT::finish() {
    if (amb->owner) {
        fed->deleteObject(amb->testcaseRTTObjectHandle);
    }
}