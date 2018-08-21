/*******************************************************************************
 * Copyright (c) 2016, 2018 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 * or the Apache License, Version 2.0 which accompanies this distribution and
 * is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following
 * Secondary Licenses when the conditions for such availability set
 * forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 * General Public License, version 2 with the GNU Classpath
 * Exception [1] and GNU General Public License, version 2 with the
 * OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string>
#include <cstring>

#include "Jit.hpp"
#include "ilgen/TypeDictionary.hpp"
#include "ilgen/JitBuilderRecorderTextFile.hpp"
#include "ilgen/JitBuilderReplayTextFile.hpp"
#include "ilgen/MethodBuilderReplay.hpp"
#include "Simple.hpp"

using std::cout;
using std::cerr;

int
main(int argc, char *argv[])
   {
   cout << "Step 1: initialize JIT\n";
   bool initialized = initializeJit();
   if (!initialized)
      {
      cerr << "FAIL: could not initialize JIT\n";
      exit(-1);
      }

   cout << "Step 2: define type dictionary\n";
   TR::TypeDictionary types;
   // Create a recorder so we can directly control the file for this particular test
   TR::JitBuilderRecorderTextFile recorder(NULL, "simple.out");

   cout << "Step 3: compile method builder\n";
   SimpleMethod method(&types, &recorder);

   uint8_t *entry = 0;
   int32_t rc = recordMethodBuilder(&method);
   if (rc != 0)
      {
      cerr << "FAIL: compilation error " << rc << "\n";
      exit(-2);
      }

   cout << "Step 4: Replay\n";
   TR::JitBuilderReplayTextFile replay("simple.out");
   TR::JitBuilderRecorderTextFile recorder2(NULL, "simple2.out");

   TR::TypeDictionary types2;

   cout << "Step 5: verify output file\n";
   TR::MethodBuilderReplay mb(&types2, &replay, &recorder2); // Process Constructor
   rc = compileMethodBuilder(&mb, &entry); // Process buildIL

   if (rc != 0)
      {
      cerr << "FAIL: compilation error " << rc << "\n";
      exit(-2);
      }

   cout << "Step 6: run compiled code from replay\n";
   typedef int32_t (SimpleMethodFunction)(int32_t);
   // typedef int32_t (SimpleMethodFunction)();
   SimpleMethodFunction *increment = (SimpleMethodFunction *) entry;
   int32_t v;
   v=0; cout << "increment(" << v << ") == " << increment(v) << "\n";
   v=1; cout << "increment(" << v << ") == " << increment(v) << "\n";
   v=10; cout << "increment(" << v << ") == " << increment(v) << "\n";
   v=-15; cout << "increment(" << v << ") == " << increment(v) << "\n";

   cout << "Step 7: shutdown JIT\n";
   shutdownJit();
   }


SimpleMethod::SimpleMethod(TR::TypeDictionary *d, TR::JitBuilderRecorder *recorder)
   : MethodBuilder(d, NULL, recorder)
   {

   DefineLine(LINETOSTR(__LINE__));
   // cout << LINETOSTR(__LINE__) << " // CHECKING VALUE OF DEFINELINE \n";
   DefineFile(__FILE__);

   DefineName("increment");

   // ORIGINAL SIMPLE.cpp
   DefineParameter("value", Int32);

   // COMPLICATED SIMPLE.cpp
   // DefineParameter("value1", Int32);
   DefineReturnType(Int32);
   }

bool
SimpleMethod::buildIL()
   {
   cout << "SimpleMethod::buildIL() running!\n";

   // MOST SIMPLE SIMPLE.cpp
   // Return(
   //    ConstInt32(88));

   // *****************************************************************
   // ORIGINAL SIMPLE.cpp
   Return(
      Add(
         Load("value"),
         ConstInt32(1)));

   // *****************************************************************
   // COMPLICATED SIMPLE.cpp
   // Return(
   //    Add(
   //      Add(
   //        Load("value1"),
   //        ConstInt32(17)
   //      ),
   //      Add(
   //        Load("value2"),
   //        ConstInt32(14))));

  // IlValue * value1 = Load("value1");
   return true;
   }
