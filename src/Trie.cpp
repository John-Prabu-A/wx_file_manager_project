#include "Trie.h"

Trie::Trie()
{
    root = new TrieNode();
}

Trie::~Trie()
{
    delete root;
}

void Trie::insert(const std::string &word)
{
    TrieNode *node = root;
    for (char c : word)
    {
        if (node->children.find(c) == node->children.end())
        {
            node->children[c] = new TrieNode();
        }
        node = node->children[c];
    }
    node->isEndOfWord = true;
}

bool Trie::search(const std::string &word)
{
    TrieNode *node = root;
    for (char c : word)
    {
        if (node->children.find(c) == node->children.end())
        {
            return false;
        }
        node = node->children[c];
    }
    return node->isEndOfWord;
}

std::vector<std::string> Trie::searchWithPrefix(const std::string &prefix)
{
    std::vector<std::string> results;
    TrieNode *node = root;
    for (char c : prefix)
    {
        if (node->children.find(c) == node->children.end())
        {
            return results;
        }
        node = node->children[c];
    }
    searchWithPrefixHelper(node, prefix, results);
    return results;
}

void Trie::searchWithPrefixHelper(TrieNode *node, std::string prefix, std::vector<std::string> &results)
{
    if (node->isEndOfWord)
    {
        results.push_back(prefix);
    }
    for (const auto &pair : node->children)
    {
        searchWithPrefixHelper(pair.second, prefix + pair.first, results);
    }
}
