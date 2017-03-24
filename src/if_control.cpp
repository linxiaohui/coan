/***************************************************************************
 *   Copyright (C) 2004, 2006 Symbian Software Ltd.                        *
 *   All rights reserved.                                                  *
 *   Copyright (C) 2002, 2003 Tony Finch <dot@dotat.at>.                   *
 *   All rights reserved.                                                  *
 *   Copyright (C) 1985, 1993 The Regents of the University of California. *
 *   All rights reserved.                                                  *
 *   Copyright (C) 2007-2013 Mike Kinghan, imk@burroingroingjoing.com      *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   Contributed by Mike Kinghan, imk@burroingroingjoing.com,              *
 *   derived from the code of Tony Finch                                   *
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
 *   Neither the name of Symbian Software Ltd. nor the names of its        *
 *   contributors may be used to endorse or promote products derived from  *
 *   this software without specific prior written permission.              *
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

#include "if_control.h"
#include "diagnostic.h"
#include "line_despatch.h"
#include "chew.h"
#include "io.h"

/**	\file if_control.cpp
 *   This file implements `struct if_control`
 */
if_control::if_state	if_control::ifstate[MAXDEPTH] = {if_state(0)};
size_t		if_control::depth = 0;
size_t		if_control::if_start_lines[MAXDEPTH] = {0};

void if_control::Strue()
{
	line_despatch::drop();
	set_state(IF_STATE_TRUE_PREFIX);
}

void if_control::Sfalse()
{
	line_despatch::drop();
	set_state(IF_STATE_FALSE_PREFIX);
}

void if_control::Selse()
{
	line_despatch::drop();
	set_state(IF_STATE_TRUE_ELSE);
}

void if_control::Pelif()
{
	line_despatch::print();
	set_state(IF_STATE_PASS_MIDDLE);
}

void if_control::Pelse()
{
	line_despatch::print();
	set_state(IF_STATE_PASS_ELSE);
}

void if_control::Pendif()
{
	line_despatch::print();
	--depth;
}

void if_control::Dfalse()
{
	line_despatch::drop();
	set_state(IF_STATE_FALSE_TRAILER);
}

void if_control::Delif()
{
	line_despatch::drop();
	set_state(IF_STATE_FALSE_MIDDLE);
}

void if_control::Delse()
{
	line_despatch::drop();
	set_state(IF_STATE_FALSE_ELSE);
}

void if_control::Dendif()
{
	line_despatch::drop();
	--depth;
}

void if_control::Mpass()
{
	line_despatch::cur_line().keyword_edit<HASH_IF>();
	line_despatch::print();
	set_state(IF_STATE_PASS_MIDDLE);
}

void if_control::Mtrue()
{
	line_despatch::cur_line().keyword_edit<HASH_ELSE>();
	line_despatch::print();
	set_state(IF_STATE_TRUE_MIDDLE);
}

void if_control::Melif()
{
	line_despatch::cur_line().keyword_edit<HASH_ENDIF>();
	line_despatch::print();
	set_state(IF_STATE_FALSE_TRAILER);
}

void if_control::Melse()
{
	line_despatch::cur_line().keyword_edit<HASH_ENDIF>();
	line_despatch::print();
	set_state(IF_STATE_FALSE_ELSE);
}

void if_control::orphan_elif()
{
	error_orphan_elif() << "Orphan #elif" << emit();
}

void if_control::orphan_else()
{
	error_orphan_else() << "Orphan #else" << emit();
}

void if_control::orphan_endif()
{
	error_orphan_endif() << "Orphan #endif" << emit();
}

void if_control::early_eof()
{
	error_eof_too_soon() << "Unexpected end of file" << emit();
}


if_control::transition_t * const
if_control::transition_table[IF_STATE_COUNT][LT_SENTINEL] = {
	/* IF_STATE_OUTSIDE*/
	{
		Fpass,
		Ftrue,
		Ffalse,
		orphan_elif,
		orphan_elif,
		orphan_elif,
		orphan_else,
		orphan_endif,
		line_despatch::print,
		diagnostic_base::flush_all
	},
	/* IF_STATE_FALSE_PREFIX*/
	{
		Fdrop,
		Fdrop,
		Fdrop,
		Mpass,
		Strue,
		Sfalse,
		Selse,
		Dendif,
		line_despatch::drop,
		early_eof
	},
	/* IF_STATE_TRUE_PREFIX*/
	{
		Fpass,
		Ftrue,
		Ffalse,
		Dfalse,
		Dfalse,
		Dfalse,
		Delse,
		Dendif,
		line_despatch::print,
		early_eof
	},
	/* IF_STATE_PASS_MIDDLE*/
	{
		Fpass,
		Ftrue,
		Ffalse,
		Pelif,
		Mtrue,
		Delif,
		Pelse,
		Pendif,
		line_despatch::print,
		early_eof
	},
	/* IF_STATE_FALSE_MIDDLE*/
	{
		Fdrop,
		Fdrop,
		Fdrop,
		Pelif,
		Mtrue,
		Delif,
		Pelse,
		Pendif,
		line_despatch::drop,
		early_eof
	},
	/* IF_STATE_TRUE_MIDDLE*/
	{
		Fpass,
		Ftrue,
		Ffalse,
		Melif,
		Melif,
		Melif,
		Melse,
		Pendif,
		line_despatch::print,
		early_eof
	},
	/* IF_STATE_PASS_ELSE*/
	{
		Fpass,
		Ftrue,
		Ffalse,
		orphan_elif,
		orphan_elif,
		orphan_elif,
		orphan_else,
		Pendif,
		line_despatch::print,
		early_eof
	},
	/* IF_STATE_FALSE_ELSE*/
	{
		Fdrop,
		Fdrop,
		Fdrop,
		orphan_elif,
		orphan_elif,
		orphan_elif,
		orphan_else,
		Dendif,
		line_despatch::drop,
		early_eof
	},
	/* IF_STATE_TRUE_ELSE*/
	{
		Fpass,
		Ftrue,
		Ffalse,
		orphan_elif,
		orphan_elif,
		orphan_elif,
		orphan_else,
		Dendif,
		line_despatch::print,
		early_eof
	},
	/* IF_STATE_FALSE_TRAILER*/
	{
		Fdrop,
		Fdrop,
		Fdrop,
		Dfalse,
		Dfalse,
		Dfalse,
		Delse,
		Dendif,
		line_despatch::drop,
		early_eof
	}
	/*IF     TRUE   FALSE  ELIF   ELTRUE ELFALSE ELSE  ENDIF  PLAIN  EOF*/
};

void if_control::nest()
{
	size_t deep = ++depth;
	if (deep >= MAXDEPTH) {
		error_too_deep() << "Too many levels of nesting" << emit();
	}
	if_start_lines[deep] = line_despatch::cur_line().num();
}

void if_control::transition(line_type linetype)
{
	if_state state = ifstate[if_depth()];
	transition_table[state][linetype]();
}

bool if_control::dead_line()
{
	if_state state =
	    ifstate[if_depth()];
	return state == IF_STATE_FALSE_PREFIX ||
	       state == IF_STATE_FALSE_MIDDLE ||
	       state == IF_STATE_FALSE_ELSE ||
	       state == IF_STATE_FALSE_TRAILER;
}

bool if_control::is_unconditional_line()
{
	if_state state =
	    ifstate[if_depth()];
	return	state == IF_STATE_OUTSIDE ||
	        state == IF_STATE_TRUE_PREFIX ||
	        state == IF_STATE_TRUE_MIDDLE ||
	        state == IF_STATE_TRUE_ELSE ;
}

/* EOF*/
