/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#pragma once

#include "stdafx.h"

#include <iostream>
#include <string>

#include <typeinfo>

#include <boost/fusion/sequence.hpp>
#include <boost/fusion/include/sequence.hpp>

#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/include/algorithm.hpp>

#include <boost/preprocessor/tuple/elem.hpp>

#include <boost/preprocessor/cat.hpp>

#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>

#include <boost/preprocessor/punctuation/comma_if.hpp>

#include "AutoIncKeys.h"

// record descriptors require three fields
//  first field is always key field
#define OU_DB_RECORD_FIELD_DESCR_COUNT 3
#define OU_DB_IX_ENUM 0
#define OU_DB_IX_CLASS 1
#define OU_DB_IX_VN 2 /* variable name */

// basic extraction routines for struct members, tuple members, and reference between struct and tuple
#define OU_DB_EXTRACT_RECORD_FIELD( z, tuple, field, data ) \
	BOOST_PP_TUPLE_ELEM( OU_DB_RECORD_FIELD_DESCR_COUNT, field, tuple )

#define OU_DB_EMIT_VAR_DECLARATION( r, data, elem ) \
  OU_DB_EXTRACT_RECORD_FIELD( r, elem, OU_DB_IX_CLASS, data )::fldStored_t \
  OU_DB_EXTRACT_RECORD_FIELD( r, elem, OU_DB_IX_VN, data );

#define OU_DB_EMIT_TYPE_LIST( r, data, fieldix, tuple  ) \
	BOOST_PP_COMMA_IF(fieldix)\
	OU_DB_EXTRACT_RECORD_FIELD( r, tuple, OU_DB_IX_CLASS, data )

#define OU_DB_EMIT_REF_LIST( r, data, i, elem ) \
	BOOST_PP_COMMA_IF(i) \
	BOOST_PP_CAT(BOOST_PP_CAT(m_rec,data).,OU_DB_EXTRACT_RECORD_FIELD( r, elem, OU_DB_IX_VN, ~ ))

#define OU_DB_EMIT_ENUM_LIST( r, data, i, elem ) \
  BOOST_PP_COMMA_IF(i) \
  OU_DB_EXTRACT_RECORD_FIELD( r, elem, OU_DB_IX_ENUM, ~ )

// simplifying defines
#define OU_DB_STRUCT_ENUMS( record_fields ) \
  BOOST_PP_SEQ_FOR_EACH_I( OU_DB_EMIT_ENUM_LIST, ~, record_fields )

#define OU_DB_STRUCT_MEMBERS( record_fields ) \
  BOOST_PP_SEQ_FOR_EACH( OU_DB_EMIT_VAR_DECLARATION, ~, record_fields )

#define OU_DB_TUPLE_MEMBERS( record_fields ) \
  BOOST_PP_SEQ_FOR_EACH_I( OU_DB_EMIT_TYPE_LIST, ~, record_fields )

#define OU_DB_REFERENCES( record_fields, data ) \
	BOOST_PP_SEQ_FOR_EACH_I( OU_DB_EMIT_REF_LIST, data, record_fields )

#define OU_DB_DECLARE_STRUCTURES( base_name, record_fields ) \
  enum BOOST_PP_CAT(enum,base_name) { OU_DB_STRUCT_ENUMS(record_fields) }; \
	struct BOOST_PP_CAT(struct,base_name) { \
		OU_DB_STRUCT_MEMBERS( record_fields ) \
	}; \
  BOOST_PP_CAT(struct,base_name) BOOST_PP_CAT(m_rec,base_name); \
  typedef boost::fusion::vector<OU_DB_TUPLE_MEMBERS( record_fields )> BOOST_PP_CAT(BOOST_PP_CAT(tpl,base_name),_t); \
  BOOST_PP_CAT(BOOST_PP_CAT(tpl,base_name),_t) BOOST_PP_CAT(m_tpl,base_name); \
  /* */

// OU_DB_DECLARE_STRUCTURES example output:
//  struct structAccountAdvisor { ProcessFieldPk::stored_t pk; ProcessFieldSk::stored_t sAdvisorName;   }; 
//  structAccountAdvisor m_recAccountAdvisor; 
//  typedef boost::fusion::vector< ProcessFieldPk , ProcessFieldSk  > tplAccountAdvisor_t; tplAccountAdvisor_t m_tplAccountAdvisor;

#define OU_DB_INITIALIZE_STRUCTURES( base_name, record_fields ) \
  BOOST_PP_CAT(m_tpl,base_name)(OU_DB_REFERENCES( record_fields, base_name) ) \
  /* */

// OU_DB_INITIALIZE_STRUCTURES example output:
//  m_tplAccountAdvisor( m_recAccountAdvisor.pk , m_recAccountAdvisor.sAdvisorName   )

template <typename H, typename S> // H handler, S type for storage
class ProcessFieldBase
{
public:

  typedef S fldLocal_t;
  typedef S fldStored_t;

  ProcessFieldBase( fldStored_t& stored ): m_stored( stored ) {};
  virtual ~ProcessFieldBase( void ) {};

  fldStored_t const& constant_value( void ) const { return m_stored; };
  fldStored_t& value( void ) { return m_stored; };

  fldStored_t& operator=( fldStored_t& rhs ) {
    m_stored = rhs;
    return m_stored;
  }

  void SetKey( void** data, u_int32_t& size ) {
    *data = static_cast<void*>( &m_local );
    size = sizeof( fldStored_t );
  }

  void SetKey( Dbt& key ) {
    key.set_data( static_cast<void*>( &m_stored ) );
    key.set_size( sizeof( fldStored_t ) );
    key.set_ulen( sizeof( fldStored_t ) );
  }

  void PreAppend( void ) {};
  void PreInsert( void ) {};
  void PreUpdate( void ) {};
  void PreDelete( void ) {};
  void PostFetch( void ) {};

protected:
  fldStored_t& m_stored;
private:
  ProcessFieldBase( void ); // no default constructor
};

template <typename T> // T is a plain old datatype or composite structure (multi-field key)
class ProcessFieldPod: public ProcessFieldBase<ProcessFieldPod<T>,T> {
public:
  ProcessFieldPod( T& t ): ProcessFieldBase( t ) {};
  ~ProcessFieldPod( void ) {};
protected:
private:
  ProcessFieldPod(void);
};

// auto inc key
class ProcessFieldAutoIncKey: public ProcessFieldBase<ProcessFieldAutoIncKey,CAutoIncKeys::keyValue_t> {
public:
  ProcessFieldAutoIncKey( CAutoIncKeys::keyValue_t& key )
    : ProcessFieldBase<ProcessFieldAutoIncKey,CAutoIncKeys::keyValue_t>( key ) 
    {};
  ~ProcessFieldAutoIncKey( void ) {};

  void PreAppend( void ) {};
  void PreInsert( void ) {};
  void PreUpdate( void ) {std::cout << "ProcessFieldPk" << std::endl;};
  void PreDelete( void ) {};
  void PostFetch( void ) {};
protected:
private:
  ProcessFieldAutoIncKey(void);
};

// string key: index stored, but actual string used for key look up
class ProcessFieldSk: public ProcessFieldBase<ProcessFieldSk,CAutoIncKeys::keyValue_t> {
public:

  typedef std::string fldLocal_t;
  typedef CAutoIncKeys::keyValue_t fldStored_t;

  ProcessFieldSk( fldStored_t& key );
  ~ProcessFieldSk( void ) {};

  fldLocal_t const& constant_value( void ) const { return m_local; };
  fldLocal_t& value( void ) { return m_local; };

  fldLocal_t const& operator=( const std::string& rhs ) {
    m_local = rhs;
    m_stored = 0;
    return m_local;
  }

  void PreInsert( void ) {
    if ( 0 == m_local.length() ) {
      m_stored = 0;
    }
    else {
      m_stored = m_keyAutoInc.GetNextId();
      m_kvStrings.Set( m_stored, m_local );
    }
  };

  void PreAppend( void ) {};
  void PreUpdate( void ) {std::cout << "ProcessFieldSk" << std::endl;};
  void PreDelete( void ) {};
  void PostFetch( void ) {};

  void SetKey( void** data, u_int32_t& size ) {
    *data = static_cast<void*>( &m_local );
    size = m_local.length();
  }

  void SetKey( Dbt& key ) {
    if ( 0 == m_local.length() ) {
      throw std::runtime_error( "ProcessFieldSk SetKey length 0" );
    }
    key.set_data( static_cast<void*>( &m_local ) );
    key.set_size( m_local.length() );
    key.set_ulen( m_local.length() );
  }

protected:
private:
  fldLocal_t m_local;
  ProcessFieldSk(void);

  static CAutoIncKey m_keyAutoInc;  // used for getting a key for each new string
  static CKeyValuePairs m_kvStrings;  // used for storing the string indexed by the unique integer
};

// at some later point, need to perform validations against foreigh record
template <typename FK>
class ProcessFieldFk: public ProcessFieldBase<ProcessFieldFk<FK>,FK> {
public:
  ProcessFieldFk( FK& key ): ProcessFieldBase<ProcessFieldFk<FK>,FK>( key ) {};
  ~ProcessFieldFk( void ) {};

  void PreInsert( void ) {};
  void PreAppend( void ) {};
  void PreUpdate( void ) {std::cout << "ProcessFieldFk" << std::endl;};
  void PreDelete( void ) {};
  void PostFetch( void ) {};
protected:
private:
  ProcessFieldFk(void);
};

struct PreInsertRecordField {
  template <typename T>
  void operator()(T & x)  const  {
    x.PreInsert();
  }
};

struct PreAppendRecordField {
  template <typename T>
  void operator()(T & x)  const  {
    x.PreAppend();
  }
};

struct PreUpdateRecordField {
  template <typename T>
  void operator()(T & x)  const  {
    x.PreUpdate();
  }
};

struct PreDeleteRecordField {
  template <typename T>
  void operator()(T & x)  const  {
    x.PreDelete();
  }
};

struct PostFetchRecordField {
  template <typename T>
  void operator()(T & x)  const  {
    x.PostFetch();
  }
};