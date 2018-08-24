/*******************************************************************************
 *
 * (c) Copyright IBM Corp. 2018, 2018
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
 *******************************************************************************/

 #include <stdint.h>
 #include <cstring>
 #include <iostream>
 #include <fstream>

 #include "infra/Assert.hpp"
 #include "ilgen/JitBuilderReplayBinaryFile.hpp"
 #include "ilgen/MethodBuilder.hpp"
 #include "ilgen/JitBuilderReplay.hpp"
 #include "ilgen/TypeDictionary.hpp"
 #include "ilgen/IlBuilder.hpp"


 OMR::JitBuilderReplayBinaryFile::JitBuilderReplayBinaryFile(const char *fileName)
    : TR::JitBuilderReplay(), _file(fileName, std::fstream::in), _isFile(true)
    {
    start();
    #ifdef DEBUG
    std::cout << "JitBuilderReplayBinaryFile: constructor - file passed in" << '\n';
    #endif
    }

 OMR::JitBuilderReplayTextFile::JitBuilderReplayBinaryFile(std::string fileString)
    : TR::JitBuilderReplay(), _isFile(false), _fileStream(std::istringstream(fileString))
    {
    start();
    #ifdef DEBUG
    std::cout << "JitBuilderReplayBinaryFile: constructor - file as string passed in" << '\n';
    #endif
    }

 void
 OMR::JitBuilderReplayBinaryFile::start()
       {
       TR::JitBuilderReplay::start();
       processFirstLineFromBinaryFile();//Next function below
       }

void
OMR::JitBuilderReplayBinaryFile::processFirstLineFromBinaryFile()
        {
           uint8_t token;
           token = getFileAsByte();//Next function below

           // while (token != NULL) {
           //    token = std::strtok(NULL, SPACE);
           //   }
          }

uint8_t
OMR::JitBuilderReplayBinaryFile::getFileAsByte()
        {
             uint8_t oneByte;
          //   std::string line;

             if(_isFile)
                {
                _isFile.read(oneByte, sizeof(unsigned char));
                }
             else
                {
                _fileStream.read(oneByte, sizeof(unsigned char));
                }

             return oneByte;
          }


bool
OMR::JitBuilderReplayBinaryFile::parseConstructor()
   {
   #ifdef DEBUG
   std::cout << "JitBuilderReplayBinaryFile: Parsing Constructor..." << '\n';
   #endif

      // get a line
      // get tokens from the line
      // if DEF is seen: eg. Def A# "length [STRING]" --> 4 tokens
         // Def
         // A#
         // "length <-- use this length to find out how many chars to keep from next token after first char
         // [STRING]"
         // put pointer value at given ID in table --> what is this pointer value?????
      // if B[#] is seen == operation is seen: eg. B2 S4
         // B2
         uint8_t byte_1;
         uint8_t byte_2;
         bool constructorFlag = true;

         //std::string textLine;

         while(constructorFlag)
         {
            byte_1 = getFileAsByte();//04
            byte_2 = getFileAsByte();//00

            //00 04
            uint8_t original_byte = (uint8_t) ((byte_2 << 8) + byte_1);
            int count = (int)original_byte;
            //uint32_t id = getNumberFromToken(tokens);

            if(count == 0) // It's DEF
               {
                  addIDPointerPairToMap(tokens);
               }
            else   // Its probably a call to MethodBuilder
               {
                 while(count != 0){
                    char current_byte = (char)getFileAsByte(); //transform from uint8_t to char?
                    //concatenate those chars together

                    //Once reading in one character, minus count by 1:
                    count--;
                 }

                  constructorFlag = handleService(METHOD_BUILDER, tokens);
               }
         }
         return true;
   }
