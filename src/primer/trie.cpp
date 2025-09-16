//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// trie.cpp
//
// Identification: src/primer/trie.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "primer/trie.h"
#include <map>
#include <memory>
#include <string_view>
#include <utility>
#include "common/exception.h"

namespace bustub {

/**
 * @brief Get the value associated with the given key.
 * 1. If the key is not in the trie, return nullptr.
 * 2. If the key is in the trie but the type is mismatched, return nullptr.
 * 3. Otherwise, return the value.
 */
template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  //throw NotImplementedException("Trie::Get is not implemented.");

  // You should walk through the trie to find the node corresponding to the key. If the node doesn't exist, return
  // nullptr. After you find the node, you should use `dynamic_cast` to cast it to `const TrieNodeWithValue<T> *`. If
  // dynamic_cast returns `nullptr`, it means the type of the value is mismatched, and you should return nullptr.
  // Otherwise, return the value.
  auto currentNode = this->root_;
  for(char ch: key){
    if(currentNode == nullptr) {
      return nullptr;
    }
    auto findnode = currentNode->children_.find(ch);
    if(findnode == currentNode->children_.end()){
      return nullptr;
    }else {
      currentNode = findnode->second;
    }
  }
  auto *value_node = dynamic_cast<const TrieNodeWithValue<T> *>(currentNode.get());
  if (value_node == nullptr) return nullptr;

  return value_node->value_.get();
}

/**
 * @brief Put a new key-value pair into the trie. If the key already exists, overwrite the value.
 * @return the new trie.
 */
template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie {
  // Note that `T` might be a non-copyable type. Always use `std::move` when creating `shared_ptr` on that value.
  auto currentNode = this->root_;
  std::vector<std::pair<char, std::shared_ptr<const TrieNode>>> parents;
  for(char ch: key){
    parents.push_back(std::make_pair(ch, currentNode));
    if(currentNode == nullptr) {
      continue;
    }
    auto findnode = currentNode->children_.find(ch);
    if(findnode == currentNode->children_.end()){
      currentNode = nullptr;
    }else {
      currentNode = findnode->second;
    }
  }
  if(currentNode == nullptr) {
    currentNode = std::make_shared<TrieNodeWithValue<T>>(std::make_shared<T>(std::move(value)));
  }else{
    currentNode = std::make_shared<TrieNodeWithValue<T>>(currentNode->children_,std::make_shared<T>(std::move(value)));
  }
  while(!parents.empty()) {
    auto [key, node] = parents.back();
    parents.pop_back();
    std::shared_ptr<TrieNode> cloned_node;
    if(node != nullptr)
      cloned_node = node->Clone();
    else
      cloned_node = std::make_shared<TrieNode>(std::map<char, std::shared_ptr<const TrieNode>>{});
    cloned_node->children_[key] = currentNode;
    currentNode = std::move(cloned_node);
  }

  return Trie{currentNode->Clone()};
  // You should walk through the trie and create new nodes if necessary. If the node corresponding to the key already
  // exists, you should create a new `TrieNodeWithValue`.
}

/**
 * @brief Remove the key from the trie.
 * @return If the key does not exist, return the original trie. Otherwise, returns the new trie.
 */
auto Trie::Remove(std::string_view key) const -> Trie {
  throw NotImplementedException("Trie::Remove is not implemented.");

  // You should walk through the trie and remove nodes if necessary. If the node doesn't contain a value anymore,
  // you should convert it to `TrieNode`. If a node doesn't have children anymore, you should remove it.
}

// Below are explicit instantiation of template functions.
//
// Generally people would write the implementation of template classes and functions in the header file. However, we
// separate the implementation into a .cpp file to make things clearer. In order to make the compiler know the
// implementation of the template functions, we need to explicitly instantiate them here, so that they can be picked up
// by the linker.

template auto Trie::Put(std::string_view key, uint32_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint32_t *;

template auto Trie::Put(std::string_view key, uint64_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint64_t *;

template auto Trie::Put(std::string_view key, std::string value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const std::string *;

// If your solution cannot compile for non-copy tests, you can remove the below lines to get partial score.

using Integer = std::unique_ptr<uint32_t>;

template auto Trie::Put(std::string_view key, Integer value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const Integer *;

template auto Trie::Put(std::string_view key, MoveBlocked value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const MoveBlocked *;

}  // namespace bustub
