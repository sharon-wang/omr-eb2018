###############################################################################
# Copyright (c) 2016, 2017 IBM Corp. and others
#
# This program and the accompanying materials are made available under
# the terms of the Eclipse Public License 2.0 which accompanies this
# distribution and is available at https://www.eclipse.org/legal/epl-2.0/
# or the Apache License, Version 2.0 which accompanies this distribution and
# is available at https://www.apache.org/licenses/LICENSE-2.0.
#
# This Source Code may also be made available under the following
# Secondary Licenses when the conditions for such availability set
# forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
# General Public License, version 2 with the GNU Classpath
# Exception [1] and GNU General Public License, version 2 with the
# OpenJDK Assembly Exception [2].
#
# [1] https://www.gnu.org/software/classpath/license.html
# [2] http://openjdk.java.net/legal/assembly-exception.html
#
# SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
###############################################################################

# Add our targets to the global targets
all: jit jitbuilder
clean: jit_clean
cleandeps: jit_cleandeps
cleandll: jit_cleandll

#
# Define our targets. "jit_clean" "jit_cleandeps" and "jit_cleandll" are double-colon so they can be appended to
# throughout the makefile.
#
.phony: jit jit_clean jit_cleandeps jit_cleandll
jit:
jit_clean::
jit_cleandeps::
jit_cleandll::

include $(JIT_MAKE_DIR)/rules/$(TOOLCHAIN)/common.mk

RELEASE_DIR=release
RELEASE_SRC=$(RELEASE_DIR)/src
RELEASE_INCLUDE=$(RELEASE_DIR)/include
JITBUILDER_TARBALL=jitbuilder.tgz

jitbuilder: $(JITBUILDER_TARBALL)

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/env:
	mkdir -p $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/env

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il:
	mkdir -p $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen:
	mkdir -p $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/infra:
	mkdir -p $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/infra

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/env/jittypes.h: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/env/jittypes.h $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/env
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/env/defines.h: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/env/defines.h $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/env
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/env/TypedAllocator.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/env/TypedAllocator.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/env
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il/DataTypes.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/il/DataTypes.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il/DataTypesEnum.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/il/DataTypesEnum.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il/DataTypes_inlines.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/il/DataTypes_inlines.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il/ILOpCodes.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/il/ILOpCodes.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il/OMRDataTypes.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/il/OMRDataTypes.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il/OMRDataTypes_inlines.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/il/OMRDataTypes_inlines.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il/ILOpCodesEnum.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/il/ILOpCodesEnum.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il/OMRILOpCodesEnum.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/il/OMRILOpCodesEnum.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il/ILHelpers.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/il/ILHelpers.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRIlType.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRIlType.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/IlType.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/IlType.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRIlValue.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRIlValue.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/IlValue.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/IlValue.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/IlInjector.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/IlInjector.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRIlBuilder.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRIlBuilder.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/IlBuilder.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/IlBuilder.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRIlBuilderRecorder.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRIlBuilderRecorder.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/IlBuilderRecorder.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/IlBuilderRecorder.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRMethodBuilder.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRMethodBuilder.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/MethodBuilder.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/MethodBuilder.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRMethodBuilderRecorder.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRMethodBuilderRecorder.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/MethodBuilderRecorder.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/MethodBuilderRecorder.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRMethodBuilderReplay.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRMethodBuilderReplay.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/MethodBuilderReplay.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/MethodBuilderReplay.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRThunkBuilder.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRThunkBuilder.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/ThunkBuilder.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/ThunkBuilder.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRBytecodeBuilder.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRBytecodeBuilder.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/BytecodeBuilder.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/BytecodeBuilder.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRBytecodeBuilderRecorder.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRBytecodeBuilderRecorder.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/BytecodeBuilderRecorder.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/BytecodeBuilderRecorder.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRTypeDictionary.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRTypeDictionary.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/TypeDictionary.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/TypeDictionary.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRJitBuilderRecorder.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRJitBuilderRecorder.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
		cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/JitBuilderRecorder.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/JitBuilderRecorder.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRJitBuilderRecorderBinaryBuffer.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRJitBuilderRecorderBinaryBuffer.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/JitBuilderRecorderBinaryBuffer.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/JitBuilderRecorderBinaryBuffer.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRJitBuilderRecorderBinaryFile.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRJitBuilderRecorderBinaryFile.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/JitBuilderRecorderBinaryFile.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/JitBuilderRecorderBinaryFile.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRJitBuilderRecorderTextFile.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRJitBuilderRecorderTextFile.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/JitBuilderRecorderTextFile.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/JitBuilderRecorderTextFile.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRJitBuilderReplay.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRJitBuilderReplay.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/JitBuilderReplay.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/JitBuilderReplay.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRJitBuilderReplayTextFile.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRJitBuilderReplayTextFile.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/JitBuilderReplayTextFile.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/JitBuilderReplayTextFile.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRVirtualMachineState.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRVirtualMachineState.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/VirtualMachineState.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/VirtualMachineState.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRVirtualMachineRegister.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRVirtualMachineRegister.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/VirtualMachineRegister.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/VirtualMachineRegister.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRVirtualMachineRegisterInStruct.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRVirtualMachineRegisterInStruct.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/VirtualMachineRegisterInStruct.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/VirtualMachineRegisterInStruct.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRVirtualMachineOperandArray.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRVirtualMachineOperandArray.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp -u $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/VirtualMachineOperandArray.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/VirtualMachineOperandArray.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp -u $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRVirtualMachineOperandStack.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRVirtualMachineOperandStack.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/VirtualMachineOperandStack.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/VirtualMachineOperandStack.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/IlGen.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/ilgen/IlGen.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen
	cp $< $@ || cp $< $@

$(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/infra/Annotations.hpp: $(FIXED_SRCBASE)/$(JIT_OMR_DIRTY_DIR)/infra/Annotations.hpp $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/infra
	cp $< $@ || cp $< $@

JITBUILDER_FILES=$(RELEASE_DIR)/Makefile \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/env/defines.h \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/env/jittypes.h \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/env/TypedAllocator.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il/DataTypes.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il/DataTypesEnum.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il/DataTypes_inlines.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il/ILHelpers.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il/ILOpCodes.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il/ILOpCodesEnum.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il/OMRDataTypes.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il/OMRDataTypes_inlines.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/il/OMRILOpCodesEnum.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRIlType.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/IlType.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRIlValue.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/IlValue.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/IlInjector.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRJitBuilderRecorder.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/JitBuilderRecorder.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRJitBuilderRecorderBinaryBuffer.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/JitBuilderRecorderBinaryBuffer.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRJitBuilderRecorderBinaryFile.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/JitBuilderRecorderBinaryFile.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRJitBuilderRecorderTextFile.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/JitBuilderRecorderTextFile.hpp \
			 	 $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRJitBuilderReplay.hpp \
			 	 $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/JitBuilderReplay.hpp \
				 $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRJitBuilderReplayTextFile.hpp \
				 $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/JitBuilderReplayTextFile.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRIlBuilderRecorder.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/IlBuilderRecorder.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRMethodBuilderRecorder.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/MethodBuilderRecorder.hpp \
				 $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRMethodBuilderReplay.hpp \
				 $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/MethodBuilderReplay.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRIlBuilder.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/IlBuilder.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRMethodBuilder.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/MethodBuilder.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRThunkBuilder.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/ThunkBuilder.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRBytecodeBuilder.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/BytecodeBuilder.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRBytecodeBuilderRecorder.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/BytecodeBuilderRecorder.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRTypeDictionary.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/TypeDictionary.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRVirtualMachineState.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/VirtualMachineState.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRVirtualMachineRegister.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/VirtualMachineRegister.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRVirtualMachineRegisterInStruct.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/VirtualMachineRegisterInStruct.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRVirtualMachineOperandArray.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/VirtualMachineOperandArray.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/OMRVirtualMachineOperandStack.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/VirtualMachineOperandStack.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/ilgen/IlGen.hpp \
             $(RELEASE_INCLUDE)/$(JIT_OMR_DIRTY_DIR)/infra/Annotations.hpp \
             $(RELEASE_SRC)/Call.hpp \
             $(RELEASE_SRC)/Call.cpp \
             $(RELEASE_SRC)/DotProduct.hpp \
             $(RELEASE_SRC)/DotProduct.cpp \
             $(RELEASE_SRC)/IterativeFib.hpp \
             $(RELEASE_SRC)/IterativeFib.cpp \
             $(RELEASE_SRC)/LinkedList.hpp \
             $(RELEASE_SRC)/LinkedList.cpp \
             $(RELEASE_SRC)/Mandelbrot.hpp \
             $(RELEASE_SRC)/Mandelbrot.cpp \
             $(RELEASE_SRC)/NestedLoop.hpp \
             $(RELEASE_SRC)/NestedLoop.cpp \
             $(RELEASE_SRC)/Pointer.hpp \
             $(RELEASE_SRC)/Pointer.cpp \
             $(RELEASE_SRC)/RecordReplayTest.hpp \
             $(RELEASE_SRC)/RecordReplayTest.cpp \
             $(RELEASE_SRC)/RecursiveFib.hpp \
             $(RELEASE_SRC)/RecursiveFib.cpp \
             $(RELEASE_SRC)/Simple.hpp \
             $(RELEASE_SRC)/Simple.cpp \
             $(RELEASE_SRC)/Switch.hpp \
             $(RELEASE_SRC)/Switch.cpp \
             $(RELEASE_SRC)/Pow2.hpp \
             $(RELEASE_SRC)/Pow2.cpp \


$(JITBUILDER_TARBALL) : $(JITBUILDER_FILES) $(JIT_PRODUCT_BACKEND_LIBRARY)
	cd $(RELEASE_DIR) && tar cvzf $(JITBUILDER_TARBALL) libjitbuilder.a Makefile README.md LICENSE include/ src/
