/*******************************************************************************
 * Copyright (c) 2016, 2018 IBM Corp. and others
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
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#include "ilgen/VirtualMachineState.hpp"

namespace TR { class IlBuilder; }
class TR_Memory;

template <class T> class List;
template <class T> class ListAppender;


void
OMR::VirtualMachineState::Commit(TR::IlBuilder *b)
   {
   if (_clientCallbackCommit)
      (*_clientCallbackCommit)(this, b->client());
   }

void
OMR::VirtualMachineState::Reload(TR::IlBuilder *b)
   {
   if (_clientCallbackReload)
      (*_clientCallbackReload)(this, b->client());
   }

TR::VirtualMachineState *
OMR::VirtualMachineState::MakeCopy()
   {
   if (_clientCallbackMakeCopy)
      return (TR::VirtualMachineState *) (*_clientCallbackMakeCopy)(this);

   return static_cast<TR::VirtualMachineState *>(this);
   }

void
OMR::VirtualMachineState::MergeInto(TR::VirtualMachineState *other, TR::IlBuilder *b)
   {
   if (_clientCallbackMergeInto)
      (*_clientCallbackMergeInto)(this, other->client(), b->client());
   }
