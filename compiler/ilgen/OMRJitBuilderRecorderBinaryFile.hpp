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

#ifndef OMR_JITBUILDERRECORDER_BINARYFILE_INCL
#define OMR_JITBUILDERRECORDER_BINARYFILE_INCL

#include "ilgen/JitBuilderRecorderBinaryBuffer.hpp"
#include <fstream>

namespace TR { class IlBuilderRecorder; }
namespace TR { class MethodBuilderRecorder; }
namespace TR { class IlType; }
namespace TR { class IlValue; }

namespace OMR
{

class JitBuilderRecorderBinaryFile : public TR::JitBuilderRecorderBinaryBuffer
   {
   public:
   JitBuilderRecorderBinaryFile(const TR::MethodBuilderRecorder *mb, const char *fileName);
   virtual ~JitBuilderRecorderBinaryFile() { }

   virtual void Close();

   private:
   std::fstream _file;
   };

} // namespace OMR

#endif // !defined(OMR_JITBUILDERRECORDER_BINARYFILE_INCL)
