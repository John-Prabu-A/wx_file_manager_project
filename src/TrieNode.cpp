#include "TrieNode.h"

TrieNode::TrieNode() : isEndOfWord(false) {}

TrieNode::~TrieNode()
{
    for (auto &child : children)
    {
        delete child.second;
    }
}
