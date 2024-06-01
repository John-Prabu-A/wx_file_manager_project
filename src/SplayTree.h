#ifndef SPLAYTREE_H
#define SPLAYTREE_H

#include <vector>

template <typename T>
struct SplayTreeNode
{
    T key;
    SplayTreeNode *left;
    SplayTreeNode *right;
    SplayTreeNode(const T &k) : key(k), left(nullptr), right(nullptr) {}
};

template <typename T>
class SplayTree
{
public:
    SplayTree();
    ~SplayTree();
    void insert(const T &key);
    bool find(const T &key);
    void remove(const T &key);
    std::vector<T> getRecentlyAccessed();

private:
    SplayTreeNode<T> *root;
    SplayTreeNode<T> *splay(SplayTreeNode<T> *root, const T &key);
    SplayTreeNode<T> *rightRotate(SplayTreeNode<T> *x);
    SplayTreeNode<T> *leftRotate(SplayTreeNode<T> *x);
    void inorder(SplayTreeNode<T> *root, std::vector<T> &result);
    void destroy(SplayTreeNode<T> *node);
};

#include "SplayTree.cpp"

#endif // SPLAYTREE_H
