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
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#include "ilgen/IlBuilderRecorder.hpp"
#include "ilgen/IlBuilder.hpp"

#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#include "infra/Assert.hpp"
#include "compile/Compilation.hpp"
#include "ilgen/JitBuilderRecorder.hpp"
#include "ilgen/TypeDictionary.hpp"
#include "ilgen/MethodBuilder.hpp"
#include "ilgen/IlValue.hpp"

#include "ilgen/JitBuilderRecorderTextFile.hpp"
#include "ilgen/JitBuilderReplayTextFile.hpp"
#include "ilgen/MethodBuilderReplay.hpp"
// IlBuilder is a class designed to help build Testarossa IL quickly without
// a lot of knowledge of the intricacies of commoned references, symbols,
// symbol references, or blocks. You can add operations to an IlBuilder via
// a set of services, for example: Add, Sub, Load, Store. These services
// operate on TR::IlValues, which currently correspond to SymbolReferences.
// However, IlBuilders also incorporate a notion of "symbols" that are
// identified by strings (i.e. arbitrary symbolic names).
//
// So Load("a") returns a TR::IlValue. Load("b") returns a different
// TR::IlValue. The value of a+b can be computed by Add(Load("a"),Load("b")),
// and that value can be stored into c by Store("c", Add(Load("a"),Load("b"))).
//
// More complicated services exist to construct control flow by linking
// together sets of IlBuilder objects. A simple if-then construct, for
// example, can be built with the following code:
//    TR::IlValue *condition = NotEqualToZero(Load("a"));
//    TR::IlBuilder *then = NULL;
//    IfThen(&then, condition);
//    then->Return(then->ConstInt32(0));
//
//
// An IlBuilder is really a sequence of Blocks and other IlBuilders, but an
// IlBuilder is also a sequence of TreeTops connecting together a set of
// Blocks that are arranged in the CFG. Another way to think of an IlBuilder
// is as representing the code needed to execute a particular code path
// (which may itasIlBuilder have sub control flow handled by embedded IlBuilders).
// All IlBuilders exist within the single control flow graph, but the full
// control flow graph will not be connected together until all IlBuilder
// objects have had injectIL() called.


OMR::IlBuilderRecorder::IlBuilderRecorder(TR::MethodBuilder *methodBuilder, TR::TypeDictionary *types)
      : TR::IlInjector(types),
      _methodBuilder(methodBuilder)
   {
   }

OMR::IlBuilderRecorder::IlBuilderRecorder(TR::IlBuilder *source)
      : TR::IlInjector(source),
      _methodBuilder(source->methodBuilder())
   {
   }

TR::IlBuilder *
OMR::IlBuilderRecorder::asIlBuilder()
   {
   return static_cast<TR::IlBuilder *>(this);
   }

TR::JitBuilderRecorder *
OMR::IlBuilderRecorder::recorder() const
   {
   return _methodBuilder->recorder();
   }

TR::JitBuilderRecorder *
OMR::IlBuilderRecorder::clearRecorder()
   {
   TR::JitBuilderRecorder *recorder = _methodBuilder->recorder();
   _methodBuilder->setRecorder(NULL);
   return recorder;
   }

void
OMR::IlBuilderRecorder::restoreRecorder(TR::JitBuilderRecorder *recorder)
   {
   _methodBuilder->setRecorder(recorder);
   }

void
OMR::IlBuilderRecorder::DoneConstructor(const char * value)
   {
     TR::JitBuilderRecorder *rec = recorder();
     if (rec)
        {
        rec->BeginStatement(asIlBuilder(), rec->STATEMENT_DONECONSTRUCTOR);
        rec->String(value);
        rec->EndStatement();
        }
   }

void
OMR::IlBuilderRecorder::NewIlBuilder()
   {
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(asIlBuilder());
      rec->BeginStatement(rec->STATEMENT_NEWILBUILDER);
      rec->Builder(asIlBuilder());
      rec->EndStatement();
      }
   }

TR::IlBuilder *
OMR::IlBuilderRecorder::createBuilderIfNeeded(TR::IlBuilder *builder)
   {
   if (builder == NULL)
      builder = asIlBuilder()->OrphanBuilder();
   return builder;
   }

TR::IlValue *
OMR::IlBuilderRecorder::newValue()
   {
   TR::IlValue *value = new (_comp->trHeapMemory()) TR::IlValue(_methodBuilder);
   return value;
   }

void
OMR::IlBuilderRecorder::binaryOp(const TR::IlValue *returnValue, const TR::IlValue *left, const TR::IlValue *right, const char *s)
   {
   TR::JitBuilderRecorder *rec = recorder();
   TR_ASSERT(rec, "cannot call IlBuilderRecorder::binaryOp if recorder is NULL");

   rec->StoreID(returnValue);
   rec->BeginStatement(asIlBuilder(), s);
   rec->Value(returnValue);
   rec->Value(left);
   rec->Value(right);
   rec->EndStatement();
   }

void
OMR::IlBuilderRecorder::shiftOp(const TR::IlValue *returnValue, const TR::IlValue *v, const TR::IlValue *amount, const char *s)
   {
   TR::JitBuilderRecorder *rec = recorder();
   TR_ASSERT(rec, "cannot call IlBuilderRecorder::shiftOp if recorder is NULL");

   rec->StoreID(returnValue);
   rec->BeginStatement(asIlBuilder(), s);
   rec->Value(returnValue);
   rec->Value(v);
   rec->Value(amount);
   rec->EndStatement();
   }

void
OMR::IlBuilderRecorder::AppendBuilder(TR::IlBuilder *builder)
   {
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_APPENDBUILDER);
      rec->Builder(builder);
      rec->EndStatement();
      }
   }

/**
 * @brief Store an IlValue into a named local variable
 * @param varName the name of the local variable to be stored into. If the name has not been used before, this local variable will
 *                take the same data type as the value being written to it.
 * @param value IlValue that should be written to the local variable, which should be the same data type
 */
void
OMR::IlBuilderRecorder::Store(const char *name, TR::IlValue *value)
   {
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_STORE);
      rec->String(name);
      rec->Value(value);
      rec->EndStatement();
      }
   }

/**
 * @brief Store an IlValue into the same local value as another IlValue
 * @param dest IlValue that should now hold the same value as "value"
 * @param value IlValue that should overwrite "dest"
 */
void
OMR::IlBuilderRecorder::StoreOver(TR::IlValue *dest, TR::IlValue *value)
   {
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_STOREOVER);
      rec->Value(dest);
      rec->Value(value);
      rec->EndStatement();
      }
   }

/**
 * @brief Store a vector IlValue into a named local variable
 * @param varName the name of the local variable to be vector stored into. if the name has not been used before, this local variable will
 *                take the same data type as the value being written to it. The width of this data will be determined by the vector
 *                data type of IlValue.
 * @param value IlValue with the vector data that should be written to the local variable, and should have the same data type
 */
void
OMR::IlBuilderRecorder::VectorStore(const char *name, TR::IlValue *value)
   {
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_VECTORSTORE);
      rec->String(name);
      rec->Value(value);
      rec->EndStatement();
      }
   }

/**
 * @brief Store an IlValue through a pointer
 * @param address the pointer address through which the value will be written
 * @param value IlValue that should be written at "address"
 */
void
OMR::IlBuilderRecorder::StoreAt(TR::IlValue *address, TR::IlValue *value)
   {
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_STOREAT);
      rec->Value(address);
      rec->Value(value);
      rec->EndStatement();
      }
   }

/**
 * @brief Store a vector IlValue through a pointer
 * @param address the pointer address through which the vector value will be written. The width of the store will be determined
 *                by the vector data type of IlValue.
 * @param value IlValue with the vector data that should be written  at "address"
 */
void
OMR::IlBuilderRecorder::VectorStoreAt(TR::IlValue *address, TR::IlValue *value)
   {
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_VECTORSTOREAT);
      rec->Value(address);
      rec->Value(value);
      rec->EndStatement();
      }
   }

TR::IlValue *
OMR::IlBuilderRecorder::CreateLocalArray(int32_t numElements, TR::IlType *elementType)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(returnValue);

      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_CREATELOCALARRAY);
      rec->Value(returnValue);
      rec->Number(numElements);
      rec->Type(elementType);
      rec->EndStatement();
      }
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::CreateLocalStruct(TR::IlType *structType)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(returnValue);

      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_CREATELOCALSTRUCT);
      rec->Value(returnValue);
      rec->Type(structType);
      rec->EndStatement();
      }
   return returnValue;
   }

void
OMR::IlBuilderRecorder::StoreIndirect(const char *type, const char *field, TR::IlValue *object, TR::IlValue *value)
  {
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_STOREINDIRECT);
      rec->String(type);
      rec->String(field);
      rec->Value(object);
      rec->Value(value);
      rec->EndStatement();
      }
  }

TR::IlValue *
OMR::IlBuilderRecorder::Load(const char *name)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(returnValue);

      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_LOAD);
      rec->Value(returnValue);
      rec->String(name);
      rec->EndStatement();
      }

   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::VectorLoad(const char *name)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(returnValue);

      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_VECTORLOAD);
      rec->Value(returnValue);
      rec->String(name);
      rec->EndStatement();
      }

   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::LoadIndirect(const char *type, const char *field, TR::IlValue *object)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(returnValue);

      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_LOADINDIRECT);
      rec->Value(returnValue);
      rec->String(type);
      rec->String(field);
      rec->Value(object);
      rec->EndStatement();
      }

   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::LoadAt(TR::IlType *dt, TR::IlValue *address)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(returnValue);
      dt->RecordFirstTime(rec);

      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_LOADAT);
      rec->Value(returnValue);
      rec->Type(dt);
      rec->Value(address);
      rec->EndStatement();
      }

   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::VectorLoadAt(TR::IlType *dt, TR::IlValue *address)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(returnValue);
      dt->RecordFirstTime(rec);

      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_VECTORLOADAT);
      rec->Value(returnValue);
      rec->Type(dt);
      rec->Value(address);
      rec->EndStatement();
      }

   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::IndexAt(TR::IlType *dt, TR::IlValue *base, TR::IlValue *index)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(returnValue);
      dt->RecordFirstTime(rec);

      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_INDEXAT);
      rec->Value(returnValue);
      rec->Type(dt);
      rec->Value(base);
      rec->Value(index);
      rec->EndStatement();
      }

   return returnValue;
   }

/**
 * @brief Generate IL to load the address of a struct field.
 *
 * The address of the field is calculated by adding the field's offset to the
 * base address of the struct. The address is also converted (type casted) to
 * a pointer to the type of the field.
 */
TR::IlValue *
OMR::IlBuilderRecorder::StructFieldInstanceAddress(const char* structName, const char* fieldName, TR::IlValue* obj)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(returnValue);

      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_STRUCTFIELDINSTANCEADDRESS);
      rec->Value(returnValue);
      rec->String(structName);
      rec->String(fieldName);
      rec->Value(obj);
      rec->EndStatement();
      }

   return returnValue;
   }

/**
 * @brief Generate IL to load the address of a union field.
 *
 * The address of the field is simply the base address of the union, since the
 * offset of all union fields is zero.
 */
TR::IlValue *
OMR::IlBuilderRecorder::UnionFieldInstanceAddress(const char* unionName, const char* fieldName, TR::IlValue* obj)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(returnValue);

      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_UNIONFIELDINSTANCEADDRESS);
      rec->Value(returnValue);
      rec->String(unionName);
      rec->String(fieldName);
      rec->Value(obj);
      rec->EndStatement();
      }

   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::NullAddress()
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(returnValue);

      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_NULLADDRESS);
      rec->Value(returnValue);
      rec->EndStatement();
      }

   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::ConstInt8(int8_t value)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(returnValue);
      Int8->RecordFirstTime(rec);

      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_CONSTINT8);
      rec->Value(returnValue);
      rec->Number(value);
      rec->EndStatement();
      }
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::ConstInt16(int16_t value)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(returnValue);
      Int16->RecordFirstTime(rec);

      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_CONSTINT16);
      rec->Value(returnValue);
      rec->Number(value);
      rec->EndStatement();
      }
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::ConstInt32(int32_t value)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(returnValue);
      Int32->RecordFirstTime(rec);

      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_CONSTINT32);
      rec->Value(returnValue);
      rec->Number(value);
      rec->EndStatement();
      }
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::ConstInt64(int64_t value)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(returnValue);
      Int64->RecordFirstTime(rec);

      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_CONSTINT64);
      rec->Value(returnValue);
      rec->Number(value);
      rec->EndStatement();
      }
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::ConstFloat(float value)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(returnValue);
      Float->RecordFirstTime(rec);

      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_CONSTFLOAT);
      rec->Value(returnValue);
      rec->Number(value);
      rec->EndStatement();
      }
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::ConstDouble(double value)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(returnValue);
      Double->RecordFirstTime(rec);

      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_CONSTDOUBLE);
      rec->Value(returnValue);
      rec->Number(value);
      rec->EndStatement();
      }
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::Add(TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      binaryOp(returnValue, left, right, rec->STATEMENT_ADD);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::LessThan(TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      binaryOp(returnValue, left, right, rec->STATEMENT_LESSTHAN);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::GreaterThan(TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      binaryOp(returnValue, left, right, rec->STATEMENT_GREATERTHAN);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::EqualTo(TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::ConstString(const char * const value)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(returnValue);
      Address->RecordFirstTime(rec);

      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_CONSTSTRING);
      rec->Value(returnValue);
      rec->String(value);
      rec->EndStatement();
      }
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::ConstAddress(const void * const value)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(returnValue);
      Address->RecordFirstTime(rec);

      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_CONSTADDRESS);
      rec->Value(returnValue);
      rec->Location(value);
      rec->EndStatement();
      }
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::ConstInteger(TR::IlType *intType, int64_t value)
   {
   if      (intType == Int8)  return ConstInt8 ((int8_t)  value);
   else if (intType == Int16) return ConstInt16((int16_t) value);
   else if (intType == Int32) return ConstInt32((int32_t) value);
   else if (intType == Int64) return ConstInt64(          value);

   TR_ASSERT(0, "unknown integer type");
   return NULL;
   }

TR::IlValue *
OMR::IlBuilderRecorder::ConvertTo(TR::IlType *dt, TR::IlValue *v)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      convertTo(returnValue, dt, v, rec->STATEMENT_CONVERTTO);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::UnsignedConvertTo(TR::IlType *dt, TR::IlValue *v)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      convertTo(returnValue, dt, v, rec->STATEMENT_UNSIGNEDCONVERTTO);
   return returnValue;
   }

void
OMR::IlBuilderRecorder::convertTo(TR::IlValue *returnValue, TR::IlType *dt, TR::IlValue *v, const char *s)
   {
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(returnValue);
      dt->RecordFirstTime(rec);

      rec->BeginStatement(asIlBuilder(), s);
      rec->Value(returnValue);
      rec->Type(dt);
      rec->Value(v);
      rec->EndStatement();
      }
   }

void
OMR::IlBuilderRecorder::unaryOp(TR::IlValue *returnValue, TR::IlValue *v, const char *s)
   {
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->StoreID(returnValue);

      rec->BeginStatement(s);
      rec->Value(returnValue);
      rec->Value(v);
      rec->EndStatement();
      }
   }

TR::IlValue *
OMR::IlBuilderRecorder::ShiftL(TR::IlValue *v, TR::IlValue *amount)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::ShiftR(TR::IlValue *v, TR::IlValue *amount)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::UnsignedShiftR(TR::IlValue *v, TR::IlValue *amount)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      shiftOp(returnValue, v, amount, rec->STATEMENT_UNSIGNEDSHIFTR);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::NotEqualTo(TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      binaryOp(returnValue, left, right, rec->STATEMENT_NOTEQUALTO);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::LessOrEqualTo(TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::GreaterOrEqualTo(TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::UnsignedLessThan(TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::UnsignedGreaterThan(TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::UnsignedLessOrEqualTo(TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::UnsignedGreaterOrEqualTo(TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   return returnValue;
   }

void
OMR::IlBuilderRecorder::Goto(TR::IlBuilder **dest)
   {
   *dest = createBuilderIfNeeded(*dest);
   Goto(*dest);
   }

void
OMR::IlBuilderRecorder::Goto(TR::IlBuilder *dest)
   {
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_GOTO);
      rec->Builder(dest);
      rec->EndStatement();
      }
   }

void
OMR::IlBuilderRecorder::Return()
   {
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_RETURN);
      rec->EndStatement();
      }
   }

void
OMR::IlBuilderRecorder::Return(TR::IlValue *value)
   {
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_RETURNVALUE);
      rec->Value(value);
      rec->EndStatement();
      }
   }

TR::IlValue *
OMR::IlBuilderRecorder::Sub(TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      binaryOp(returnValue, left, right, rec->STATEMENT_SUB);
   return returnValue;
   }


TR::IlValue *
OMR::IlBuilderRecorder::Mul(TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      binaryOp(returnValue, left, right, rec->STATEMENT_MUL);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::Div(TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      binaryOp(returnValue, left, right, rec->STATEMENT_DIV);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::Rem(TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   return returnValue;
   }

void
OMR::IlBuilderRecorder::assertNotRecorded(TR::JitBuilderRecorder *rec)
   {
   if (rec)
      TR_ASSERT(0, "Something is not being recorded\n");
   }

TR::IlValue *
OMR::IlBuilderRecorder::And(TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      binaryOp(returnValue, left, right, rec->STATEMENT_AND);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::Or(TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      binaryOp(returnValue, left, right, rec->STATEMENT_OR);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::Xor(TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      binaryOp(returnValue, left, right, rec->STATEMENT_XOR);
   return returnValue;
   }

void
OMR::IlBuilderRecorder::IfCmpEqualZero(TR::IlBuilder *target, TR::IlValue *condition)
   {
   TR::JitBuilderRecorder *rec = recorder();

   if(rec)
      {
      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_IFCMPEQUALZERO);
      rec->Builder(target);
      rec->Value(condition);
      rec->EndStatement();
      }
   }

#if 0
/*
 * blockThrowsException:
 * ....
 * goto blockAfterExceptionHandler
 * Handler:
 * ....
 * goto blockAfterExceptionHandler
 * blockAfterExceptionHandler:
 */
void
OMR::IlBuilder::appendExceptionHandler(TR::Block *blockThrowsException, TR::IlBuilder **handler, uint32_t catchType)
   {
   //split block after overflowCHK, and add an goto to blockAfterExceptionHandler
   appendBlock();
   TR::Block *blockWithGoto = _currentBlock;
   TR::Node *gotoNode = TR::Node::create(NULL, TR::Goto);
   genTreeTop(gotoNode);
   _currentBlock = NULL;
   _currentBlockNumber = -1;

   //append handler, add exception edge and merge edge
   *handler = createBuilderIfNeeded(*handler);
   TR_ASSERT(*handler != NULL, "exception handler cannot be NULL\n");
   (*handler)->_isHandler = true;
   cfg()->addExceptionEdge(blockThrowsException, (*handler)->getEntry());
   AppendBuilder(*handler);
   (*handler)->setHandlerInfo(catchType);

   TR::Block *blockAfterExceptionHandler = _currentBlock;
   TraceIL("blockAfterExceptionHandler block_%d, blockWithGoto block_%d \n", blockAfterExceptionHandler->getNumber(), blockWithGoto->getNumber());
   gotoNode->setBranchDestination(blockAfterExceptionHandler->getEntry());
   cfg()->addEdge(blockWithGoto, blockAfterExceptionHandler);
   }

void
OMR::IlBuilderRecorder::setHandlerInfo(uint32_t catchType)
   {
   TR::Block *catchBlock = getEntry();
   catchBlock->setIsCold();
   catchBlock->setHandlerInfoWithOutBCInfo(catchType, comp()->getInlineDepth(), -1, _methodSymbol->getResolvedMethod(), comp());
   }

TR::Node*
OMR::IlBuilderRecorder::genOverflowCHKTreeTop(TR::Node *operationNode, TR::ILOpCodes overflow)
   {
   TR::Node *overflowChkNode = TR::Node::createWithRoomForOneMore(overflow, 3, symRefTab()->findOrCreateOverflowCheckSymbolRef(_methodSymbol), operationNode, operationNode->getFirstChild(), operationNode->getSecondChild());
   overflowChkNode->setOverflowCheckOperation(operationNode->getOpCodeValue());
   genTreeTop(overflowChkNode);
   return overflowChkNode;
   }

TR::IlValue *
OMR::IlBuilderRecorder::genOperationWithOverflowCHK(TR::ILOpCodes op, TR::Node *leftNode, TR::Node *rightNode, TR::IlBuilder **handler, TR::ILOpCodes overflow)
   {
   /*
    * BB1:
    *    overflowCHK
    *       operation(add/sub/mul)
    *          child1
    *          child2
    *       =>child1
    *       =>child2
    *    store
    *       => operation
    * BB2:
    *    goto BB3
    * Handler:
    *    ...
    * BB3:
    *    continue
    */
   TR::Node *operationNode = binaryOpNodeFromNodes(op, leftNode, rightNode);
   TR::Node *overflowChkNode = genOverflowCHKTreeTop(operationNode, overflow);

   TR::Block *blockWithOverflowCHK = _currentBlock;
   TR::IlValue *resultValue = newValue(operationNode->getDataType(), operationNode);
   genTreeTop(TR::Node::createStore(resultValue->getSymbolReference(), operationNode));

   appendExceptionHandler(blockWithOverflowCHK, handler, TR::Block::CanCatchOverflowCheck);
   return resultValue;
   }

// This function takes 4 arguments and generate the addValue.
// This function is called by AddWithOverflow and AddWithUnsignedOverflow.
TR::ILOpCodes
OMR::IlBuilderRecorder::getOpCode(TR::IlValue *leftValue, TR::IlValue *rightValue)
   {
   TR::ILOpCodes op;
   if (leftValue->getDataType() == TR::Address)
      {
      if (rightValue->getDataType() == TR::Int32)
         op = TR::aiadd;
      else if (rightValue->getDataType() == TR::Int64)
         op = TR::aladd;
      else
         TR_ASSERT(0, "the right child type must be either TR::Int32 or TR::Int64 when the left child of Add is TR::Address\n");
      }
   else
      {
      op = addOpCode(leftValue->getDataType());
      }
   return op;
   }
#endif 

TR::IlValue *
OMR::IlBuilderRecorder::AddWithOverflow(TR::IlBuilder **handler, TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::AddWithUnsignedOverflow(TR::IlBuilder **handler, TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::SubWithOverflow(TR::IlBuilder **handler, TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::SubWithUnsignedOverflow(TR::IlBuilder **handler, TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::MulWithOverflow(TR::IlBuilder **handler, TR::IlValue *left, TR::IlValue *right)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   return returnValue;
   }

/*
 * @brief IfAnd service for constructing short circuit AND conditional nests (like the && operator)
 * @param allTrueBuilder builder containing operations to execute if all conditional tests evaluate to true
 * @param anyFalseBuilder builder containing operations to execute if any conditional test is false
 * @param numTerms the number of conditional terms
 * @param ... for each term, provide a TR::IlBuilder object and a TR::IlValue object that evaluates a condition (builder is where all the operations to evaluate the condition go, the value is the final result of the condition)
 *
 * Example:
 * TR::IlBuilder *cond1Builder = OrphanBuilder();
 * TR::IlValue *cond1 = cond1Builder->GreaterOrEqual(
 *                      cond1Builder->   Load("x"),
 *                      cond1Builder->   Load("lower"));
 * TR::IlBuilder *cond2Builder = OrphanBuilder();
 * TR::IlValue *cond2 = cond2Builder->LessThan(
 *                      cond2Builder->   Load("x"),
 *                      cond2Builder->   Load("upper"));
 * TR::IlBuilder *inRange = NULL, *outOfRange = NULL;
 * IfAnd(&inRange, &outOfRange, 2, cond1Builder, cond1, cond2Builder, cond2);
 */
void
OMR::IlBuilderRecorder::IfAnd(TR::IlBuilder **allTrueBuilder, TR::IlBuilder **anyFalseBuilder, int32_t numTerms, ...)
   {
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   }

/*
 * @brief IfOr service for constructing short circuit OR conditional nests (like the || operator)
 * @param anyTrueBuilder builder containing operations to execute if any conditional test evaluates to true
 * @param allFalseBuilder builder containing operations to execute if all conditional tests are false
 * @param numTerms the number of conditional terms
 * @param ... for each term, provide a TR::IlBuilder object and a TR::IlValue object that evaluates a condition (builder is where all the operations to evaluate the condition go, the value is the final result of the condition)
 *
 * Example:
 * TR::IlBuilder *cond1Builder = asIlBuilder()->OrphanBuilder();
 * TR::IlValue *cond1 = cond1Builder->LessThan(
 *                      cond1Builder->   Load("x"),
 *                      cond1Builder->   Load("lower"));
 * TR::IlBuilder *cond2Builder = asIlBuilder()->OrphanBuilder();
 * TR::IlValue *cond2 = cond2Builder->GreaterOrEqual(
 *                      cond2Builder->   Load("x"),
 *                      cond2Builder->   Load("upper"));
 * TR::IlBuilder *inRange = NULL, *outOfRange = NULL;
 * IfOr(&outOfRange, &inRange, 2, cond1Builder, cond1, cond2Builder, cond2);
 */
void
OMR::IlBuilderRecorder::IfOr(TR::IlBuilder **anyTrueBuilder, TR::IlBuilder **allFalseBuilder, int32_t numTerms, ...)
   {
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   }

#if 0

void
OMR::IlBuilderRecorder::integerizeAddresses(TR::IlValue **leftPtr, TR::IlValue **rightPtr)
   {
   TR::IlValue *left = *leftPtr;
   if (left->getDataType() == TR::Address)
      *leftPtr = ConvertTo(Word, left);

   TR::IlValue *right = *rightPtr;
   if (right->getDataType() == TR::Address)
      *rightPtr = ConvertTo(Word, right);
   }

TR::IlValue**
OMR::IlBuilderRecorder::processCallArgs(TR::Compilation *comp, int numArgs, va_list args)
   {
   TR::IlValue ** argValues = (TR::IlValue **) comp->trMemory()->allocateHeapMemory(numArgs * sizeof(TR::IlValue *));
   for (int32_t a=0;a < numArgs;a++)
      {
      argValues[a] = va_arg(args, TR::IlValue*);
      }
   return argValues;
   }

#endif

/*
 * \param numArgs
 *    Number of actual arguments for the method  plus 1
 * \param ...
 *    The list is a computed address followed by the actual arguments
 */
TR::IlValue *
OMR::IlBuilderRecorder::ComputedCall(const char *functionName, int32_t numArgs, ...)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   return returnValue;
   }

/*
 * \param numArgs
 *    Number of actual arguments for the method  plus 1
 * \param argValues
 *    the computed address followed by the actual arguments
 */
TR::IlValue *
OMR::IlBuilderRecorder::ComputedCall(const char *functionName, int32_t numArgs, TR::IlValue **argValues)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   return returnValue;
   }

TR::IlValue *
OMR::IlBuilderRecorder::Call(const char *functionName, TR::DataType returnType, int32_t numArgs, TR::IlValue ** argValues)
   {
   TR::IlValue *returnValue = NULL;
   if (returnType != TR::NoType)
      {
      returnValue = newValue();
      }
   TR::JitBuilderRecorder *rec = recorder();
   if (NULL != rec)
      {
      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_CALL);
      rec->String(functionName);
      rec->Number(numArgs);
      for (int32_t v=0;v < numArgs;v++)
         rec->Value(argValues[v]);
      if (returnType != TR::NoType)
         {
         rec->StoreID(returnValue);
         rec->Value(returnValue);
         }
      rec->EndStatement();
      }
   return returnValue;
   }

#if 0
TR::IlValue *
OMR::IlBuilderRecorder::genCall(TR::SymbolReference *methodSymRef, int32_t numArgs, TR::IlValue ** argValues, bool isDirectCall /* true by default*/)
   {
   TR::DataType returnType = methodSymRef->getSymbol()->castToMethodSymbol()->getMethod()->returnType();
   TR::Node *callNode = TR::Node::createWithSymRef(isDirectCall? TR::ILOpCode::getDirectCall(returnType): TR::ILOpCode::getIndirectCall(returnType), numArgs, methodSymRef);

   // TODO: should really verify argument types here
   int32_t childIndex = 0;
   for (int32_t a=0;a < numArgs;a++)
      {
      TR::IlValue *arg = argValues[a];
      if (arg->getDataType() == TR::Int8 || arg->getDataType() == TR::Int16 || Word == Int64 && arg->getDataType() == TR::Int32)
         arg = ConvertTo(Word, arg);
      callNode->setAndIncChild(childIndex++, loadValue(arg));
      }

   // callNode must be anchored by itasIlBuilder
   genTreeTop(callNode);

   if (returnType != TR::NoType)
      {
      TR::IlValue *returnValue = newValue(callNode->getDataType(), callNode);
      return returnValue;
      }

   return NULL;
   }

#endif 

/** \brief
 *     The service is used to atomically increase memory location [\p baseAddress + \p offset] by amount of \p value.
 *
 *  \param value
 *     The amount to increase for the memory location.
 *
 *  \note
 *	   This service currently only supports Int32/Int64.
 *
 *  \return
 *     The old value at the location [\p baseAddress + \p offset].
 */
TR::IlValue *
OMR::IlBuilderRecorder::AtomicAddWithOffset(TR::IlValue * baseAddress, TR::IlValue * offset, TR::IlValue * value)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   return returnValue;
   }

/**
 * \brief
 *  The service is for generating a treetop for transaction begin when the user needs to use transactional memory.
 *  At the end of transaction path, service will automatically generate transaction end instruction.
 *
 * \verbatim
 *
 *    +---------------------------------+
 *    |<block_$tstart>                  |
 *    |==============                   |
 *    | TSTART                          |
 *    |    |                            |
 *    |    |_ <block_$persistentFailure>|
 *    |    |_ <block_$transientFailure> |
 *    |    |_ <block_$transaction>      |+------------------------+----------------------------------+
 *    |                                 |                         |                                  |
 *    +---------------------------------+                         |                                  |
 *                       |                                        |                                  |
 *                       v                                        V                                  V
 *    +-----------------------------------------+   +-------------------------------+    +-------------------------+
 *    |<block_$transaction>                     |   |<block_$persistentFailure>     |    |<block_$transientFailure>|
 *    |====================                     |   |===========================    |    |=========================|
 *    |     add (For illustration, we take add  |   |AtomicAdd (For illustration, we|    |   ...                   |
 *    |     ... as an example. User could       |   |...       take atomicAdd as an |    |   ...                   |
 *    |     ... replace it with other non-atomic|   |...       example. User could  |    |   ...                   |
 *    |     ... operations.)                    |   |...       replace it with other|    |   ...                   |
 *    |     ...                                 |   |...       atomic operations.)  |    |   ...                   |
 *    |     TEND                                |   |...                            |    |   ...                   |
 *    |goto --> block_$merge                    |   |goto->block_$merge             |    |goto->block_$merge       |
 *    +----------------------------------------+    +-------------------------------+    +-------------------------+
 *                      |                                          |                                 |
 *                      |                                          |                                 |
 *                      |------------------------------------------+---------------------------------+
 *                      |
 *                      v
 *              +----------------+
 *              | block_$merge   |
 *              | ============== |
 *              |      ...       |
 *              +----------------+
 *
 * \endverbatim
 *
 * \structure & \param value
 *     tstart
 *       |
 *       ----persistentFailure // This is a permanent failure, try atomic way to do it instead
 *       |
 *       ----transientFailure // Temporary failure, user can retry
 *       |
 *       ----transaction // Success, use general(non-atomic) way to do it
 *                |
 *                ---- non-atomic operations
 *                |
 *                ---- ...
 *                |
 *                ---- tend
 *
 * \note
 *      If user's platform doesn't support TM, go to persistentFailure path directly/
 *      In this case, current IlBuilder walks around transientFailureBuilder and transactionBuilder
 *      and goes to persistentFailureBuilder.
 *
 *      _currentBuilder
 *          |
 *          ->Goto()
 *              |   transientFailureBuilder
 *              |   transactionBuilder
 *              |-->persistentFailurie
 */
void
OMR::IlBuilderRecorder::Transaction(TR::IlBuilder **persistentFailureBuilder, TR::IlBuilder **transientFailureBuilder, TR::IlBuilder **transactionBuilder)
   {
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   }


/**
 * Generate XABORT instruction to abort transaction
 */
void
OMR::IlBuilderRecorder::TransactionAbort()
   {
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   }

void
OMR::IlBuilderRecorder::IfCmpNotEqualZero(TR::IlBuilder *target, TR::IlValue *condition)
   {
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   }

void
OMR::IlBuilderRecorder::IfCmpNotEqual(TR::IlBuilder *target, TR::IlValue *left, TR::IlValue *right)
   {
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   }

void
OMR::IlBuilderRecorder::IfCmpEqual(TR::IlBuilder *target, TR::IlValue *left, TR::IlValue *right)
   {
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   }

void
OMR::IlBuilderRecorder::IfCmpLessThan(TR::IlBuilder *target, TR::IlValue *left, TR::IlValue *right)
   {
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   }

void
OMR::IlBuilderRecorder::IfCmpUnsignedLessThan(TR::IlBuilder *target, TR::IlValue *left, TR::IlValue *right)
   {
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   }

void
OMR::IlBuilderRecorder::IfCmpLessOrEqual(TR::IlBuilder *target, TR::IlValue *left, TR::IlValue *right)
   {
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   }

void
OMR::IlBuilderRecorder::IfCmpUnsignedLessOrEqual(TR::IlBuilder *target, TR::IlValue *left, TR::IlValue *right)
   {
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   }

void
OMR::IlBuilderRecorder::IfCmpGreaterThan(TR::IlBuilder *target, TR::IlValue *left, TR::IlValue *right)
   {
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   }

void
OMR::IlBuilderRecorder::IfCmpUnsignedGreaterThan(TR::IlBuilder *target, TR::IlValue *left, TR::IlValue *right)
   {
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   }

void
OMR::IlBuilderRecorder::IfCmpGreaterOrEqual(TR::IlBuilder *target, TR::IlValue *left, TR::IlValue *right)
   {
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   }

void
OMR::IlBuilderRecorder::IfCmpUnsignedGreaterOrEqual(TR::IlBuilder *target, TR::IlValue *left, TR::IlValue *right)
   {
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   }

#if 0

void
OMR::IlBuilderRecorder::ifCmpCondition(TR_ComparisonTypes ct, bool isUnsignedCmp, TR::IlValue *left, TR::IlValue *right, TR::Block *target)
   {
   integerizeAddresses(&left, &right);
   TR::Node *leftNode = loadValue(left);
   TR::Node *rightNode = loadValue(right);
   TR::ILOpCode cmpOpCode(TR::ILOpCode::compareOpCode(leftNode->getDataType(), ct, isUnsignedCmp));
   ifjump(cmpOpCode.convertCmpToIfCmp(),
          leftNode,
          rightNode,
          target);
   appendBlock();
   }

void
OMR::IlBuilderRecorder::ifCmpNotEqualZero(TR::IlValue *condition, TR::Block *target)
   {
   ifCmpCondition(TR_cmpNE, false, condition, zeroForValue(condition), target);
   }

void
OMR::IlBuilderRecorder::ifCmpEqualZero(TR::IlValue *condition, TR::Block *target)
   {
   ifCmpCondition(TR_cmpEQ, false, condition, zeroForValue(condition), target);
   }

void
OMR::IlBuilderRecorder::appendGoto(TR::Block *destBlock)
   {
   gotoBlock(destBlock);
   appendBlock();
   }
#endif

/* Flexible builder for if...then...else structures
 * Basically 3 ways to call it:
 *    1) with then and else paths
 *    2) only a then path
 *    3) only an else path
 * #2 and #3 are similar: the existing path is appended and the proper condition causes branch around to the merge point
 * #1: then path exists "out of line", else path falls through from If and then jumps to merge, followed by then path which
 *     falls through to the merge point
 */
void
OMR::IlBuilderRecorder::IfThenElse(TR::IlBuilder *thenPath, TR::IlBuilder *elsePath, TR::IlValue *condition)
   {
   if (recorder())
      {
      recorder()->BeginStatement(asIlBuilder(), recorder()->STATEMENT_IFTHENELSE);
      recorder()->Builder(thenPath);
      if (elsePath)
         recorder()->Builder(elsePath);
      else
         recorder()->Builder(NULL);
      recorder()->Value(condition);
      recorder()->EndStatement();
      }
   }

void
OMR::IlBuilderRecorder::IfThenElse(TR::IlBuilder **thenPath, TR::IlBuilder **elsePath, TR::IlValue *condition)
   {
   TR_ASSERT(thenPath != NULL || elsePath != NULL, "IfThenElse needs at least one conditional path");

   TR::IlBuilder *bThen=NULL;
   if (thenPath)
      bThen = *thenPath = createBuilderIfNeeded(*thenPath);

   TR::IlBuilder *bElse=NULL;
   if (elsePath)
      bElse = *elsePath = createBuilderIfNeeded(*elsePath);

   IfThenElse(bThen, bElse, condition);
   }

void
OMR::IlBuilderRecorder::Switch(const char *selectionVar,
                  TR::IlBuilder **defaultBuilder,
                  uint32_t numCases,
                  int32_t *caseValues,
                  TR::IlBuilder **caseBuilders,
                  bool *caseFallsThrough)
   {
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   }

// TODO: Which switch to keep?
void
OMR::IlBuilderRecorder::Switch(const char *selectionVar,
                  TR::IlBuilder **defaultBuilder,
                  uint32_t numCases,
                  ...)
   {
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   }

void
OMR::IlBuilderRecorder::ForLoop(bool countsUp,
                                const char *indVar,
                                TR::IlBuilder *bLoop,
                                TR::IlBuilder *bBreak,
                                TR::IlBuilder *bContinue,
                                TR::IlValue *initial,
                                TR::IlValue *end,
                                TR::IlValue *increment)
   {
   TR::JitBuilderRecorder *rec = recorder();
   if (rec)
      {
      rec->BeginStatement(asIlBuilder(), rec->STATEMENT_FORLOOP);
      // rec->Number((int8_t)countsUp);
      if(countsUp)
         rec->Number(1); // True
      else
         rec->Number(0); // False
      rec->String(indVar);
      rec->Builder(bLoop);
      rec->EnsureAvailableID(bBreak);
      rec->Builder(bBreak);
      rec->EnsureAvailableID(bContinue);
      rec->Builder(bContinue);
      rec->Value(initial);
      rec->Value(end);
      rec->Value(increment);
      rec->EndStatement();
      }
   }

void
OMR::IlBuilderRecorder::ForLoop(bool countsUp,
                                const char *indVar,
                                TR::IlBuilder **loopCode,
                                TR::IlBuilder **breakBuilder,
                                TR::IlBuilder **continueBuilder,
                                TR::IlValue *initial,
                                TR::IlValue *end,
                                TR::IlValue *increment)
   {
   TR_ASSERT(loopCode != NULL, "ForLoop needs to have loopCode builder");
   TR::IlBuilder *bLoop = *loopCode = createBuilderIfNeeded(*loopCode);

   TR::IlBuilder *bBreak = NULL;
   if (breakBuilder)
      {
      TR_ASSERT(*breakBuilder == NULL, "ForLoop returns breakBuilder, cannot provide breakBuilder as input");
      bBreak = *breakBuilder = asIlBuilder()->OrphanBuilder();
      }

   TR::IlBuilder *bContinue = NULL;
   if (continueBuilder)
      {
      TR_ASSERT(*continueBuilder == NULL, "ForLoop returns continueBuilder, cannot provide continueBuilder as input");
      bContinue = *continueBuilder = asIlBuilder()->OrphanBuilder();
      }

   ForLoop(countsUp, indVar, *loopCode, bBreak, bContinue, initial, end, increment);
   }

void
OMR::IlBuilderRecorder::DoWhileLoop(const char *whileCondition, TR::IlBuilder **body, TR::IlBuilder **breakBuilder, TR::IlBuilder **continueBuilder)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
    }

void 
OMR::IlBuilderRecorder::WhileDoLoop(const char *whileCondition, TR::IlBuilder **body, TR::IlBuilder **breakBuilder, TR::IlBuilder **continueBuilder)
   {
   TR::IlValue *returnValue = newValue();
   TR::JitBuilderRecorder *rec = recorder();
   assertNotRecorded(rec);
   }

