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
 * Do nothing more than updating the value of an object and advanding the time.
 *
 ******************************************************************************/


#include "TestcaseObjectUpdate.h"
TestcaseObjectUpdate::TestcaseObjectUpdate(std::string n, int role, int i, Federate* f, FedAmb* a) : Testcase(n, role, i, f, a) {
    counter = 0;
}
void TestcaseObjectUpdate::init() {
    wcout << L"Running TestcaseObjectUpdate" << endl;

    if (role == 0)
        localObjectHandle = fed->registerObject(L"HLAobjectRoot.TestcaseObject");
}

void TestcaseObjectUpdate::step(int i) {
    if (role == 0) {
        AttributeHandleValueMap attributes;
        counter++;
        VariableLengthData encodedValue(&counter, sizeof(counter));
        attributes.insert(std::pair<AttributeHandle, VariableLengthData>(amb->testcaseAttributeHandle, encodedValue));
        fed->rtiamb->updateAttributeValues(localObjectHandle, attributes, fed->generateTag());
    }
    // request a time advance and wait until we get it
    fed->advanceTime();
}

void TestcaseObjectUpdate::finish() {
    if (role == 0)
        fed->deleteObject(localObjectHandle);
}
