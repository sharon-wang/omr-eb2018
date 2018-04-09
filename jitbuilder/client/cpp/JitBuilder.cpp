/*******************************************************************************
 * Copyright (c) 2014, 2018 IBM Corp. and others
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

#include "ilgen/IlBuilder.hpp"
#include "ilgen/MethodBuilder.hpp"

#include "release/cpp/include/JitBuilder.hpp"
#include "release/cpp/include/MethodBuilder.hpp"
#include "release/cpp/include/VirtualMachineState.hpp"

extern bool internal_initializeJitWithOptions(char *options);
extern bool internal_initializeJit();
extern int32_t internal_compileMethodBuilder(TR::MethodBuilder *mb, void **entry);
extern void internal_shutdownJit();

extern "C"
{

bool
IlBuilderCallback_buildIL(void *clientObj)
   {
   OMR::JitBuilder::IlBuilder *b = (OMR::JitBuilder::IlBuilder *)clientObj;
   return b->buildIL();
   }

bool
MethodBuilderCallback_RequestFunction(void *clientObj, char *name)
   {
   OMR::JitBuilder::MethodBuilder *mb = (OMR::JitBuilder::MethodBuilder *)clientObj;
   return mb->RequestFunction((const char *)name);
   }

void
VirtualMachineStateCallback_Commit(void *clientObj, void *builderObj)
   {
   OMR::JitBuilder::VirtualMachineState *s = (OMR::JitBuilder::VirtualMachineState *)clientObj;
   OMR::JitBuilder::IlBuilder *b = (OMR::JitBuilder::IlBuilder *)builderObj;
   s->Commit(b);
   }

void
VirtualMachineStateCallback_Reload(void *clientObj, void *builderObj)
   {
   OMR::JitBuilder::VirtualMachineState *s = (OMR::JitBuilder::VirtualMachineState *)clientObj;
   OMR::JitBuilder::IlBuilder *b = (OMR::JitBuilder::IlBuilder *)builderObj;
   s->Reload(b);
   }

void *
VirtualMachineStateCallback_MakeCopy(void *clientObj)
   {
   OMR::JitBuilder::VirtualMachineState *s = (OMR::JitBuilder::VirtualMachineState *)clientObj;
   return s->MakeCopy();
   }

void
VirtualMachineStateCallback_MergeInto(void *clientObj, void *otherObj, void *builder_otherObj)
   {
   OMR::JitBuilder::VirtualMachineState *s = (OMR::JitBuilder::VirtualMachineState *)clientObj;
   OMR::JitBuilder::VirtualMachineState *other = (OMR::JitBuilder::VirtualMachineState *)otherObj;
   OMR::JitBuilder::IlBuilder *b_other = (OMR::JitBuilder::IlBuilder *)builder_otherObj;
   s->MergeInto(other, b_other);
   }
}

bool
OMR::JitBuilder::IlBuilder::buildIL()
   {
   return false;
   }

bool
OMR::JitBuilder::MethodBuilder::RequestFunction(const char *name)
   {
   return false;
   }

void
OMR::JitBuilder::VirtualMachineState::Commit(OMR::JitBuilder::IlBuilder *b)
   {
   }

void
OMR::JitBuilder::VirtualMachineState::Reload(OMR::JitBuilder::IlBuilder *b)
   {
   }

OMR::JitBuilder::VirtualMachineState *
OMR::JitBuilder::VirtualMachineState::MakeCopy()
   {
   return this; // no actual state in OMR::JitBuilderVirtualMachineState, so just use the same object
   }

void
OMR::JitBuilder::VirtualMachineState::MergeInto(OMR::JitBuilder::VirtualMachineState *s, OMR::JitBuilder::IlBuilder *b)
   {
   }

bool
initializeJitWithOptions(char *options)
   {
   return internal_initializeJitWithOptions(options);
   }

bool
initializeJit()
   {
   return internal_initializeJit();
   }

int32_t
compileMethodBuilder(OMR::JitBuilder::MethodBuilder *m, void **entry)
   {
   return internal_compileMethodBuilder((TR::MethodBuilder *)(m->_impl), entry);
   }

void
shutdownJit()
   {
   internal_shutdownJit();
   }
