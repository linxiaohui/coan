/***************************************************************************
 *   Copyright (C) 2007-2013 Mike Kinghan, imk@burroingroingjoing.com      *
 *   All rights reserved.                                                  *
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

#include "file_tree.h"
#include "dataset.h"
#include "diagnostic.h"
#include "directory.h"
#include "syserr.h"
#include <cassert>

/** \file file_tree.cpp
 *   This file implements the class `file_tree`
 */
using namespace std;

void file_tree::node::insert(std::string const & key, node_ptr & child)
{
	if (!_children.get()) {
		_children.reset(new child_list);
	}
	child->_parent = this;
	_children->insert(entry(key,child));
}

file_tree::node * file_tree::node::root()
{
	file_tree::node * up = _parent;
	for (   ; up->_parent != nullptr; up = up->_parent) {}
	return up;
}

void file_tree::node::traverse(file_tree::traverser & action) const
{
	if (_children) {
        for (auto const & child : *_children) {
            action(child);
            child.second->traverse(action);
        }
	}
}

void file_tree::traverser::operator()(entry const & entry)
{
	while (_cur_path.posn() > 0 && entry.second->parent() != _cur_dir) {
		leave_dir(_cur_path.str());
		_cur_dir = _cur_dir->parent();
		_cur_path.pop_back();
	}
	if (entry.second->is_file()) {
		_cur_path.push_back(entry.first);
		_cur_path.to_end();
		at_file(_cur_path.str());
		_cur_path.pop_back();
	} else {
		_cur_dir = entry.second.get();
		_cur_path.push_back(entry.first);
		_cur_path.to_end();
		enter_dir(_cur_path.str());
	}
}

file_tree::node *
file_tree::node::ancestral_candidate_for_real_path(path_t const & cur_path,
        path_t const & real_path)
{
	path_t sub_path(cur_path);
	sub_path.posn() = sub_path.elements() - 1;
	sub_path.pop_back();
	node * ancestor = parent();
	for (	; ancestor &&
	        ancestor->parent() !=
	        ancestor->guardian(sub_path.cur_element());
	        sub_path.pop_back(),ancestor = ancestor->parent()) {
		if (sub_path.is_prefix_of(real_path)) {
			return ancestor;
		}
	}
	return nullptr;
}

template<typename Filter>
unsigned
file_tree::node::intermediate_insert(path_t & abs_path, Filter & filter)
{
    unsigned new_files = 0;
    std::string key = abs_path.cur_element();
    node * found = find(key);
    if (!found) {
        node_ptr candidate(new node(this));
        abs_path.posn() += (abs_path.posn() < int(abs_path.elements()));
        unsigned more_files =
            candidate->terminal_insert(abs_path,filter);
        if (more_files) {
            insert(key,candidate);
            new_files += more_files;
        }
    } else if (++abs_path.posn() < int(abs_path.elements())) {
        new_files += found->intermediate_insert(abs_path,filter);
    }
    return new_files;
}

template
unsigned
file_tree::node::intermediate_insert(
    path_t & abs_path, dataset::selector & filter);

template<typename Filter>
unsigned file_tree::node::terminal_insert(path_t & abs_path, Filter & filter)
{
    unsigned new_files = 0;
    if (abs_path.posn() < int(abs_path.elements())) {
        std::string key = abs_path.cur_element();
        node_ptr candidate(new node(this));
        ++abs_path.posn();
        unsigned more_files =
            candidate->terminal_insert(abs_path,filter);
        if (more_files) {
            new_files += more_files;
            insert(key,candidate);
        }
    } else {
        fs::obj_type_t obj_type = fs::obj_type(abs_path.str());
        if (fs::is_slink((obj_type))) {
            path_t real_path(fs::real_path(abs_path.str()));
            if (!ancestral_candidate_for_real_path(abs_path,real_path)) {
                root()->insert(real_path,filter);
            }
        } else if (fs::is_file(obj_type)) {
            new_files += filter(abs_path.str());
        } else if (fs::is_dir(obj_type)) {
            directory dir(abs_path.str());
            for (std::string entry;
                 dir && (!(entry = dir.next()).empty()); ) {
                abs_path.push_back(entry);
                abs_path.to_end();
                new_files += terminal_insert(abs_path,filter);
                abs_path.pop_back();
            }
            if (!dir) {
                if (!dir.open()) {
                    abend_cant_open_dir() <<
                          "Can't open directory \"" << abs_path.str() <<
                          "\" for reading: " <<
                          system_error_message(dir.last_error())
                          << emit();
                } else {
                    abend_cant_read_dir() <<
                      "Read error on directory \"" << abs_path.str() <<
                      "\": " <<
                      system_error_message(dir.last_error()) << emit();
                }
            }
        } else {
            assert(false);
        }
    }
    return new_files;
}


#ifdef FILETREE_DEBUG
#include <iostream>
void file_tree::node::display(std::string const & name,
                              node const * n, unsigned indent)
{
	cout << string(indent,' ') << "node: " << name << endl;
	if (n->_children) {
		child_list::const_iterator start = n->_children->begin();
		child_list::const_iterator end = n->_children->end();
		for (   ; start != end; ++start) {
			display(start->first,start->second.get(),indent + 1);
		}
	}
}
#endif


// EOF
