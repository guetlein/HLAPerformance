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
 * Send an interaction with specific payload size.
 * 
 ******************************************************************************/

#include "TestcaseInteraction.h"
TestcaseInteraction::TestcaseInteraction(std::string n, int role, int i, Federate* f, FedAmb* a) : Testcase(n, role, i, f, a) {
}

void TestcaseInteraction::init() {
    wcout << L"Running TestcaseInteraction" << endl;
}

void TestcaseInteraction::step(int i) {
    if (role == 0)
        ParameterHandleValueMap parameters;
    const char* payload = new char[fed->interactionParamSize];
    VariableLengthData encodedValue((void*)payload, fed->interactionParamSize + 1);
    parameters.insert(std::pair<ParameterHandle, VariableLengthData>(amb->testcaseInteractionParamHandle, encodedValue));

    fed->rtiamb->sendInteraction(amb->testcaseInteractionHandle, parameters, fed->generateTag());

    delete payload;
}
fed->advanceTime();
}
