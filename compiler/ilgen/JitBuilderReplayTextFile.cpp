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
 #include "ilgen/JitBuilderReplayTextFile.hpp"
 #include "ilgen/MethodBuilder.hpp"
 #include "ilgen/JitBuilderReplay.hpp"
 #include "ilgen/TypeDictionary.hpp"
  #include "ilgen/IlBuilder.hpp"

 OMR::JitBuilderReplayTextFile::JitBuilderReplayTextFile(const char *fileName)
    : TR::JitBuilderReplay(), _file(fileName, std::fstream::in), _isFile(true)
    {
    start();
    std::cout << "File constructor..." << '\n';
    std::cout << ">>> Start reading 2nd line and BEYOND!" << '\n';
    }

 OMR::JitBuilderReplayTextFile::JitBuilderReplayTextFile(std::string fileString)
    : TR::JitBuilderReplay(), _isFile(false), _fileStream(std::istringstream(fileString))
    {
    start();
    std::cout << "File as a string constructor..." << '\n';
    std::cout << ">>> Start reading 2nd line and BEYOND!" << '\n';
    }

void
OMR::JitBuilderReplayTextFile::start()
   {
   TR::JitBuilderReplay::start();
   processFirstLineFromTextFile();
   }

char *
OMR::JitBuilderReplayTextFile::getLineAsChar()
   {
   char * lineAsChar;
   std::string line;

   if(_isFile)
      {
      std::getline(_file, line);
      }
   else
      {
      std::getline(_fileStream, line);
      }

   lineAsChar = getTokensFromLine(line);
   return lineAsChar;
   }

void
OMR::JitBuilderReplayTextFile::processFirstLineFromTextFile()
   {
    char * token;
    token = getLineAsChar();

    while (token != NULL) {
       token = std::strtok(NULL, SPACE);
      }
   }

char *
OMR::JitBuilderReplayTextFile::getTokensFromLine(std::string currentLine)
   {
      char * line = strdup(currentLine.c_str());
      char * token = std::strtok(line, SPACE);

      return token;
   }

uint32_t
OMR::JitBuilderReplayTextFile::getNumberFromToken(char * token)
   {
       uint32_t IDtoReturn;

       if(isdigit(token[0]))
          return atoi(&token[0]);

       if(token == NULL)
          {
             std::cerr << "Token is NULL.\n";
             IDtoReturn = 0;
          }
       else if(token[1])
          {
             IDtoReturn = atoi(&token[1]);
             // std::cout << "Token found: " << token << ". uint32_t #: " << IDtoReturn << '\n';
          }
       else
          {
             TR_ASSERT_FATAL(0, "Unexpected short token: %s", token);
          }
       return IDtoReturn;
   }

void
OMR::JitBuilderReplayTextFile::addIDPointerPairToMap(char * tokens)
   {
       // Tokes: "Def S3 [16 "DefileLineString"]"
       tokens = std::strtok(NULL, SPACE);
       uint32_t ID = getNumberFromToken(tokens);
       tokens = std::strtok(NULL, SPACE);
       uint32_t strLen = getNumberFromToken(tokens);
       tokens = std::strtok(NULL, SPACE);

       char * idPointer = getServiceStringFromToken(strLen, tokens);

       // std::cout << "Storing ID: " << ID << ", and pointer: " << idPointer << ", to the map." << '\n';
       StoreIDPointerPair(ID, idPointer);
   }

char *
OMR::JitBuilderReplayTextFile::getServiceStringFromToken(uint32_t strLen, char * tokens)
   {
     char idPointerStr[strLen];
     strncpy(idPointerStr, &tokens[1], sizeof(idPointerStr));
     idPointerStr[strLen] = '\0';

     return strdup(idPointerStr);
   }

const char *
OMR::JitBuilderReplayTextFile::getServiceStringFromMap(char ** tokens)
   {
     char * temp = *tokens;
     temp = std::strtok(NULL, SPACE);
     uint32_t serviceID = getNumberFromToken(temp);

     const char * serviceString = static_cast<const char *>(lookupPointer(serviceID));
     temp = std::strtok(NULL, SPACE);

     *tokens = temp;

     return serviceString;
   }

bool
OMR::JitBuilderReplayTextFile::handleService(BuilderFlag builderFlag, char * tokens)
   {
       bool handleServiceFlag = true;

       uint32_t mbID = getNumberFromToken(tokens);

       if(builderFlag == METHOD_BUILDER)
          {
              // divides work into handleServiceMethodBuilder and handleServiceIlBuilder
              handleServiceFlag = handleServiceMethodBuilder(mbID, tokens);
          }
       else if(builderFlag == IL_BUILDER)
          {
              // divided work for handleServiceIlBuilder
              handleServiceFlag = handleServiceIlBuilder(mbID, tokens);
          }

       return handleServiceFlag;
   }

bool
OMR::JitBuilderReplayTextFile::handleServiceMethodBuilder(uint32_t mbID, char * tokens)
   {
   TR::MethodBuilder * mb = static_cast<TR::MethodBuilder *>(lookupPointer(mbID));

   const char * serviceString = getServiceStringFromMap(&tokens);

   // At this point we already have a builder and the service string to be called
   // Therefore, tokens will contain 1 to 3 tokens at the most

   // Here we have the builder and the service to be called
   // Starting to check if else if statements to call each service
   if(strcmp(serviceString, STATEMENT_NEWMETHODBUILDER) == 0)
      {
      handleMethodBuilder(mbID, tokens);
      }
   else if(strcmp(serviceString, STATEMENT_DEFINELINESTRING) == 0)
      {
      handleDefineLine(mb, tokens);
      }
   else if(strcmp(serviceString, STATEMENT_DEFINEFILE) == 0)
      {
      handleDefineFile(mb, tokens);
      }
   else if(strcmp(serviceString, STATEMENT_DEFINENAME) == 0)
      {
      handleDefineName(mb, tokens);
      }
   else if(strcmp(serviceString, STATEMENT_DEFINEPARAMETER) == 0)
      {
      handleDefineParameter(mb, tokens);
      }
   else if(strcmp(serviceString, STATEMENT_DEFINEARRAYPARAMETER) == 0)
      {
      handleDefineArrayParameter(mb, tokens);
      }
   else if(strcmp(serviceString, STATEMENT_DEFINELOCAL) == 0)
      {
      handleDefineLocal(mb, tokens);
      }
   else if(strcmp(serviceString, STATEMENT_PRIMITIVETYPE) == 0)
      {
      handlePrimitiveType(mb, tokens);
      }
   else if(strcmp(serviceString, STATEMENT_POINTERTYPE) == 0)
      {
      handlePointerType(mb, tokens);
      }
   else if(strcmp(serviceString, STATEMENT_DEFINERETURNTYPE) == 0)
      {
      handleDefineReturnType(mb, tokens);
      }
   else if(strcmp(serviceString, STATEMENT_DEFINEFUNCTION) == 0)
      {
      handleDefineFunction(mb, tokens);
      }
   else if(strcmp(serviceString, STATEMENT_ALLLOCALSHAVEBEENDEFINED) == 0)
      {
      handleAllLocalsHaveBeenDefined(mb, tokens);
      }
   else if(strcmp(serviceString, STATEMENT_DONECONSTRUCTOR) == 0)
      {
      std::cout << "Constructor DONE. About to call buildIl.\n";
      return false;
      }
   else
      {
      TR_ASSERT_FATAL(0, "handleServiceMethodBuilder asked to handle unknown serive %s", serviceString);
      }

   return true;
   }


bool
OMR::JitBuilderReplayTextFile::handleServiceIlBuilder(uint32_t mbID, char * tokens)
   {
      TR::IlBuilder * ilmb = static_cast<TR::IlBuilder *>(lookupPointer(mbID));

      const char * serviceString = getServiceStringFromMap(&tokens);

      if(strcmp(serviceString, STATEMENT_DEFINELOCAL) == 0)
         {
         handleDefineLocal(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_ALLLOCALSHAVEBEENDEFINED) == 0)
         {
         handleAllLocalsHaveBeenDefined(static_cast<TR::MethodBuilder *>(ilmb), tokens);
         }
      else if(strcmp(serviceString, STATEMENT_CONSTINT8) == 0)
         {
         handleConstInt8(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_CONSTINT32) == 0)
         {
         handleConstInt32(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_CONSTINT64) == 0)
         {
         handleConstInt64(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_CONSTDOUBLE) == 0)
         {
         handleConstDouble(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_CONSTADDRESS) == 0)
         {
         handleConstAddress(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_LOAD) == 0)
         {
         handleLoad(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_LOADAT) == 0)
         {
         handleLoadAt(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_ADD) == 0)
         {
         handleAdd(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_RETURNVALUE) == 0)
         {
         handleReturnValue(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_STORE) == 0)
         {
         handleStore(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_STOREAT) == 0)
         {
         handleStoreAt(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_SUB) == 0)
         {
         handleSub(ilmb, tokens);
<<<<<<< HEAD
      }
      else if (strcmp(serviceString, STATEMENT_LESSTHAN) == 0) {
        handleLessThan(ilmb, tokens);
      }
      else if(strcmp(serviceString, STATEMENT_NEWILBUILDER) == 0) {
        handleNewIlBuilder(ilmb, tokens);
      }
      else if(strcmp(serviceString, STATEMENT_IFTHENELSE) == 0){
        handleIfThenElse(ilmb, tokens);
      }
      else if(strcmp(serviceString, STATEMENT_POINTERTYPE) == 0){
        handlePointerType(ilmb, tokens);
      }
      else if(strcmp(serviceString, STATEMENT_CREATELOCALARRAY) == 0){
        handleCreateLocalArray(ilmb, tokens);
      }
      //Needs to be implemented:
      else if(strcmp(serviceString, STATEMENT_INDEXAT) == 0){
        handleIndexAt(ilmb, tokens);
      }
      else if(strcmp(serviceString, STATEMENT_STOREAT) == 0){
        handleStoreAt(ilmb, tokens);
      }
      else if(strcmp(serviceString, STATEMENT_LOADAT) == 0){
        handleLoadAt(ilmb, tokens);
      }
      else{
          TR_ASSERT_FATAL(0, "handleServiceIlBuilder asked to handle unknown serive %s", serviceString);
=======
         }
      else if(strcmp(serviceString, STATEMENT_MUL) == 0)
         {
         handleMul(ilmb, tokens);
         }      
      else if(strcmp(serviceString, STATEMENT_DIV) == 0)
         {
         handleDiv(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_AND) == 0)
         {
         handleAnd(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_OR) == 0)
         {
         handleOr(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_XOR) == 0)
         {
         handleXor(ilmb, tokens);
         }
      else if (strcmp(serviceString, STATEMENT_LESSTHAN) == 0) 
         {
         handleLessThan(ilmb, tokens);
         }
      else if (strcmp(serviceString, STATEMENT_GREATERTHAN) == 0) 
         {
         handleGreaterThan(ilmb, tokens);
         }
      else if (strcmp(serviceString, STATEMENT_NOTEQUALTO) == 0) 
         {
         handleNotEqualTo(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_NEWILBUILDER) == 0) 
         {
         handleNewIlBuilder(ilmb, tokens);
         } 
      else if(strcmp(serviceString, STATEMENT_IFTHENELSE) == 0)
         {
         handleIfThenElse(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_FORLOOP) == 0)
         {
         handleForLoop(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_CALL) == 0)
         {
         handleCall(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_CONVERTTO) == 0)
         {
         handleConvertTo(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_UNSIGNEDSHIFTR) == 0)
         {
         handleUnsignedShiftR(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_IFCMPEQUALZERO) == 0)
         {
         handleIfCmpEqualZero(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_INDEXAT) == 0)
         {
         handleIndexAt(ilmb, tokens);
         }
      else if(strcmp(serviceString, STATEMENT_CREATELOCALARRAY) == 0)
         {
         handleCreateLocalArray(ilmb, tokens);
         }
      else
         {
         TR_ASSERT_FATAL(0, "handleServiceIlBuilder asked to handle unknown serive %s", serviceString);
>>>>>>> 353620cb23170059e785b2e12d9ec1643059c02f
         }

      return true;
   }

void
OMR::JitBuilderReplayTextFile::handleMethodBuilder(uint32_t serviceID, char * tokens)
   {
      // Sanity check to see if ID from token matches ID from methodBuilder
      std::cout << "Calling handleMethodBuilder helper.\n";

      uint32_t mbID = getNumberFromToken(tokens);
      if(serviceID == mbID)
         {
         std::cout << "MethodBuilder IDs match passed.\n";
         }
      else
         {
         TR_ASSERT_FATAL(0, "MethodBuilder IDs do not match. Found ID %d in map while %d was passed as a parameter.", serviceID, mbID);
         }
   }

void
OMR::JitBuilderReplayTextFile::handleDefineLine(TR::MethodBuilder * mb, char * tokens)
   {
      std::cout << "Calling handleDefineLine helper.\n";
      uint32_t strLen = getNumberFromToken(tokens);
      tokens = std::strtok(NULL, SPACE);

      const char * defineLineParam = getServiceStringFromToken(strLen, tokens);

      mb->DefineLine(defineLineParam);
   }

void
OMR::JitBuilderReplayTextFile::handleDefineFile(TR::MethodBuilder * mb, char * tokens)
   {
     std::cout << "Calling handleDefineFile helper.\n";
     uint32_t strLen = getNumberFromToken(tokens);
     tokens = std::strtok(NULL, SPACE);

     const char * defineFileParam = getServiceStringFromToken(strLen, tokens);

     mb->DefineFile(defineFileParam);
   }

void
OMR::JitBuilderReplayTextFile::handleDefineName(TR::MethodBuilder * mb, char * tokens)
   {
     std::cout << "Calling handleDefineName helper.\n";
     uint32_t strLen = getNumberFromToken(tokens);
     tokens = std::strtok(NULL, SPACE);

     const char * defineNameParam = getServiceStringFromToken(strLen, tokens);

     mb->DefineName(defineNameParam);
   }

void
OMR::JitBuilderReplayTextFile::handleDefineParameter(TR::MethodBuilder * mb, char * tokens)
   {
    // B2 S9 T7 "5 [value]"
    // DefineParameter("value", Int32);
    std::cout << "Calling handleDefineParameter helper.\n";

    uint32_t typeID = getNumberFromToken(tokens);
    TR::IlType *type = static_cast<TR::IlType *>(lookupPointer(typeID));
    tokens = std::strtok(NULL, SPACE);

    uint32_t strLen = getNumberFromToken(tokens);
    tokens = std::strtok(NULL, SPACE);
    const char * defineParameterParam = getServiceStringFromToken(strLen, tokens);

    mb->DefineParameter(defineParameterParam, type);
   }

void
OMR::JitBuilderReplayTextFile::handleDefineArrayParameter(TR::MethodBuilder * mb, char * tokens)
   {
    // Def S13 "20 [DefineArrayParameter]" 
    // B2 S13 T10 "6 [buffer]" 
    std::cout << "Calling handleDefineArrayParameter helper.\n";

    uint32_t typeID = getNumberFromToken(tokens);
    TR::IlType *type = static_cast<TR::IlType *>(lookupPointer(typeID));
    tokens = std::strtok(NULL, SPACE);

    uint32_t strLen = getNumberFromToken(tokens);
    tokens = std::strtok(NULL, SPACE);
    const char * defineParameterParam = getServiceStringFromToken(strLen, tokens);

    mb->DefineArrayParameter(defineParameterParam, type);
   }

void OMR::JitBuilderReplayTextFile::handlePrimitiveType(TR::MethodBuilder * mb, char * tokens)
   {
    std::cout << "Calling handlePrimitiveType helper.\n";
    // Store IlType in map with respective ID from tokens
    // tokens: "T7 3"
    // Key: 7, Value: IlValue(3)
    uint32_t ID = getNumberFromToken(tokens); //"7"
    tokens = std::strtok(NULL, SPACE);
    uint32_t value = getNumberFromToken(tokens); //"3"

    TR::DataType dt((TR::DataTypes)value);
    TR::IlType *type = mb->typeDictionary()->PrimitiveType(dt);

    StoreIDPointerPair(ID, type);
   }

void
OMR::JitBuilderReplayTextFile::handleDefineReturnType(TR::MethodBuilder * mb, char * tokens)
   {
     std::cout << "Calling handleDefineReturnType helper.\n";
     uint32_t ID = getNumberFromToken(tokens);

     TR::IlType * returnTypeParam = static_cast<TR::IlType *>(lookupPointer(ID));

     mb->DefineReturnType(returnTypeParam);
   }

void
OMR::JitBuilderReplayTextFile::handleDefineFunction(TR::MethodBuilder * mb, char * tokens)
   {
   //Def S13 "14 [DefineFunction]"
   //B2 S13 "11 [printString]" "68 [/Users/charliegracie/git/omr/jitbuilder/release/src/RecursiveFib.cpp]" "2 [67]" {0x102ef45c0} T11 1 T12
   std::cout << "Calling handleDefineFunction helper.\n";

   uint32_t strLen = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);
   const char * functionName = getServiceStringFromToken(strLen, tokens);
   tokens = std::strtok(NULL, SPACE);

   strLen = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);
   const char * fileName = getServiceStringFromToken(strLen, tokens);
   tokens = std::strtok(NULL, SPACE);

   strLen = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);
   const char * lineNumber = getServiceStringFromToken(strLen, tokens);
   tokens = std::strtok(NULL, SPACE);

   int64_t entryPoint = strtol(tokens + 1, NULL, 16);
   tokens = std::strtok(NULL, SPACE);

   uint32_t returnTypeID = getNumberFromToken(tokens);
   TR::IlType * returnType = static_cast<TR::IlType *>(lookupPointer(returnTypeID));
   tokens = std::strtok(NULL, SPACE);

   uint32_t paramCount = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);

   TR::IlType **paramTypes = (TR::IlType **)malloc(paramCount * sizeof(TR::IlType *));
   TR_ASSERT_FATAL(NULL != paramTypes, "Unable to allocate values array for handleDefineFunction");

   for (int i = 0; i < paramCount; i++)
      {
      uint32_t valueID = getNumberFromToken(tokens);
      TR::IlType * value = static_cast<TR::IlType *>(lookupPointer(valueID));
      tokens = std::strtok(NULL, SPACE);

      paramTypes[i] = value;
      }

   mb->DefineFunction(functionName, fileName, lineNumber, (void *) entryPoint, returnType, (int32_t)paramCount, paramTypes);

   //TODO free paramTypes......
   }

void
OMR::JitBuilderReplayTextFile::handleDefineLocal(TR::MethodBuilder *mb, char *tokens)
   {
   //B2 S25 "6 [result]" T7
   std::cout << "Calling handleDefineLocal helper - mb.\n";

   uint32_t typeID = getNumberFromToken(tokens);
   TR::IlType *type = static_cast<TR::IlType *>(lookupPointer(typeID));
   tokens = std::strtok(NULL, SPACE);

   uint32_t strLen = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);
   const char * defineLocalParam = getServiceStringFromToken(strLen, tokens);

   mb->DefineLocal(defineLocalParam, type);
   }

void
OMR::JitBuilderReplayTextFile::handleDefineLocal(TR::IlBuilder *ilmb, char *tokens)
   {
   //B2 S25 "6 [result]" T7
   std::cout << "Calling handleDefineLocal helper - ilmb.\n";

   TR::MethodBuilder *mb = static_cast<TR::MethodBuilder *>(ilmb);
   handleDefineLocal(mb, tokens);
   }

void
OMR::JitBuilderReplayTextFile::handleAllLocalsHaveBeenDefined(TR::MethodBuilder * mb, char * tokens)
   {
     std::cout << "Calling handleAllLocalsHaveBeenDefined helper.\n";
     mb->AllLocalsHaveBeenDefined();
   }

void
OMR::JitBuilderReplayTextFile::handleConstInt8(TR::IlBuilder * ilmb, char * tokens)
   {
   // Put into map: key ID, value IlValue*
   std::cout << "Calling handleConstInt8 helper.\n";

   uint32_t ID = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);
   uint32_t value = getNumberFromToken(tokens);

   // std::cout << "Found ID: " << ID << ", and value: " << value << ". Storing into map.\n";

   IlValue * val = ilmb->ConstInt8(value);
   StoreIDPointerPair(ID, val);
   }

void
OMR::JitBuilderReplayTextFile::handleConstInt32(TR::IlBuilder * ilmb, char * tokens)
   {
   // Put into map: key ID, value IlValue*
   std::cout << "Calling handleConstInt32 helper.\n";

   uint32_t ID = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);
   uint32_t value = getNumberFromToken(tokens);

   // std::cout << "Found ID: " << ID << ", and value: " << value << ". Storing into map.\n";

   IlValue * val = ilmb->ConstInt32(value);
   StoreIDPointerPair(ID, val);
   }

void
OMR::JitBuilderReplayTextFile::handleConstInt64(TR::IlBuilder * ilmb, char * tokens)
   {
   // Put into map: key ID, value IlValue*
   std::cout << "Calling handleConstInt64 helper.\n";

   uint32_t ID = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);

   int64_t value = atol(tokens);
   IlValue * val = ilmb->ConstInt64(value);
   StoreIDPointerPair(ID, val);
   }

void
OMR::JitBuilderReplayTextFile::handleConstDouble(TR::IlBuilder * ilmb, char * tokens)
   {
   // Put into map: key ID, value IlValue*
   std::cout << "Calling handleConstDouble helper.\n";

   uint32_t ID = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);

   int64_t value = atol(tokens);
   IlValue * val = ilmb->ConstDouble(value);
   StoreIDPointerPair(ID, val);
   }

void
OMR::JitBuilderReplayTextFile::handleConstAddress(TR::IlBuilder * ilmb, char * tokens)
   {
   // Put into map: key ID, value IlValue*
   std::cout << "Calling handleConstAddress helper.\n";

   uint32_t ID = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);

   void * value = (void *) atol(tokens);
   IlValue * val = ilmb->ConstAddress(value);
   StoreIDPointerPair(ID, val);
   }

void
OMR::JitBuilderReplayTextFile::handleLoad(TR::IlBuilder * ilmb, char * tokens)
   {
   // Def S12 "4 [Load]"
   // B2 S12 V11 "5 [value]"
   // Load("value")
   std::cout << "Calling handleLoad helper.\n";

   uint32_t ID = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);

   uint32_t strLen = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);
   const char * defineLoadParam = getServiceStringFromToken(strLen, tokens);

   IlValue * loadVal = ilmb->Load(defineLoadParam);
   StoreIDPointerPair(ID, loadVal);
   }

void
OMR::JitBuilderReplayTextFile::handleLoadAt(TR::IlBuilder * ilmb, char * tokens)
   {
   // Def S112 "6 [LoadAt]" 
   // B104 S112 V111 T14 V110 

   std::cout << "Calling handleLoadAt helper.\n";

   uint32_t storeID = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);

   uint32_t typeID = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);
   uint32_t valueID = getNumberFromToken(tokens);

   TR::IlType * param1 = static_cast<TR::IlType *>(lookupPointer(typeID));
   TR::IlValue * param2 = static_cast<TR::IlValue *>(lookupPointer(valueID));

   IlValue * loadVal = ilmb->LoadAt(param1, param2);
   StoreIDPointerPair(storeID, loadVal);
   }

   void
   OMR::JitBuilderReplayTextFile::handleStore(TR::IlBuilder * ilmb, char * tokens)
      {
      // Def S22 "5 [Store]"
      //B2 S22 "7 [result2]" V21
      //B2 S13 V23 "7 [result2]"
      std::cout << "Calling handleStore helper.\n";

      uint32_t strLen = getNumberFromToken(tokens);
      tokens = std::strtok(NULL, SPACE);
      const char * defineStoreParam = getServiceStringFromToken(strLen, tokens);
      tokens = std::strtok(NULL, SPACE);
      uint32_t valuetoStore = getNumberFromToken(tokens);

      TR::IlValue * paramIlValue = static_cast<TR::IlValue *>(lookupPointer(valuetoStore));

      ilmb->Store(defineStoreParam, paramIlValue);
      //StoreIDPointerPair(ID, loadVal);
      }

   void
   OMR::JitBuilderReplayTextFile::handleStoreAt(TR::IlBuilder * ilmb, char * tokens)
      {
    //   Def S73 "7 [StoreAt]" 
    //   B52 S73 V60 V71 

      std::cout << "Calling handleStoreAt helper.\n";

      uint32_t addressID = getNumberFromToken(tokens);
      tokens = std::strtok(NULL, SPACE);
      uint32_t valuetoStore = getNumberFromToken(tokens);

      TR::IlValue * param1Value = static_cast<TR::IlValue *>(lookupPointer(addressID));
      TR::IlValue * param2Value = static_cast<TR::IlValue *>(lookupPointer(valuetoStore));

      ilmb->StoreAt(param1Value, param2Value);
      //StoreIDPointerPair(ID, loadVal);
      }

void
OMR::JitBuilderReplayTextFile::handleAdd(TR::IlBuilder * ilmb, char * tokens)
   {
   // Def S16 "3 [Add]"
   // B2 S16 V15 V11 V13
   std::cout << "Calling handleAdd helper.\n";

   uint32_t IDtoStore = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);

   uint32_t param1ID = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);
   uint32_t param2ID = getNumberFromToken(tokens);

   TR::IlValue * param1IlValue = static_cast<TR::IlValue *>(lookupPointer(param1ID));
   TR::IlValue * param2IlValue = static_cast<TR::IlValue *>(lookupPointer(param2ID));

   TR::IlValue * addResult = ilmb->Add(param1IlValue, param2IlValue);

   StoreIDPointerPair(IDtoStore, addResult);
   }

//Add Sub June.27
   void
   OMR::JitBuilderReplayTextFile::handleSub(TR::IlBuilder * ilmb, char * tokens)
      {
      // Def S16 "3 [Sub]"
      // B2 S16 V15 V11 V13
      std::cout << "Calling handleSub helper.\n";

      uint32_t IDtoStore = getNumberFromToken(tokens);
      tokens = std::strtok(NULL, SPACE);

      uint32_t param1ID = getNumberFromToken(tokens);
      tokens = std::strtok(NULL, SPACE);
      uint32_t param2ID = getNumberFromToken(tokens);

      TR::IlValue * param1IlValue = static_cast<TR::IlValue *>(lookupPointer(param1ID));
      TR::IlValue * param2IlValue = static_cast<TR::IlValue *>(lookupPointer(param2ID));

      TR::IlValue * subResult = ilmb->Sub(param1IlValue, param2IlValue);

      StoreIDPointerPair(IDtoStore, subResult);
      }

  void
  OMR::JitBuilderReplayTextFile::handleMul(TR::IlBuilder * ilmb, char * tokens)
    {
    // Def S16 "3 [Mul]"
    // B2 S16 V15 V11 V13
    std::cout << "Calling handleMul helper.\n";

    uint32_t IDtoStore = getNumberFromToken(tokens);
    tokens = std::strtok(NULL, SPACE);

    uint32_t param1ID = getNumberFromToken(tokens);
    tokens = std::strtok(NULL, SPACE);
    uint32_t param2ID = getNumberFromToken(tokens);

    TR::IlValue * param1IlValue = static_cast<TR::IlValue *>(lookupPointer(param1ID));
    TR::IlValue * param2IlValue = static_cast<TR::IlValue *>(lookupPointer(param2ID));
    TR::IlValue * addResult = ilmb->Mul(param1IlValue, param2IlValue);

    StoreIDPointerPair(IDtoStore, addResult);
    }

  void
  OMR::JitBuilderReplayTextFile::handleDiv(TR::IlBuilder * ilmb, char * tokens)
    {
    std::cout << "Calling handleDiv helper.\n";
    
    uint32_t IDtoStore = getNumberFromToken(tokens);
    tokens = std::strtok(NULL, SPACE);

    uint32_t param1ID = getNumberFromToken(tokens);
    tokens = std::strtok(NULL, SPACE);
    uint32_t param2ID = getNumberFromToken(tokens);

    TR::IlValue * param1IlValue = static_cast<TR::IlValue *>(lookupPointer(param1ID));
    TR::IlValue * param2IlValue = static_cast<TR::IlValue *>(lookupPointer(param2ID));

    TR::IlValue * divResult = ilmb->Div(param1IlValue, param2IlValue);

    StoreIDPointerPair(IDtoStore, divResult);
    }

void
OMR::JitBuilderReplayTextFile::handleAnd(TR::IlBuilder * ilmb, char * tokens)
   {
   std::cout << "Calling handleAnd helper.\n";

   uint32_t IDtoStore = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);

   uint32_t param1ID = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);
   uint32_t param2ID = getNumberFromToken(tokens);

   TR::IlValue * param1IlValue = static_cast<TR::IlValue *>(lookupPointer(param1ID));
   TR::IlValue * param2IlValue = static_cast<TR::IlValue *>(lookupPointer(param2ID));

   TR::IlValue * andResult = ilmb->And(param1IlValue, param2IlValue);

   StoreIDPointerPair(IDtoStore, andResult);
   }

void
OMR::JitBuilderReplayTextFile::handleOr(TR::IlBuilder * ilmb, char * tokens)
   {
   std::cout << "Calling handleOr helper.\n";

   uint32_t IDtoStore = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);

   uint32_t param1ID = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);
   uint32_t param2ID = getNumberFromToken(tokens);

   TR::IlValue * param1IlValue = static_cast<TR::IlValue *>(lookupPointer(param1ID));
   TR::IlValue * param2IlValue = static_cast<TR::IlValue *>(lookupPointer(param2ID));

   TR::IlValue * orResult = ilmb->Or(param1IlValue, param2IlValue);

   StoreIDPointerPair(IDtoStore, orResult);
   }

void
OMR::JitBuilderReplayTextFile::handleXor(TR::IlBuilder * ilmb, char * tokens)
   {
   std::cout << "Calling handleXor helper.\n";

   uint32_t IDtoStore = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);

   uint32_t param1ID = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);
   uint32_t param2ID = getNumberFromToken(tokens);

   TR::IlValue * param1IlValue = static_cast<TR::IlValue *>(lookupPointer(param1ID));
   TR::IlValue * param2IlValue = static_cast<TR::IlValue *>(lookupPointer(param2ID));

   TR::IlValue * xorResult = ilmb->Xor(param1IlValue, param2IlValue);

   StoreIDPointerPair(IDtoStore, xorResult);
   }

// Add New Builder June.29
  void
  OMR::JitBuilderReplayTextFile::handleNewIlBuilder(TR::IlBuilder * ilmb, char * tokens)
   {
   // Def S20 "12 [NewIlBuilder]"
   // B2 S20 B19
   std::cout << "Calling handleNewBuilder helper.\n";

   uint32_t IDtoStore = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);


   TR::IlBuilder * newBuilder = ilmb->OrphanBuilder();//???error: cannot
      //initialize a variable of type 'TR::IlValue *' with an rvalue of type
      //'void'

   StoreIDPointerPair(IDtoStore, newBuilder);
   }

// Add LessThan June.29
  void
  OMR::JitBuilderReplayTextFile::handleLessThan(TR::IlBuilder * ilmb, char * tokens){
    // Def S17 "8 [LessThan]"
    // B2 S17 V16 V12 V14
    std::cout << "Calling handleLessThan helper.\n";

    uint32_t IDtoStore = getNumberFromToken(tokens);
    tokens = std::strtok(NULL, SPACE);

    uint32_t param1ID = getNumberFromToken(tokens);
    tokens = std::strtok(NULL, SPACE);
    uint32_t param2ID = getNumberFromToken(tokens);

    TR::IlValue * leftValue  = static_cast<TR::IlValue *>(lookupPointer(param1ID));
    TR::IlValue * rightValue = static_cast<TR::IlValue *>(lookupPointer(param2ID));

    TR::IlValue * lessThanResult = ilmb->LessThan(leftValue, rightValue);

    StoreIDPointerPair(IDtoStore, lessThanResult);

  }

  void 
  OMR::JitBuilderReplayTextFile::handleCreateLocalArray(TR::IlBuilder * ilmb, char * tokens)  
     {
    //  Def S22 "16 [CreateLocalArray]" 
    //  B2 S22 V21 8 T15 
     std::cout << "Calling handleCreateLocalArray helper.\n";

     uint32_t IDtoStore = getNumberFromToken(tokens);
     tokens = std::strtok(NULL, SPACE);
 
     uint32_t param1 = getNumberFromToken(tokens);
     tokens = std::strtok(NULL, SPACE);
     uint32_t param2ID = getNumberFromToken(tokens);

     TR::IlType * param2 = static_cast<TR::IlType *>(lookupPointer(param2ID));
 
     TR::IlValue * lArray = ilmb->CreateLocalArray((int32_t)param1, param2);

     StoreIDPointerPair(IDtoStore, lArray);
     }

  void
  OMR::JitBuilderReplayTextFile::handleGreaterThan(TR::IlBuilder * ilmb, char * tokens){
    std::cout << "Calling handleGreaterThan helper.\n";

    uint32_t IDtoStore = getNumberFromToken(tokens);
    tokens = std::strtok(NULL, SPACE);

    uint32_t param1ID = getNumberFromToken(tokens);
    tokens = std::strtok(NULL, SPACE);
    uint32_t param2ID = getNumberFromToken(tokens);

    TR::IlValue * leftValue  = static_cast<TR::IlValue *>(lookupPointer(param1ID));
    TR::IlValue * rightValue = static_cast<TR::IlValue *>(lookupPointer(param2ID));

    TR::IlValue * result = ilmb->GreaterThan(leftValue, rightValue);

    StoreIDPointerPair(IDtoStore, result);
  }

  void
  OMR::JitBuilderReplayTextFile::handleNotEqualTo(TR::IlBuilder * ilmb, char * tokens){
    std::cout << "Calling handleNotEqualTo helper.\n";

    uint32_t IDtoStore = getNumberFromToken(tokens);
    tokens = std::strtok(NULL, SPACE);

    uint32_t param1ID = getNumberFromToken(tokens);
    tokens = std::strtok(NULL, SPACE);
    uint32_t param2ID = getNumberFromToken(tokens);

    TR::IlValue * leftValue  = static_cast<TR::IlValue *>(lookupPointer(param1ID));
    TR::IlValue * rightValue = static_cast<TR::IlValue *>(lookupPointer(param2ID));

    TR::IlValue * result = ilmb->NotEqualTo(leftValue, rightValue);

    StoreIDPointerPair(IDtoStore, result);
  }


   // Add IfThenElse June.29.2018
   void
   OMR::JitBuilderReplayTextFile::handleIfThenElse(TR::IlBuilder * ilmb, char * tokens)
      {
      // B2 S13 V22 "14 [lessThanResult]
      // Def S23 "10 [IfThenElse]"
      // B2 S23 B19 B21 V22
      // B19 S15 V24 1
      // B19 S18 "6 [result]" V24
      // B21 S15 V25 0
      // B21 S18 "6 [result]" V25
      std::cout << "Calling handleIfThenElse helper.\n";

      uint32_t IfID = getNumberFromToken(tokens);
      tokens = std::strtok(NULL, SPACE);

      uint32_t ElseID = getNumberFromToken(tokens);
      tokens = std::strtok(NULL, SPACE);
      uint32_t conditionID = getNumberFromToken(tokens);

      TR::IlBuilder * IfBlock   = static_cast<TR::IlBuilder *>(lookupPointer(IfID));
      
      TR::IlValue * conditionValue = static_cast<TR::IlValue *>(lookupPointer(conditionID));

      if(ElseID == 0)
         ilmb->IfThen(&IfBlock, conditionValue);
      else {
        TR::IlBuilder * ElseBlock = static_cast<TR::IlBuilder *>(lookupPointer(ElseID));
        ilmb->IfThenElse(&IfBlock, &ElseBlock, conditionValue);
      }
         
      }

void
OMR::JitBuilderReplayTextFile::handleForLoop(TR::IlBuilder * ilmb, char * tokens)
   {
   // Def S30 "7 [ForLoop]"
   // B2 S30  1 "1 [i]" B29 Def Def V26 V27 V28
   std::cout << "Calling handleForLoop helper.\n";

   bool boolParam;
   uint32_t boolean = getNumberFromToken(tokens);

   if(boolean == 1)
       boolParam = true;
   else
       boolParam = false;

   tokens = std::strtok(NULL, SPACE);
   uint32_t strLen = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);

   char * indVar = getServiceStringFromToken(strLen, tokens);
   tokens = std::strtok(NULL, SPACE);

   uint32_t builderID = getNumberFromToken(tokens);
   TR::IlBuilder * body = static_cast<TR::IlBuilder *>(lookupPointer(builderID));
   tokens = std::strtok(NULL, SPACE);

   TR::IlBuilder * breakKey;
   TR::IlBuilder * continueKey;

   uint32_t breakID = getNumberFromToken(tokens);
   breakKey = static_cast<TR::IlBuilder *>(lookupPointer(breakID));

   tokens = std::strtok(NULL, SPACE);

   uint32_t continueID = getNumberFromToken(tokens);
   continueKey = static_cast<TR::IlBuilder *>(lookupPointer(continueID));

   tokens = std::strtok(NULL, SPACE);

   uint32_t initID = getNumberFromToken(tokens);
   TR::IlValue * initValue = static_cast<TR::IlValue *>(lookupPointer(initID));
   tokens = std::strtok(NULL, SPACE);

   uint32_t iterateID = getNumberFromToken(tokens);
   TR::IlValue * iterateValue = static_cast<TR::IlValue *>(lookupPointer(iterateID));
   tokens = std::strtok(NULL, SPACE);

   uint32_t incrementID = getNumberFromToken(tokens);
   TR::IlValue * incrementValue = static_cast<TR::IlValue *>(lookupPointer(incrementID));

   if(breakID == 0 && continueID == 0)
      ilmb->ForLoop(boolParam, indVar, &body, NULL, NULL, initValue, iterateValue, incrementValue);
   else if(breakID == 0)
      ilmb->ForLoop(boolParam, indVar, &body, NULL, &continueKey, initValue, iterateValue, incrementValue);
   else if(continueID == 0)
      ilmb->ForLoop(boolParam, indVar, &body, &breakKey, NULL, initValue, iterateValue, incrementValue);
   else
      ilmb->ForLoop(boolParam, indVar, &body, &breakKey, &continueKey, initValue, iterateValue, incrementValue);
  }

void
OMR::JitBuilderReplayTextFile::handleCall(TR::IlBuilder * ilmb, char * tokens)
   {
   //Def S32 "4 [Call]"
   //B23 S32 "3 [fib]" 1 V30 V33
   std::cout << "Calling handleCall helper.\n";

   uint32_t strLen = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);
   const char * functionName = getServiceStringFromToken(strLen, tokens);
   tokens = std::strtok(NULL, SPACE);

   uint32_t numberOfParams = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);

   TR::IlValue **values = (TR::IlValue **)malloc(numberOfParams * sizeof(TR::IlValue *));
   TR_ASSERT_FATAL(NULL != values, "Unable to allocate values array for handleCall");

   for (int i = 0; i < numberOfParams; i++)
      {
      uint32_t valueID = getNumberFromToken(tokens);
      TR::IlValue * value = static_cast<TR::IlValue *>(lookupPointer(valueID));
      tokens = std::strtok(NULL, SPACE);

      values[i] = value;
      }

   bool hasReturn = (tokens != NULL);

   TR::IlValue *returnValue = ilmb->Call(functionName, (int32_t)numberOfParams, values);

   if (hasReturn)
      {
      TR_ASSERT_FATAL(NULL != returnValue, "Call did not return a result as expected");
      uint32_t IDtoStore = getNumberFromToken(tokens);
      StoreIDPointerPair(IDtoStore, returnValue);
      }

   //TODO free values.....
   }

void
OMR::JitBuilderReplayTextFile::handleConvertTo(TR::IlBuilder * ilmb, char * tokens)
   {
   //Def S35 "9 [ConvertTo]"
   //B23 S35 V34 T12 V30
   std::cout << "Calling handleConvertTo helper.\n";

   uint32_t IDtoStore = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);

   uint32_t param1ID = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);
   uint32_t param2ID = getNumberFromToken(tokens);

   TR::IlType * param1IlValue = static_cast<TR::IlType *>(lookupPointer(param1ID));
   TR::IlValue * param2IlValue = static_cast<TR::IlValue *>(lookupPointer(param2ID));

   TR::IlValue * addResult = ilmb->ConvertTo(param1IlValue, param2IlValue);

   StoreIDPointerPair(IDtoStore, addResult);
   }

void
OMR::JitBuilderReplayTextFile::handlePointerType(TR::IlBuilder * ilmb, char * tokens)
   {
   // Def S12 "11 [PointerType]" 
   // B2 S12 T10 T11 
   std::cout << "Calling handlePointerType helper.\n";

   uint32_t IDtoStore = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);

   uint32_t param1ID = getNumberFromToken(tokens);

   TR::IlType * param1IlValue = static_cast<TR::IlType *>(lookupPointer(param1ID));

   TR::IlType * addResult = ilmb->typeDictionary()->PointerTo(param1IlValue);

   StoreIDPointerPair(IDtoStore, addResult);
   }

void
OMR::JitBuilderReplayTextFile::handlePointerType(TR::IlBuilder * ilmb, char * tokens)
{
  // Def S14 "11 [PointerType]"
  // B2 S14 T13 T7

  std::cout << "Calling handlePointerType helper.\n";

  uint32_t PointerTypeID = getNumberFromToken(tokens);
  tokens = std::strtok(NULL, SPACE);

  uint32_t TypeID = getNumberFromToken(tokens);
  //Find the corresponding type through mapping:
  TR::IlType * TypeValue = static_cast<TR::IlType *>(lookupPointer(TypeID));

  //Define a new dt and create a pointer to it through TypeDictionary
  TR::IlType *pointer = ilmb->typeDictionary()->PointerTo(TypeValue);

  StoreIDPointerPair(PointerTypeID, pointer);

}


void
OMR::JitBuilderReplayTextFile::handleCreateLocalArray(TR::IlBuilder * ilmb, char * tokens) {
  // Def S13 "16 [CreateLocalArray]"
  // B2 S13 V12 2 T7

  std::cout << "Calling handleCreateLocalArray helper.\n";

  uint32_t IDtoStore = getNumberFromToken(tokens);
  tokens = std::strtok(NULL, SPACE);

  uint32_t arrayLength = getNumberFromToken(tokens);
  tokens = std::strtok(NULL, SPACE);
  uint32_t typeID = getNumberFromToken(tokens);

  TR::IlType * ilType = static_cast<TR::IlType *>(lookupPointer(typeID));

  TR::IlValue * result = ilmb->CreateLocalArray(arrayLength, ilType);

  StoreIDPointerPair(IDtoStore, result);

}


void
OMR::JitBuilderReplayTextFile::handleIndexAt(TR::IlBuilder * ilmb, char * tokens) {

  //Def S30 "7 [IndexAt]"
  //B21 S30 V27(store the value at) T28(type: pInt32) V12(localArray) V26(0)

  std::cout << "Calling handleIndexAt helper.\n";

  uint32_t IDtoStore = getNumberFromToken(tokens);
  tokens = std::strtok(NULL, SPACE);

  uint32_t dataTypeID = getNumberFromToken(tokens);
  tokens = std::strtok(NULL, SPACE);

  uint32_t arrayID = getNumberFromToken(tokens);
  tokens = std::strtok(NULL, SPACE);

  uint32_t position = getNumberFromToken(tokens);
  tokens = std::strtok(NULL, SPACE);

  TR::IlType * dataType = static_cast<TR::IlType *>(lookupPointer(dataTypeID));
  TR::IlValue * base = static_cast<TR::IlValue *>(lookupPointer(arrayID));
  TR::IlValue * index = static_cast<TR::IlValue *>(lookupPointer(position));

  TR::IlValue * result = ilmb->IndexAt(dataType, base, index);

  StoreIDPointerPair(IDtoStore, result);
}


void
OMR::JitBuilderReplayTextFile::handleStoreAt(TR::IlBuilder * ilmb, char * tokens) {
  //Def S32 "7 [StoreAt]"
  //B21 S32 V27 V31

  std::cout << "Calling handleStoreAt helper.\n";

  uint32_t addressID = getNumberFromToken(tokens);
  tokens = std::strtok(NULL, SPACE);
  uint32_t ValueID = getNumberFromToken(tokens);


  TR::IlValue * address = static_cast<TR::IlValue *>(lookupPointer(addressID));
  TR::IlValue * value = static_cast<TR::IlValue *>(lookupPointer(ValueID));

  ilmb->StoreAt(address, value);
}


void
OMR::JitBuilderReplayTextFile::handleLoadAt(TR::IlBuilder * ilmb, char * tokens) {
  //Def S45 "6 [LoadAt]"
  //B2 S45 V44 T28 V43

  std::cout << "Calling handleLoadAt helper.\n";

  uint32_t IDtoStore = getNumberFromToken(tokens);
  tokens = std::strtok(NULL, SPACE);

  uint32_t dataTypeID = getNumberFromToken(tokens);
  tokens = std::strtok(NULL, SPACE);

  uint32_t addressID = getNumberFromToken(tokens);
  tokens = std::strtok(NULL, SPACE);

  TR::IlType * dataType = static_cast<TR::IlType *>(lookupPointer(dataTypeID));
  TR::IlValue * address = static_cast<TR::IlValue *>(lookupPointer(addressID));

  TR::IlValue * result = ilmb->LoadAt(dataType, address);

  StoreIDPointerPair(IDtoStore, result);

}



void
OMR::JitBuilderReplayTextFile::handleReturnValue(TR::IlBuilder * ilmb, char * tokens)
   {
   std::cout << "Calling handleReturnValue helper.\n";

   uint32_t ID = getNumberFromToken(tokens);
   TR::IlValue * value = static_cast<TR::IlValue *>(lookupPointer(ID));

   ilmb->Return(value);
   }

void
OMR::JitBuilderReplayTextFile::handleUnsignedShiftR(TR::IlBuilder * ilmb, char * tokens)
   {
   std::cout << "Calling handleUnsignedShiftR helper.\n";

   uint32_t IDtoStore = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);

   uint32_t param1ID = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);
   uint32_t param2ID = getNumberFromToken(tokens);

   TR::IlValue * v  = static_cast<TR::IlValue *>(lookupPointer(param1ID));
   TR::IlValue * amount = static_cast<TR::IlValue *>(lookupPointer(param2ID));

   TR::IlValue * result = ilmb->UnsignedShiftR(v, amount);

   StoreIDPointerPair(IDtoStore, result);
   }

void
OMR::JitBuilderReplayTextFile::handleIfCmpEqualZero(TR::IlBuilder * ilmb, char * tokens)
   {
   std::cout << "Calling handleIfCmpEqualZero helper.\n";

   uint32_t IDtoStore = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);

   uint32_t param1ID = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);
   uint32_t param2ID = getNumberFromToken(tokens);

   TR::IlBuilder * target  = static_cast<TR::IlBuilder *>(lookupPointer(param1ID));
   TR::IlValue * condition = static_cast<TR::IlValue *>(lookupPointer(param2ID));

   ilmb->IfCmpEqualZero(target, condition);
   }

void
OMR::JitBuilderReplayTextFile::handleIndexAt(TR::IlBuilder * ilmb, char * tokens)
   {
   std::cout << "Calling handleIndexAt helper.\n";

   uint32_t IDtoStore = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);

   uint32_t param1ID = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);
   uint32_t param2ID = getNumberFromToken(tokens);
   tokens = std::strtok(NULL, SPACE);
   uint32_t param3ID = getNumberFromToken(tokens);

   TR::IlType * dt = static_cast<TR::IlType *>(lookupPointer(param1ID));
   TR::IlValue * base  = static_cast<TR::IlValue *>(lookupPointer(param2ID));
   TR::IlValue * index = static_cast<TR::IlValue *>(lookupPointer(param3ID));

   TR::IlValue * result = ilmb->IndexAt(dt, base, index);

   StoreIDPointerPair(IDtoStore, result);
   }

bool
OMR::JitBuilderReplayTextFile::parseConstructor()
   {
      std::cout << "Parsing Constructor...\n";
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
         char * tokens;
         bool constructorFlag = true;

         std::string textLine;

         while(constructorFlag)
         {
            tokens = getLineAsChar();

            uint32_t id = getNumberFromToken(tokens);

            if(id == 0) // It's DEF
               {
                   // std::cout << "I am DEF: " << tokens << '\n';
                   // Store something in the table
                   addIDPointerPairToMap(tokens);
               }
            else if (tokens != NULL)  // Its probably a call to MethodBuilder
               {
                   // std::cout << "I am a builder call: " << tokens << '\n';
                   constructorFlag = handleService(METHOD_BUILDER, tokens);
               }
         }
         return true;
   }

bool
OMR::JitBuilderReplayTextFile::parseBuildIL()
   {
   std::cout << "Parsing BuildIl...\n";

   char * tokens;
   bool buildIlFlag = true;

   std::string textLine;

   while(buildIlFlag)
      {
      tokens = getLineAsChar();

      uint32_t id = getNumberFromToken(tokens);

      if(id == 0) // It's DEF
         {
            if (tokens[0] == 'I')
               {
                  // std::cout << "I am ID1: " << tokens << '\n';
                  buildIlFlag = false;
               }
            else
               {
                  // std::cout << "I am DEF: " << tokens << '\n';
                  // Store something in the table
                  addIDPointerPairToMap(tokens);
               }
         }
      else if (tokens != NULL)  // Its probably a call to MethodBuilder
         {
             // std::cout << "I am a builder call: " << tokens << '\n';
             buildIlFlag = handleService(IL_BUILDER, tokens);
         }
      }
      return buildIlFlag;
   }
    // Do the processing here?
    // Useful http://www.cplusplus.com/reference/string/string/
