#ifndef FILE_TREE_H
#define FILE_TREE_H
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
#include "prohibit.h"
#include "path.h"
#include "filesys.h"
#include <memory>
#include <map>

/** \file file_tree.h
 *   This file defines `struct file_tree`.
 */

/*! \brief Encapsulates a set of directory/file trees.

    `struct file_tree` encapsulates a set of directory/file trees,
    notionally united at a root node that does not represent any
    file-system object, but just affords entry to the set.
*/
struct file_tree : private no_copy
{
	struct traverser;

	/** \brief Type of a node in a `file_tree`.
     *
	 *   `struct node` is the internal container type that implements
	 *   `file_tree`. A `node` consists of a pointer to its parent
	 *   node and a pointer to a map containing its children, keyed
	 *   by filename.
	 */
	struct node {
		/// Type of a pointer to a `node`.
		using node_ptr = std::shared_ptr<node>;
		/// Type of sequence of children of a `node`.
		using child_list = std::map<std::string,node_ptr>;
		/// Type of an element in a `child_list`.
		using entry = child_list::value_type;
		/// Type of a pointer to a `child_list`
		using child_list_ptr = std::shared_ptr<child_list>;

		friend struct file_tree;

		/// Say whether the `node` represents a file
		bool is_file() const {
			return _children.get() == nullptr;
		}

		/// Say whether the `node` represents a directory
		bool is_dir() const {
			return !is_file();
		}

		/** \brief Traverse the `node` recursively, performing an action at
		 *   each node encountered.
         *
		 *  \param action The action to be applied recursively to nodes.
		 */
		void traverse(traverser & action) const;

		/// Get a pointer to the root node of the `node`.
		node * root();

		/** \brief Recursively insert files within a path into the `node`.
         *
		 *   Recursively insert files within a path into the `node`,
		 *   selecting eligible files by a filter.
         *
		 *   \param  abs_path  The absolute path within which files
		 *       are to be added to the node.
		 *   \param  filter  The filter for selecting eligible files.
		 *   \return The number of files inserted.
         */
		template<typename Filter>
		unsigned insert(path_t & abs_path, Filter & filter) {
			return intermediate_insert(abs_path,filter);
		}

	private:

		/// Private default constructor
		explicit node(node * parent = nullptr)
			: _parent(parent) {}

		/// Explicitly construct a node with a given `child_list`
		explicit node(child_list_ptr children)
		: _parent(nullptr),_children(children) {}

		/** \brief Test whether the parent of this `node` contains a given key.
         *
		 *   \param  key The key to be sought.
		 *   \return A pointer to the parent of this `node` if it exists
		 *       and has a child with key `key`; otherwise `nullptr`.
		 */
		node * guardian(std::string const & key) const {
			return _parent && _parent->find(key) ? _parent : nullptr;
		}

		/// Get a pointer to the parent of this `node`, which may be `nullptr`
		node * parent() const {
			return _parent;
		}

		/** \brief Test whether this `node` contains a given key.
		 *   \param key  The key to be sought.
		 *   \return A pointer to the child of this `node` that has key `key`,
		 *       else `nullptr`.
		 */
		node * find(std::string const & key) {
			node * hit = nullptr;
			if (_children) {
				auto where = _children->find(key);
				if (where != _children->end()) {
					hit = where->second.get();
				}
			}
			return hit;
		}

		/** \brief Ancestor probe for symbolic links.
         *
		 *   Find the nearest ancestor of the node, if any, that is:-
		 *	- an unconfirmed candidate for inclusion in the `file_tree`
		 *	- is keyed by a path that is an initial subpath of the path
		 *	that keys `this` and also of the real path to which the
		 *	path that keys `this` is resolved.
         *
		 *	\param 	cur_path The path that keys `this`.
		 *	\param	real_path	The real path to which `cur_path` resolves.
         *
		 *	\return Pointer to the nearest ancestor of `*this`
		 *	that satisfies the two conditions, if any, else nullptr.
         *
		 *	This conceptually complicated method is called only
		 *	when the path that keys `*this` refers to a symbolic link.
		 *	In this case the obvious course is just to take the real path
		 *	to which the symbolic link resolves, insert it "from scratch" at
		 *	the root of the file tree and carry on with the current traversal.
		 *	But this may be a mistake.
         *
		 *	Since all top-level input paths are resolved to real paths, a
		 *	symbolic link will only be encountered while we are recursively
		 *	exploring some directory to determine whether any files are selected
		 *	beneath it, and therefore whether the candidate node that represents
		 *	it should actually be inserted in the `file_tree`. While we are
		 *	doing this, and when we encounter the symbolic link, that candidate
		 *	node has yet to be inserted. Hence, if the real path to the symbolic
		 *	link extends the path that keys our candidate node, the path
		 *	elements that key the nodes from the candidate downward will not
		 *	be already present in the tree and all eligible nodes beneath
		 *	the real path will be inserted with their keys. But some initial
		 *	sequence of the same keys have already been confirmed not already
		 *	present in the descent that lead us to the symbolic link in the
		 *	first place. So in handling the symbolic link, we will prematurely
		 *	generate a sub-tree that is identical with one we are in the
		 *	process of generating, and which is to be inserted in the same
		 *	place in the `file_tree`, if at all.
         *
		 *	This could be an arbitrarily great waste of time, even though the
		 *	implementation of the `file_tree` inherantly prevents duplicate
		 *	 entries being created by duplicate insertions. In fact it
		 *	could be an infinite waste of time: if the symbolic link is
		 *	 "self-including" - i.e. refers to one of its own ancestral
		 *	directories - then the simple "start from scratch" approach will
		 *	loop forever. To avoid these hazards, when we encounter a symbolic
		 *	link, we wish to know whether the real path it resolves to either
		 *	is or extends any path that we are already recursively traversing to
		 *	decide upon the insertion of some ancestral node. Only if that is
		 *	*not* the case need we bother to explore the symbolic link. This
		 *	method gives us the answer.
		 */
		node * ancestral_candidate_for_real_path(path_t const & cur_path,
		        path_t const & real_path);

		/** \brief Recursively insert files within a path into the `node`.
         *
		 *  Recursively insert files within a path into the `node`,
		 *  selecting eligible files by a filter.
         *
         * \param  abs_path  The absolute path within which files
		 *      are to be added to the node.
		 *  \param  filter The filter for selecting eleigible files.
         *
		 *  This member function implements the public member `insert`.
		 *  It calls itself recursively for as long as successive elements of
		 *  `abs_path` are already represented in the `node` and finally calls
		 *  `insert_terminal` to insert files from the terminal remainder of
		 *  `abs_path`.
         *
		 *  \return The number of files inserted.
		 */
		template<typename Filter>
		unsigned intermediate_insert(path_t & abs_path, Filter & filter);

		/** \brief Recursively insert files within a path into the `node`.
         *
		 *  Recursively insert files within a terminal segment of a path into
		 *  the `node`, selecting eligible files by a filter.
         *
		 *  \param  abs_path    The absolute path within which files
		 *       are to be added to the node.
		 *  \param  filter  The filter for selecting eleigible files.
		 *   \return The number of files inserted.
		 *
		 *  `abs_path` is assumed to be positioned at the first element, if
		 *  any, from which files have not already been inserted to
		 *  the `node`.
         *
		 *  This member function is called by `insert_intermediate` when that
		 *  function has exhausted the elements of the path that are
         *   already represented in the `node`.

		 */
		template<typename Filter>
		unsigned terminal_insert(path_t & abs_path, Filter & filter);

		/** \brief Insert a child to this `node` with a given key.
         *
		 *  \param  key The key of the child node to be inserted
		 *  \param  child The child node to be inserted.
		 */
		void insert(std::string const & key, node_ptr & child);

#ifdef FILETREE_DEBUG
		static void display(std::string const & name,
		                    node const * n, unsigned indent = 0);
#endif
		/// Pointer to the parent `node`, or `nullptr`
		node *_parent;
		/// Pointer to the immediate children of the `node`, or `nullptr`
		std::shared_ptr<child_list> _children;
	};

public:

	/// Type of a sequence of sibling nodes in a `file_tree`
	using child_list = node::child_list;
	/// Type of a pointer to a `node`
	using node_ptr = node::node_ptr;
	/// Type of an element in a `child_list`.
	using entry = node::entry;

	/** \brief A base for classes employed to traverse a `file_tree`.
     *
	 *  A `traverser` encapsulates the actions to be taken at the
	 *  junctures of a `file_tree` traversal:-
	 *  - At entering directory
	 *  - At a file
	 *  - At leaving a directory.
	 */
	struct traverser : private no_copy {

		traverser() = default;

		//! Destructor
		virtual ~traverser() = default;

		/** \brief Apply the `traverser` function-wise to an element of a
		 *   `file_tree`.
		 *   \param  entry  The element to which the traverser shall be
		 *       applied.
		 */
		void operator()(entry const & entry);

		/** \brief Do something (or nohting) on entering a directory.
		 *   \param  dirname The absolute name of the directory.
		 */
		virtual void enter_dir(std::string const & dirname) {};
		/** \brief Do something (or nothing) on reaching a file.
		 *   \param  filename The absolute name of the file.
		 */
		virtual void at_file(std::string const & filename) {};
		/** \brief Do something (or nohting) on leaving a directory.
		 *   \param  dirname The absolute name of the directory.
		 */
		virtual void leave_dir(std::string const & dirname) {};

	protected:

		/** \brief Pointer the node representing the latest directory that
		 *   traversal has reached.
		 */
		node const * _cur_dir = nullptr;
		/// The absolute path of the object that traversal has reached.
		path_t _cur_path;
	};

	/// Type representing the null filter for selecting files.
	struct no_filter {
		/// The null filter returns true for every filename.
		bool operator ()(std::string const &) const {
			return true;
		}
	};

	/// Default constructor
	file_tree()
		: _root(node::child_list_ptr(new child_list)) {}

	/// Get the number of files in the `file_tree`
	unsigned files() const {
		return _files;
	}

	/** \brief Traverse the `file_tree`.
	 *   \param action A `traverser` to be applied to each node in the
	 *   `file_tree`.
	 */
	void traverse(traverser & action) {
		_root.traverse(action);
	}

	/** \brief Add files from a path to the `file_tree`.
	 *  \param  path    The path from files are to be added.
	 *  \param  filter An optional filter for selecting the files to be
	 *      added.
     *
	 *  Files within `path` that satisfy `filter` are recursively added to
	 *  the `file_tree`. `filter` defaults to `no_filter`, which is satisfied
	 *  by any file.
	 */
	template<typename Filter = file_tree::no_filter>
	void add(std::string const & path, Filter & filter) {
		path_t abs_path(fs::real_path(path));
		_root.insert(abs_path,filter);
#ifdef FILETREE_DEBUG
		node::display("ROOT",&_root,0);
#endif
	}

	/** \brief Add files from a path to the `file_tree`.
	 *  \tparam InIter  An input iterator type.
     *
	 *  \param  start  An iterator to the start of a seqence of file or
	 *           directory names.
	 *  \param  end     The end of the sequence commencing at `start`.
	 *  \param  filter An optional filter for selecting the files to be
	 *       added.
     *
	 *  All selected files within the members of the argument sequence are
	 *  added to the `file_tree`.
     *
	 */
	template<class InIter, class Filter = file_tree::no_filter>
	void add(InIter start, InIter end, Filter & filter) {
		for (   ; start != end; add(*start++,filter)) {}
	}

private:

	/** \brief The root node of the `file_tree`.
     *
	 *  The root node does not represent any filesystem object; it simply
	 *  unites all the top-level filesystem objects that are represented.
	 */
	node _root;
	/** \brief The number of files in the `file_tree`.
     *
	 *   This data member is updated by adding files to the tree so that
	 *   the population of the tree may be queried without calculation.
	 */
	unsigned _files = 0;

};

#endif // EOF
