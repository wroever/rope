//
// Copyright Will Roever 2016 - 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <algorithm>
#include "node.hpp"

namespace proj
{
  using std::string;
  
  // A rope represents a string as a binary tree wherein the leaves contain fragments of the
  //   string. More accurately, a rope consists of a pointer to a root rope_node, which
  //   describes a binary tree of string fragments.
  
  // Examples:
  //
  //        X        |  null root pointer, represents empty string
  //
  //      "txt"      |
  //     /     \     |  root is a leaf node containing a string fragment
  //    X       X    |
  //
  //        O        |
  //       / \       |
  //  "some" "text"  |  root is an internal node formed by the concatenation of two distinct
  //    /\     /\    |  ropes containing the strings "some" and "text"
  //   X  X   X  X   |
  
  class rope {
    
  public:
    
    using handle = std::unique_ptr<rope_node>;
    
    // CONSTRUCTORS
    // Default constructor - produces a rope representing the empty string
    rope(void);
    // Construct a rope from the given string
    rope(const string&);
    // Copy constructor
    rope(const rope&);
    
    // Get the string stored in the rope
    string toString(void) const;
    // Get the length of the stored string
    size_t length(void) const;
    // Get the character at the given position in the represented string
    char at(size_t index) const;
    // Return the substring of length (len) beginning at the specified index
    string substring(size_t start, size_t len) const;
    // Determine if rope is balanced
    bool isBalanced(void) const;
    // Balance the rope
    void balance(void);
    
    // MUTATORS
    // Insert the given string/rope into the rope, beginning at the specified index (i)
    void insert(size_t i, const string& str);
    void insert(size_t i, const rope& r);
    // Concatenate the existing string/rope with the argument
    void append(const string&);
    void append(const rope&);
    // Delete the substring of (len) characters beginning at index (start)
    void rdelete(size_t start, size_t len);
    
    // OPERATORS
    rope& operator=(const rope& rhs);
    bool operator==(const rope& rhs) const;
    bool operator!=(const rope& rhs) const;
    friend std::ostream& operator<<(std::ostream& out, const rope& r);
    
  private:
    
    // Pointer to the root of the rope tree
    handle root_;
    
  }; // class rope
  
  size_t fib(size_t n);
  std::vector<size_t> buildFibList(size_t len);
  
} // namespace proj
