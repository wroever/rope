//
// Copyright Will Roever 2016 - 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "node.hpp"

namespace proj
{
  using std::make_unique;
  using std::pair;
  using handle = rope_node::handle;
  
  // Define out-of-bounds error constant
  std::invalid_argument ERROR_OOB_NODE = std::invalid_argument("Error: string index out of bounds");
  
  // Construct internal node by concatenating the given nodes
  rope_node::rope_node(handle l, handle r)
    : fragment_("")
  {
    this->left_ = move(l);
    this->right_ = move(r);
    this->weight_ = this->left_->getLength();
  }

  // Construct leaf node from the given string
  rope_node::rope_node(const std::string& str)
    : weight_(str.length()), left_(nullptr), right_(nullptr), fragment_(str)
  {}
  
  // Copy constructor
  rope_node::rope_node(const rope_node& aNode)
   : weight_(aNode.weight_), fragment_(aNode.fragment_)
  {
    rope_node * tmpLeft = aNode.left_.get();
    rope_node * tmpRight = aNode.right_.get();
    if(tmpLeft == nullptr) this->left_ = nullptr;
    else this->left_ = make_unique<rope_node>(*tmpLeft);
    if(tmpRight == nullptr) this->right_ = nullptr;
    else this->right_ = make_unique<rope_node>(*tmpRight);
  }
  
  // Determine whether a node is a leaf
  bool rope_node::isLeaf(void) const {
    return this->left_ == nullptr && this->right_ == nullptr;
  }
  
  // Get string length by adding the weight of the root and all nodes in
  //   path to rightmost child
  size_t rope_node::getLength() const {
    if(this->isLeaf())
      return this->weight_;
    size_t tmp = (this->right_ == nullptr) ? 0 : this->right_->getLength();
    return this->weight_ + tmp;
  }
  
  // Get the character at the given index
  char rope_node::getCharByIndex(size_t index) const {
    size_t w = this->weight_;
    // if node is a leaf, return the character at the specified index
    if (this->isLeaf()) {
      if (index >= this->weight_) {
        throw ERROR_OOB_NODE;
      } else {
        return this->fragment_[index];
      }
    // else search the appropriate child node
    } else {
      if (index < w) {
        return this->left_->getCharByIndex(index);
      } else {
        return this->right_->getCharByIndex(index - w);
      }
    }
  }

  // Get the substring of (len) chars beginning at index (start)
  string rope_node::getSubstring(size_t start, size_t len) const {
    size_t w = this->weight_;
    if (this->isLeaf()) {
      if(len < w) {
        return this->fragment_.substr(start,len);
      } else {
        return this->fragment_;
      }
    } else {
      // check if start index in left subtree
      if (start < w) {
        string lResult = (this->left_ == nullptr) ? "" : this->left_->getSubstring(start,len);
        if ((start + len) > w) {
          // get number of characters in left subtree
          size_t tmp =  w - start;
          string rResult = (this->right_ == nullptr) ? "" : this->right_->getSubstring(w,len-tmp);
          return lResult.append(rResult);
        } else {
          return lResult;
        }
      // if start index is in the right subtree...
      } else {
        return (this->right_ == nullptr) ? "" : this->right_->getSubstring(start-w,len);
      }
    }
  }
  
  // Get string contained in current node and its children
  string rope_node::treeToString(void) const {
    if(this->isLeaf()) {
      return this->fragment_;
    }
    string lResult = (this->left_ == nullptr) ? "" : this->left_->treeToString();
    string rResult = (this->right_ == nullptr) ? "" : this->right_->treeToString();
    return lResult.append(rResult);
  }
  
  // Split the represented string at the specified index
  pair<handle, handle> splitAt(handle node, size_t index)
  {
    size_t w = node->weight_;
    // if the given node is a leaf, split the leaf
    if(node->isLeaf()) {
      return pair<handle,handle>{
        make_unique<rope_node>(node->fragment_.substr(0,index)),
        make_unique<rope_node>(node->fragment_.substr(index,w-index))
      };
    }

    // if the given node is a concat (internal) node, compare index to weight and handle
    //   accordingly
    handle oldRight = move(node->right_);
    if (index < w) {
      node->right_ = nullptr;
      node->weight_ = index;
      std::pair<handle, handle> splitLeftResult = splitAt(move(node->left_), index);
      node->left_ = move(splitLeftResult.first);
      return pair<handle,handle>{
        move(node),
        make_unique<rope_node>(move(splitLeftResult.second), move(oldRight))
      };
    } else if (w < index) {
      pair<handle, handle> splitRightResult = splitAt(move(oldRight),index-w);
      node->right_ = move(splitRightResult.first);
      return pair<handle,handle>{
        move(node),
        move(splitRightResult.second)
      };
    } else {
      return pair<handle,handle>{
        move(node->left_), move(oldRight)
      };
    }
  }

  // Get the maximum depth of the rope, where the depth of a leaf is 0 and the
  //   depth of an internal node is 1 plus the max depth of its children
  size_t rope_node::getDepth(void) const {
    if(this->isLeaf()) return 0;
    size_t lResult = (this->left_ == nullptr) ? 0 : this->left_->getDepth();
    size_t rResult = (this->right_ == nullptr) ? 0 : this->right_->getDepth();
    return std::max(++lResult,++rResult);
  }
  
  // Store all leaves in the given vector
  void rope_node::getLeaves(std::vector<rope_node *>& v) {
    if(this->isLeaf()) {
      v.push_back(this);
    } else {
      rope_node * tmpLeft = this->left_.get();
      if (tmpLeft != nullptr) tmpLeft->getLeaves(v);
      rope_node * tmpRight = this->right_.get();
      if (tmpRight != nullptr) tmpRight->getLeaves(v);
    }
  }
  
} // namespace proj
