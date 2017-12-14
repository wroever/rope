//
// Copyright Will Roever 2016 - 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "rope.hpp"

namespace proj
{
  using std::make_unique;
  using std::pair;
  
  // out-of-bounds error constant
  std::invalid_argument ERROR_OOB_ROPE = std::invalid_argument("Error: string index out of bounds");

  // Default constructor - produces a rope representing the empty string
  rope::rope(void) : rope("")
  {}
  
  // Construct a rope from the given string
  rope::rope(const string& str) {
    this->root_ = make_unique<rope_node>(str);
  }
  
  // Copy constructor
  rope::rope(const rope& r) {
    rope_node newRoot = rope_node(*r.root_);
    this->root_ = make_unique<rope_node>(newRoot);
  }
  
  // Get the string stored in the rope
  string rope::toString(void) const {
    if(this->root_ == nullptr)
      return "";
    return this->root_->treeToString();
  }
  
  // Get the length of the stored string
  size_t rope::length(void) const {
    if(this->root_ == nullptr)
      return 0;
    return this->root_->getLength();
  }
  
  // Get the character at the given position in the represented string
  char rope::at(size_t index) const {
    if(this->root_ == nullptr)
      throw ERROR_OOB_ROPE;
    return this->root_->getCharByIndex(index);
  }

  // Return the substring of length (len) beginning at the specified index
  string rope::substring(size_t start, size_t len) const {
    size_t actualLength = this->length();
    if (start > actualLength || (start+len) > actualLength) throw ERROR_OOB_ROPE;
    return this->root_->getSubstring(start, len);
  }

  // Insert the given string into the rope, beginning at the specified index (i)
  void rope::insert(size_t i, const string& str) {
    this->insert(i,rope(str));
  }

  // Insert the given rope into the rope, beginning at the specified index (i)
  void rope::insert(size_t i, const rope& r) {
    if (this->length() < i) {
      throw ERROR_OOB_ROPE;
    } else {
      rope tmp = rope(r);
      pair<handle, handle> origRopeSplit = splitAt(move(this->root_),i);
      handle tmpConcat = make_unique<rope_node>(move(origRopeSplit.first), move(tmp.root_));
      this->root_ = make_unique<rope_node>(move(tmpConcat), move(origRopeSplit.second));
    }
  }
  
  // Append the argument to the existing rope
  void rope::append(const string& str) {
    rope tmp = rope(str);
    this->root_ = make_unique<rope_node>(move(this->root_), move(tmp.root_));
  }

  // Append the argument to the existing rope
  void rope::append(const rope& r) {
    rope tmp = rope(r);
    this->root_ = make_unique<rope_node>(move(this->root_), move(tmp.root_));
  }
  
  // Delete the substring of (len) characters beginning at index (start)
  void rope::rdelete(size_t start, size_t len) {
    size_t actualLength = this->length();
    if (start > actualLength || start+len > actualLength) {
      throw ERROR_OOB_ROPE;
    } else {
      pair<handle, handle> firstSplit = splitAt(move(this->root_),start);
      pair<handle, handle> secondSplit = splitAt(move(firstSplit.second),len);
      secondSplit.first.reset();
      this->root_ = make_unique<rope_node>(move(firstSplit.first), move(secondSplit.second));
    }
  }
  
  // Determine if rope is balanced
  //
  // A rope is balanced if and only if its length is greater than or equal to
  //   fib(d+2) where d refers to the depth of the rope and fib(n) represents
  //   the nth fibonacci number i.e. in the set {1,1,2,3,5,8,etc...}
  bool rope::isBalanced(void) const{
    if(this->root_ == nullptr)
      return true;
    size_t d = this->root_->getDepth();
    return this->length() >= fib(d+2);
  }
  
  // Balance a rope
  void rope::balance(void) {
    // initiate rebalancing only if rope is unbalanced
    if(!this->isBalanced()) {
      // build vector representation of Fibonacci intervals
      std::vector<size_t> intervals = buildFibList(this->length());
      std::vector<handle> nodes(intervals.size());
      
      // get leaf nodes
      std::vector<rope_node *> leaves;
      this->root_->getLeaves(leaves);
      
      size_t i;
      size_t max_i = intervals.size()-1;
      size_t currMaxInterval = 0;
      handle acc = nullptr;
      handle tmp = nullptr;

      // attempt to insert each leaf into nodes vector based on length
      for (auto& leaf : leaves) {
        size_t len = leaf->getLength();
        bool inserted = false;
        
        // ignore empty leaf nodes
        if (len > 0) {
          acc = make_unique<rope_node>(*leaf);
          i = 0;

          while(!inserted) {
            // find appropriate slot for the acc node to be inserted,
            // concatenating with nodes encountered along the way
            while(i < max_i && len >= intervals[i+1]) {
              if(nodes[i].get() != nullptr) {
                // concatenate encountered entries with node to be inserted
                tmp = make_unique<rope_node>(*nodes[i].get());
                acc = make_unique<rope_node>(*acc.get());
                acc = make_unique<rope_node>(move(tmp),move(acc));
                
                // update len
                len = acc->getLength();
                
                // if new length is sufficiently great that the node must be
                //   moved to a new slot, we clear out the existing entry
                if(len >= intervals[i+1]) nodes[i] = nullptr;
              }
              i++;
            }
            
            // target slot found -- check if occupied
            if (nodes[i].get() == nullptr) {
              nodes[i].swap(acc);
              inserted = true;
              // update currMaxInterval if necessary
              if(i > currMaxInterval) currMaxInterval = i;
            } else {
              // concatenate encountered entries with node to be inserted
              tmp = make_unique<rope_node>(*nodes[i].get());
              acc = make_unique<rope_node>(*acc.get());
              acc = make_unique<rope_node>(move(tmp),move(acc));
              
              // update len
              len = acc->getLength();
              
              // if new length is sufficiently great that the node must be
              //   moved to a new slot, we clear out the existing entry
              if(len >= intervals[i+1]) nodes[i] = nullptr;
            }
          }
        }
      }
      
      // concatenate remaining entries to produce balanced rope
      acc = move(nodes[currMaxInterval]);
      for(int idx = currMaxInterval; idx >= 0; idx--) {
        if(nodes[idx] != nullptr) {
          tmp = make_unique<rope_node>(*nodes[idx].get());
          acc = make_unique<rope_node>(move(acc),move(tmp));
        }
      }
      this->root_ = move(acc); // reset root
      
    }
  }
  
  // Assignment operator
  rope& rope::operator=(const rope& rhs) {
    // check for self-assignment
    if(this == &rhs) return *this;
    // delete existing rope to recover memory
    this->root_.reset();
    // invoke copy constructor
    this->root_ = make_unique<rope_node>(*(rhs.root_.get()));
    return *this;
  }
  
  // Determine if two ropes contain identical strings
  bool rope::operator ==(const rope& rhs) const {
    return this->toString() == rhs.toString();
  }
  
  // Determine if two ropes contain identical strings
  bool rope::operator !=(const rope& rhs) const {
    return !(*this == rhs);
  }
  
  // Print the rope
  std::ostream& operator<<(std::ostream& out, const rope& r) {
    return out << r.toString();
  }
  
  
  // Compute the nth Fibonacci number, in O(n) time
  size_t fib(size_t n) {
    // initialize first two numbers in sequence
    int a = 0, b = 1, next;
    if(n == 0) return a;
    for (size_t i = 2; i <= n; i++) {
      next = a + b;
      a = b;
      b = next;
    }
    return b;
  };
  
  // Construct a vector where the nth entry represents the interval between the
  //   Fibonacci numbers F(n+2) and F(n+3), and the final entry includes the number
  //   specified in the length parameter
  // e.g. buildFibList(0) -> {}
  //      buildFibList(1) -> {[1,2)}
  //      buildFibList(8) -> {[1,2),[2,3),[3,5),[5,8),[8,13)}
  std::vector<size_t> buildFibList(size_t len) {
    // initialize a and b to the first and second fib numbers respectively
    int a = 0, b = 1, next;
    std::vector<size_t> intervals = std::vector<size_t>();
    while (a <= len) {
      if (a > 0) {
        intervals.push_back(b);
      }
      next = a + b;
      a = b;
      b = next;
    }
    return intervals;
  }
  
}
