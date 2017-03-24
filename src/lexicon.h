#ifndef LEXICON_H
#define LEXICON_H
#pragma once
/***************************************************************************
 *   Copyright (C) 2007-2013 Mike Kinghan, imk@burroingroingjoing.com      *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   Contributed originally by Mike Kinghan, imk@burroingroingjoing.com    *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *                                                                         *
 *   Redistributions of source code must retain the above copyright        *
 *   notice, this list of conditions and the following disclaimer.         *
 *                                                                         *
 *   Redistributions in binary form must reproduce the above copyright     *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *                                                                         *
 *   Neither the name of Mike Kinghan nor the names of its contributors    *
 *   may be used to endorse or promote products derived from this software *
 *   without specific prior written permission.                            *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS    *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,   *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS *
 *   OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED    *
 *   AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,*
 *   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF *
 *   THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH  *
 *   DAMAGE.                                                               *
 *                                                                         *
 **************************************************************************/
#include "platform.h"

/** \file lexicon.h
 *   This file defines symbolic constants for some preprocessor keywords.
 */

/// Constant denoting the if directive
#define TOK_IF          "if"
/// Constant denoting the `#ifdef` directive
#define TOK_IFDEF       "ifdef"
/// Constant denoting the `#ifndef` directive
#define TOK_IFNDEF      "ifndef"
/// Constant denoting the `#else` directive
#define TOK_ELSE        "else"
/// Constant denoting the `#elif` directive
#define TOK_ELIF        "elif"
/// Constant denoting the `#endif` directive
#define TOK_ENDIF       "endif"
/// Constant denoting the `#define` directive
#define TOK_DEFINE      "define"
/// Constant denoting the `#undef` directive
#define TOK_UNDEF       "undef"
/// Constant denoting the `#include` directive
#define TOK_INCLUDE     "include"
/// Constant denoting the `#pragma` directive
#define TOK_PRAGMA      "pragma"
/// Constant denoting the `line` directive
#define TOK_LINE     "line"
/// Constant denoting the `#error` directive
#define TOK_ERROR       "error"
/// Constant denoting the `defined` operator
#define TOK_DEFINED     "defined"
/// Constant denoting the alternative boolean negation operator
#define TOK_ALT_BOOLEAN_NOT "not"
/// Constant denoting the alternative bitwise negation operator
#define TOK_ALT_BIT_NOT 	"compl"

#endif // EOF
