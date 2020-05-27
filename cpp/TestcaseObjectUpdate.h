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

#ifndef Testcase_ObjectUpdate_H
#define Testcase_ObjectUpdate_H

#include "Testcase.h"

class TestcaseObjectUpdate : public Testcase {
   public:
    void init();
    void finish();
    void step(int i);
    ObjectInstanceHandle localObjectHandle;
    TestcaseObjectUpdate(std::string federatename, int role, int i, Federate* fed, FedAmb* amb);
    int counter;
};

#endif /*Testcase_ObjectUpdate_H*/
