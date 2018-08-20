/*******************************************************************************
 *
 * (c) Copyright IBM Corp. 2017, 2018
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

#ifndef TR_BYTECODE_BUILDERRECORDER_INCL
#define TR_BYTECODE_BUILDERRECORDER_INCL

#include "ilgen/OMRBytecodeBuilderRecorder.hpp"

namespace TR
{
   class BytecodeBuilderRecorder : public OMR::BytecodeBuilderRecorder
      {
      public:
         BytecodeBuilderRecorder(TR::MethodBuilder *methodBuilder, int32_t bcIndex, char *name=NULL)
            : OMR::BytecodeBuilderRecorder(methodBuilder, bcIndex, name)
            { }
      };

} // namespace TR

#endif // !defined(TR_ILBUILDERRECORDER_INCL)
