#ifndef TRIENODE_H
#define TRIENODE_H

#include <map>

class TrieNode
{
public:
    TrieNode();
    ~TrieNode();
    bool isEndOfWord;
    std::map<char, TrieNode *> children;
};

#endif // TRIENODE_H
