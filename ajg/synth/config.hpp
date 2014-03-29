//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_CONFIG_HPP_INCLUDED
#define AJG_SYNTH_CONFIG_HPP_INCLUDED

#include <ajg/synth/version.hpp>

#ifndef AJG_SYNTH_SEQUENCE_LIMIT
#define AJG_SYNTH_SEQUENCE_LIMIT 40
#endif

#ifndef AJG_SYNTH_CONSTRUCT_LIMIT
#define AJG_SYNTH_CONSTRUCT_LIMIT 10
#endif

#if AJG_SYNTH_SEQUENCE_LIMIT > 50

#    define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#    define BOOST_FUSION_DONT_USE_PREPROCESSED_FILES

#endif

// #define BOOST_MPL_LIMIT_VECTOR_SIZE AJG_SYNTH_SEQUENCE_LIMIT
#define FUSION_MAX_MAP_SIZE            AJG_SYNTH_SEQUENCE_LIMIT
#define FUSION_MAX_VECTOR_SIZE         AJG_SYNTH_SEQUENCE_LIMIT

#ifndef NDEBUG

#ifndef AJG_SYNTH_DEBUG_TRACE_FRAME_LIMIT
#define AJG_SYNTH_DEBUG_TRACE_FRAME_LIMIT 1024
#endif

#if defined(__clang__) || defined(__GNUC__)
#    ifndef HAS_CXXABI_H
#    define HAS_CXXABI_H 1
#    endif
#    ifndef HAS_EXECINFO_H
#    define HAS_EXECINFO_H 1
#    endif
#else
#    ifndef HAS_CXXABI_H
#    define HAS_CXXABI_H 0
#    endif
#    ifndef HAS_EXECINFO_H
#    define HAS_EXECINFO_H 0
#    endif
#endif

// #define BOOST_THROW_EXCEPTION_CURRENT_FUNCTION

#include <ajg/synth/debug.hpp>

#endif // NDEBUG

#endif // AJG_SYNTH_CONFIG_HPP_INCLUDED
