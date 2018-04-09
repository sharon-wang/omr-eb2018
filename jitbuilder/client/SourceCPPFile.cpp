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


#include "SourceCPPFile.hpp"

using namespace std;


void
SourceCPPFile::prolog()
   {
   CPPFile::prolog();

   // don't bother figuring out which ones are needed, just include the entire JitBuilder implementation API
   includeFile("ilgen/BytecodeBuilder.hpp");
   includeFile("ilgen/IlBuilder.hpp");
   includeFile("ilgen/IlType.hpp");
   includeFile("ilgen/IlValue.hpp");
   includeFile("ilgen/MethodBuilder.hpp");
   includeFile("ilgen/ThunkBuilder.hpp");
   includeFile("ilgen/TypeDictionary.hpp");
   includeFile("ilgen/VirtualMachineOperandArray.hpp");
   includeFile("ilgen/VirtualMachineOperandStack.hpp");
   includeFile("ilgen/VirtualMachineRegister.hpp");
   includeFile("ilgen/VirtualMachineRegisterInStruct.hpp");
   includeFile("ilgen/VirtualMachineState.hpp");
   }

bool
SourceCPPFile::needsPreparation(JBParameter & parm)
   {
   switch (parm.type)
      {
      case T_BytecodeBuilderArray :
      case T_BytecodeBuilderByRef :
      case T_IlBuilderArray :
      case T_IlBuilderByRef :
      case T_IlTypeArray :
      case T_IlValueArray :
         return true;

      default:
         return false;
      }
   return false;
   }

JBType
SourceCPPFile::rootType(JBType type)
   {
   switch (type)
      {
      case T_BytecodeBuilderArray :
      case T_BytecodeBuilderByRef :
         return T_BytecodeBuilder;

      case T_IlBuilderArray :
      case T_IlBuilderByRef :
         return T_IlBuilder;

      case T_IlTypeArray :
         return T_IlType;

      case T_IlValueArray :
         return T_IlValue;

      default :
         assert(0);
      }

   return T_none;
   }

bool
SourceCPPFile::isArray(JBParameter &parm)
   {
   switch (parm.type)
      {
      case T_BytecodeBuilderArray:
      case T_IlBuilderArray:
      case T_IlTypeArray:
      case T_IlValueArray:
      case T_int32Array:
         return true;

      default:
         return false;
      }

   return false;
   }

string
SourceCPPFile::classSpecifier(JBClass *clazz)
   {
   return clazz->name + "::";
   }

void
SourceCPPFile::prepareArgument(JBParameter & parm)
   {
   indent() << "ARG_SETUP(" + bareTypeName(rootType(parm.type)) << ", " << parm.name + "Impl, " << parm.name + "Arg, " << parm.name << ");\n";
   }

void
SourceCPPFile::prepareArrayArgument(JBParameter & parm, JBParameter & sizeParm)
   {
   indent() << "ARRAY_ARG_SETUP(" << bareTypeName(rootType(parm.type)) << ", " << sizeParm.name + ", " + parm.name + "Impl, " + parm.name + ");\n";
   }

bool
SourceCPPFile::needsRecovery(JBParameter & parm)
   {
   return needsPreparation(parm);
   }

void
SourceCPPFile::recoverArgument(JBParameter & parm)
   {
   indent() << "ARG_RETURN(" << bareTypeName(rootType(parm.type)) << ", " << parm.name + "Impl, " << parm.name << ");\n";
   }

void
SourceCPPFile::recoverArrayArgument(JBParameter & parm, JBParameter & sizeParm)
   {
   indent() << "ARRAY_ARG_RETURN(" << bareTypeName(rootType(parm.type)) << ", " << sizeParm.name << ", " << parm.name + "Impl, " << parm.name << ");\n";
   }

string
SourceCPPFile::declarationQualifiers(JBFunction *func)
   {
   return "";
   }

string
SourceCPPFile::constructorCall(JBFunction *func, JBClass *clazz)
   {
   if (!clazz->hasSuper())
      return string("");

   if (func->isCreator())
      return string(" : ") + clazz->super->name + string("(impl)");
   if (func->isConstructor())
      return string("\n  : ") + clazz->super->name + string("((void *)(new ") + callFunctionString(func, clazz, false) + string("))");

   return string("");
   }

void
SourceCPPFile::startFunctionDeclaration(JBFunction *func, JBClass *clazz)
   {
   startScope(declarationQualifiers(func) + functionDeclarationText(func, clazz) + constructorCall(func, clazz));
   }

void
SourceCPPFile::endFunctionDeclaration()
   {
   endScope();
   }

string
SourceCPPFile::argumentNameForCall(JBParameter & parm)
   {
   if (needsPreparation(parm))
      {
      if (isArray(parm))
         return parm.name + "Impl";
      if (parm.passedByRef())
         return parm.name + "Arg";
      }
   else if (hasImpl(parm.type))
      return "(" + parm.name + " != NULL) ? (" + parm.name + "->_impl) : NULL";

   return parm.name;
   }

string
SourceCPPFile::argumentText(JBParameter & parm)
   {
   string cast = implTypeCast(parm.type);
   if (parm.passedByRef() || cast.length() == 0)
      return argumentNameForCall(parm);

   return implTypeCast(parm.type) + "(" + argumentNameForCall(parm) + ")";
   }

string
SourceCPPFile::argument0ForCall(JBParameter &parm)
   {
   return argumentText(parm);
   }

string
SourceCPPFile::argumentNForCall(JBParameter & parm)
   {
   return string(", ") + argumentText(parm);
   }

void
SourceCPPFile::returnValue(JBType type, string implObject)
   {
   string func;
   switch (type)
      {
      case T_BytecodeBuilder:
      case T_IlBuilder:
      case T_IlReference:
      case T_IlValue:
      case T_IlType:
      case T_TypeDictionary:
      case T_VirtualMachineState:
         indent() << "GET_CLIENT_OBJECT(clientObj, " << bareTypeName(type) + ", " + implObject + ");\n";
         indent() << "return clientObj;\n";
         break;

      default:
         indent() << "return " << implObject << ";\n";
         break;
      }

   }

void
SourceCPPFile::prepareVarArgs(uint32_t numParameters, JBParameter *parms, uint32_t varArg)
   {
   string & numVarArgGroups = parms[varArg-1].name;
   uint32_t groupSize = numParameters - varArg - 1;

   indent() << "va_list args;\n";
   indent() << "va_start(args, " << numVarArgGroups << ");\n";
   for (int32_t p=varArg+1;p < numParameters;p++)
      {
      JBParameter & parm = parms[p];
      if (parm.passedByRef())
         indent() << typeName(parm.type) << "* " << parm.name << "Ptrs = new " << typeName(parm.type) << "[" << numVarArgGroups << "];\n";
      indent() << implName(parm.type) << "* " << parm.name << " = new " << implName(parm.type) << "[" << numVarArgGroups << "];\n";
      }
   startScope("for (int32_t a=0;a < " + numVarArgGroups + ";a++)");

   for (int32_t p=varArg+1;p < numParameters;p++)
      {
      JBParameter & parm = parms[p];
      if (parm.passedByRef())
         {
         indent() << parm.name << "Ptrs[a] = va_arg(args," << typeName(parm.type) << ");";
         startScope("if (*(" + parm.name + "Ptrs[a]) != NULL)");
            indent() << parm.name << "[a] = " << implTypeCast(parm.type) << "(*(" << parm.name << "Ptrs[a]))->_impl;\n";
         endScope();
         startScope("else");
            indent() << parm.name << "[a] = NULL;\n";
         endScope();
         }
      else
         {
         indent() << parm.name << "[a] = " << implTypeCast(parm.type) << "(va_arg(args," << typeName(parm.type) << "))";
         if (hasImpl(parm.type))
            _s << "->_impl";
         _s << ";\n";
         }
      }

   endScope();
   }

string
SourceCPPFile::varArgsForCall(uint32_t numParameters, JBParameter *parms, uint32_t varArg)
   {
   string s("");
   for (int32_t p=varArg+1;p < numParameters;p++)
      {
      JBParameter & parm = parms[p];
      s += string(", ") + parm.name;
      }

   return s;
   }

void
SourceCPPFile::recoverVarArgs(uint32_t numParameters, JBParameter *parms, uint32_t varArg)
   {
   bool needByRefProcessing = false;
   for (int32_t p=varArg+1;p < numParameters;p++)
      {
      JBParameter & parm = parms[p];
      if (parm.passedByRef())
         {
         needByRefProcessing = true;
         break;
         }
      }

   if (needByRefProcessing)
      {
      string & numVarArgGroups = parms[varArg-1].name;
      startScope("for (int32_t a=0;a < " + numVarArgGroups + ";a++)", true);
      
      for (int32_t p=varArg+1;p < numParameters;p++)
         {
         JBParameter & parm = parms[p];
         if (parm.passedByRef())
            {
            indent() << "GET_CLIENT_OBJECT(clientObj_" << parm.name << "_a, " << bareTypeName(parm.type) << ", " << parm.name << "[a]);\n";
            indent() << "*(" << parm.name << "Ptrs[a]) = clientObj_" << parm.name << "_a;\n";
            }
         }

      endScope();
      }
   
   for (int32_t p=varArg+1;p < numParameters;p++)
      {
      JBParameter & parm = parms[p];
      indent() << "delete[] " << parm.name << ";\n";
      }
   }

JBParameter &
SourceCPPFile::findSizeParameter(JBParameter *parms, int32_t p)
   {
   int32_t sizeParmIndex = p-1; // assumes array args immediately preceded by size arg
   JBParameter *sizeParm=parms+sizeParmIndex;
   while (sizeParmIndex >= 0 && isArray(*sizeParm))
      {
      sizeParmIndex--;
      sizeParm = parms+sizeParmIndex;
      }
   assert(sizeParmIndex >= 0);
   return *sizeParm;
   }

void
SourceCPPFile::callFunction(JBFunction *func, JBClass *clazz)
   {
   _s << callFunctionString(func, clazz);
   }

string
SourceCPPFile::callFunctionString(JBFunction *func, JBClass *clazz, bool isStatement)
   {
   string s("");

   if (func->isConstructor())
      s += string("TR::") + bareTypeName(clazz->type) + "(";
   else
      s += string("(") + implTypeCast(clazz->type) + "_impl)->" + implFunctionName(func) + "(";

   if (func->numParameters > 0)
      {
      s += argument0ForCall(func->parms[0]);
      for (int p=1;p < func->numParameters;p++)
         {
         JBParameter &parm = func->parms[p];
         if (parm.type == T_vararg)
            {
            s += varArgsForCall(func->numParameters, func->parms, p);

            // recoverVarArgs consumes everything after T_vararg as well
            break;
            }

         s += argumentNForCall(parm);
         }
      }

   // close off the arguments
   s += ")";

   // finally end function call
   if (isStatement)
      s += ";\n";

   return s;
   }

void
SourceCPPFile::handleCallbacks(JBClass *clazz)
   {
   for (auto it = clazz->functionsBegin(); it != clazz->functionsEnd(); ++it)
      {
      JBFunction *func = *it;
      if (func->isCallback())
         indent() << "(" << implTypeCast(clazz->type) << "_impl)->setClientCallback_" << func->name << "((void *)&" << clazz->name << "Callback_" << func->name << ");\n";
      }
   }

void
SourceCPPFile::initializeFields(JBClass *implClazz, JBClass *clazz)
   {
   for (int f=0;f < clazz->numFields;f++)
      {
      JBField *field = clazz->fields+f;;
      if ((field->flags & ASSIGN_AT_CREATE) != 0)
         {
         indent() << "GET_CLIENT_OBJECT(clientObj_" << field->name << ", " << bareTypeName(field->type) << ", (" << implTypeCast(implClazz->type) << "impl)->" << field->name << ");\n";
         indent() << field->name << " = clientObj_" << field->name << ";\n";
         }
      }
   }

void
SourceCPPFile::initializeFieldsRecursive(JBClass *implClazz, JBClass *clazz)
   {
   // initialize super class fields first
   if (clazz->hasSuper())
      initializeFieldsRecursive(implClazz, clazz->super);

   initializeFields(implClazz, clazz);
   }

void
SourceCPPFile::functionBody(JBFunction *func, JBClass *clazz)
   {
   startFunctionDeclaration(func, clazz);

   bool hasByRefVarArgs=false;

   // deal with any "special" parameters
   for (int p=0;p < func->numParameters;p++)
      {
      JBParameter &parm=func->parms[p];
      if (parm.type == T_vararg)
         {
         hasByRefVarArgs = true;
         prepareVarArgs(func->numParameters, func->parms, p);

         // prepareVarArgs consumes everything after T_vararg as well
         break;
         }

      if (needsPreparation(parm))
         {
         if (isArray(parm))
            {
            JBParameter &sizeParm = findSizeParameter(func->parms, p);
            prepareArrayArgument(parm, sizeParm);
            }
         else
            prepareArgument(parm);
         }
      }

   // save return value if there is one
   if (!func->isConstructor() || clazz->super == NULL)
      {
      if (func->returnType != T_none)
         indent() << implName(func->returnType) << " implReturnValue = " << implTypeCast(func->returnType) << " ";
      else if (func->isConstructor())
         indent() << "_impl = new ";
      else
         indent();

      callFunction(func, clazz);
      }

   // recover any "special" parameters
   for (int p=0;p < func->numParameters; p++)
      {
      JBParameter &parm=func->parms[p];
      if (parm.type == T_vararg)
         {
         recoverVarArgs(func->numParameters, func->parms, p);

         // recoverVarArgs consumes everything after T_vararg as well
         break;
         }

      if (needsRecovery(parm))
         {
         if (isArray(parm))
            {
            JBParameter &sizeParm = findSizeParameter(func->parms, p);
            recoverArrayArgument(parm, sizeParm);
            }
         else
            recoverArgument(parm);
         }
      }

   if (func->isConstructor())
      {
      indent() << "(" << implTypeCast(clazz->type) << "_impl)->setClient(this);\n";
      handleCallbacks(clazz);

      // initialize all client fields
      indent() << "void * impl = _impl;\n";
      initializeFieldsRecursive(clazz, clazz);
      }

   // return any return value
   if (func->returnType != T_none)
      {
      returnValue(func->returnType, "implReturnValue");
      }

   endFunctionDeclaration();
   }

void
SourceCPPFile::creatorBody(JBFunction *func, JBClass *clazz)
   {
   startFunctionDeclaration(func, clazz);

   // initialize all client fields
   initializeFields(clazz, clazz);
  
   if (!clazz->hasSuper())
      indent() << "_impl = impl;\n";

   // superclasses handle client assignment, so if this class has no super, assign the client object
   if (clazz->super == NULL)
      {
      // set the client object
      indent() << "(" << implTypeCast(clazz->type) << "impl)->setClient(this);\n";
      }

   handleCallbacks(clazz);

   endFunctionDeclaration();
   }

void
SourceCPPFile::constructorBody(JBFunction *func, JBClass *clazz)
   {
   }

void
SourceCPPFile::destructorBody(JBFunction *func, JBClass *clazz)
   {
   startFunctionDeclaration(func, clazz);
   //indent() << "delete " << implTypeCast(clazz->type) << "_impl;\n";
   endFunctionDeclaration();
   }
