#ifndef TRIE_H
#define TRIE_H

#include <string>
#include <vector>
#include "TrieNode.h"

class Trie
{
public:
    Trie();
    ~Trie();
    void insert(const std::string &word);
    bool search(const std::string &word);
    std::vector<std::string> searchWithPrefix(const std::string &prefix);

private:
    TrieNode *root;
    void searchWithPrefixHelper(TrieNode *node, std::string prefix, std::vector<std::string> &results);
};

#endif // TRIE_H
