/*******************************************************************************
 *
 * (c) Copyright IBM Corp. 2016, 2017
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


#ifndef TR_ILBUILDERRECORDER_INCL
#define TR_ILBUILDERRECORDER_INCL

#include "ilgen/OMRIlBuilderRecorder.hpp"

namespace TR
{
   class IlBuilderRecorder : public OMR::IlBuilderRecorder
      {
      public:
         IlBuilderRecorder(TR::MethodBuilder *methodBuilder, TR::TypeDictionary *types)
            : OMR::IlBuilderRecorder(methodBuilder, types)
            { }

         IlBuilderRecorder(TR::IlBuilder *source)
            : OMR::IlBuilderRecorder(source)
            { }
      };

} // namespace TR

#endif // !defined(TR_ILBUILDERRECORDER_INCL)
