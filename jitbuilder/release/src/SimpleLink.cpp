/*******************************************************************************
 *
 * (c) Copyright IBM Corp. 2016, 2016
 *
 *  This program and the accompanying materials are made available
 *  under the terms of the Eclipse Public License v1.0 and
 *  Apache License v2.0 which accompanies this distribution.
 *
 *      The Eclipse Public License is available at
 *      http://www.eclipse.org/legal/epl-v10.html
 *
 *      The Apache License v2.0 is available at
 *      http://www.opensource.org/licenses/apache2.0.php
 *
 * Contributors:
 *    Multiple authors (IBM Corp.) - initial implementation and documentation
 ******************************************************************************/


#include <iostream>
#include <stdlib.h>
#include <stdint.h>

using std::cout;
using std::cerr;

extern "C" int32_t increment(int32_t);
extern "C" int32_t add_one(int32_t);

int
main(int argc, char *argv[])
   {
   {
   cout << "Step 1: Invoke linked compiled code and print results:\n";
   int32_t v;
   v=0;   cout << "increment(" << v << ") == " << increment(v) << "\n";
   v=1;   cout << "increment(" << v << ") == " << increment(v) << "\n";
   v=10;  cout << "increment(" << v << ") == " << increment(v) << "\n";
   v=-15; cout << "increment(" << v << ") == " << increment(v) << "\n";
   }
   {
   cout << "Step 2: Invoke linked compiled code and print results:\n";
   int32_t v;
   v=0;   cout << "add_one(" << v << ") == " << add_one(v) << "\n";
   v=1;   cout << "add_one(" << v << ") == " << add_one(v) << "\n";
   v=10;  cout << "add_one(" << v << ") == " << add_one(v) << "\n";
   v=-15; cout << "add_one(" << v << ") == " << add_one(v) << "\n";
   }
   return 0;
   }
