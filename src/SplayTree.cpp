#include "SplayTree.h"

template <typename T>
SplayTree<T>::SplayTree() : root(nullptr) {}

template <typename T>
SplayTree<T>::~SplayTree()
{
    destroy(root);
}

template <typename T>
void SplayTree<T>::destroy(SplayTreeNode<T> *node)
{
    if (node)
    {
        destroy(node->left);
        destroy(node->right);
        delete node;
    }
}

template <typename T>
SplayTreeNode<T> *SplayTree<T>::rightRotate(SplayTreeNode<T> *x)
{
    SplayTreeNode<T> *y = x->left;
    x->left = y->right;
    y->right = x;
    return y;
}

template <typename T>
SplayTreeNode<T> *SplayTree<T>::leftRotate(SplayTreeNode<T> *x)
{
    SplayTreeNode<T> *y = x->right;
    x->right = y->left;
    y->left = x;
    return y;
}

template <typename T>
SplayTreeNode<T> *SplayTree<T>::splay(SplayTreeNode<T> *root, const T &key)
{
    if (root == nullptr || root->key == key)
        return root;

    if (root->key > key)
    {
        if (root->left == nullptr)
            return root;

        if (root->left->key > key)
        {
            root->left->left = splay(root->left->left, key);
            root = rightRotate(root);
        }
        else if (root->left->key < key)
        {
            root->left->right = splay(root->left->right, key);
            if (root->left->right != nullptr)
                root->left = leftRotate(root->left);
        }
        return (root->left == nullptr) ? root : rightRotate(root);
    }
    else
    {
        if (root->right == nullptr)
            return root;

        if (root->right->key > key)
        {
            root->right->left = splay(root->right->left, key);
            if (root->right->left != nullptr)
                root->right = rightRotate(root->right);
        }
        else if (root->right->key < key)
        {
            root->right->right = splay(root->right->right, key);
            root = leftRotate(root);
        }
        return (root->right == nullptr) ? root : leftRotate(root);
    }
}

template <typename T>
void SplayTree<T>::insert(const T &key)
{
    if (root == nullptr)
    {
        root = new SplayTreeNode<T>(key);
        return;
    }

    root = splay(root, key);

    if (root->key == key)
        return;

    SplayTreeNode<T> *newNode = new SplayTreeNode<T>(key);

    if (root->key > key)
    {
        newNode->right = root;
        newNode->left = root->left;
        root->left = nullptr;
    }
    else
    {
        newNode->left = root;
        newNode->right = root->right;
        root->right = nullptr;
    }

    root = newNode;
}

template <typename T>
bool SplayTree<T>::find(const T &key)
{
    root = splay(root, key);
    return root != nullptr && root->key == key;
}

template <typename T>
void SplayTree<T>::remove(const T &key)
{
    if (root == nullptr)
        return;

    root = splay(root, key);

    if (root->key != key)
        return;

    if (root->left == nullptr)
    {
        SplayTreeNode<T> *temp = root;
        root = root->right;
        delete temp;
    }
    else
    {
        SplayTreeNode<T> *temp = root;
        root = splay(root->left, key);
        root->right = temp->right;
        delete temp;
    }
}

template <typename T>
void SplayTree<T>::inorder(SplayTreeNode<T> *root, std::vector<T> &result)
{
    if (root)
    {
        inorder(root->left, result);
        result.push_back(root->key);
        inorder(root->right, result);
    }
}

template <typename T>
std::vector<T> SplayTree<T>::getRecentlyAccessed()
{
    std::vector<T> result;
    inorder(root, result);
    return result;
}
