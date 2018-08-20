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

#ifndef TR_JITBUILDERRECORDER_BINARYFILE_INCL
#define TR_JITBUILDERRECORDER_BINARYFILE_INCL

#include "ilgen/OMRJitBuilderRecorderBinaryFile.hpp"

namespace TR
{
   class JitBuilderRecorderBinaryFile : public OMR::JitBuilderRecorderBinaryFile
      {
      public:
         JitBuilderRecorderBinaryFile(const TR::MethodBuilderRecorder *mb, const char *fileName)
            : OMR::JitBuilderRecorderBinaryFile(mb, fileName)
            { }
         virtual ~JitBuilderRecorderBinaryFile()
            { }
      };

} // namespace TR

#endif // !defined(TR_JITBUILDERRECORDER_BINARYFILE_INCL)
