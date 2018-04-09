/*******************************************************************************
 * Copyright (c) 2018, 2018 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at http://eclipse.org/legal/epl-2.0
 * or the Apache License, Version 2.0 which accompanies this distribution
 * and is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following Secondary
 * Licenses when the conditions for such availability set forth in the
 * Eclipse Public License, v. 2.0 are satisfied: GNU General Public License,
 * version 2 with the GNU Classpath Exception [1] and GNU General Public
 * License, version 2 with the OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
 *******************************************************************************/


#include "SourceFile.hpp"

using namespace std;

static const char *
copyrightHeader[] =
   {
   "Copyright (c) 2018, 2018 IBM Corp. and others\n",
   "",
   "This program and the accompanying materials are made available under\n",
   "the terms of the Eclipse Public License 2.0 which accompanies this\n",
   "distribution and is available at http://eclipse.org/legal/epl-2.0\n",
   "or the Apache License, Version 2.0 which accompanies this distribution\n",
   "and is available at https://www.apache.org/licenses/LICENSE-2.0.\n",
   "\n",
   "This Source Code may also be made available under the following Secondary\n",
   "Licenses when the conditions for such availability set forth in the\n",
   "Eclipse Public License, v. 2.0 are satisfied: GNU General Public License,\n",
   "version 2 with the GNU Classpath Exception [1] and GNU General Public\n",
   "License, version 2 with the OpenJDK Assembly Exception [2].\n",
   "\n",
   "[1] https://www.gnu.org/software/classpath/license.html\n",
   "[2] http://openjdk.java.net/legal/assembly-exception.html\n",
   "\n",
   "SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception\n",
   };
const int copyrightHeaderLines = sizeof(copyrightHeader) / sizeof(const char *);

void
SourceFile::prolog()
   {
   indent() << startCopyrightComment();
   for (int l=0;l < copyrightHeaderLines;l++)
      {
      indent() << copyrightLineStart() << copyrightHeader[l];
      }
   indent() << endCopyrightComment();

   _s << "\n";
   string autoGenerated("This file auto-generated. Do not edit directly.");
   comment(autoGenerated);
   _s << "\n\n";
   }