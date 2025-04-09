#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item) override; // TODO
    virtual void remove(const Key& key) override;  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);

    void rebalanceInsert(AVLNode<Key, Value>* node);
    void rebalanceRemove(AVLNode<Key, Value>* node, bool removedLeft);

    void fixDoubleRotationBalances(AVLNode<Key, Value>* child, AVLNode<Key, Value>* left, AVLNode<Key, Value>* right);

    

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    if (this->root_ == nullptr) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        return;
    }

    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;

    while (current != nullptr) {
        parent = current;
        if (new_item.first < current->getKey()) {
            current = current->getLeft();
        } else if (new_item.first > current->getKey()) {
            current = current->getRight();
        } else {
            current->setValue(new_item.second);
            return;
        }
    }

    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    if (new_item.first < parent->getKey()) {
        parent->setLeft(newNode);
    } else {
        parent->setRight(newNode);
    }

    rebalanceInsert(newNode);
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO

    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (node == nullptr) return;

    if (node->getLeft() != nullptr && node->getRight() != nullptr) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(node));
        this->nodeSwap(node, pred);
    }

    AVLNode<Key, Value>* child = nullptr;
    if (node->getLeft() != nullptr) {
        child = node->getLeft();
    } else {
        if (node->getRight() != nullptr) {
            child = node->getRight();
        }
    }

    AVLNode<Key, Value>* parent = node->getParent();
    bool removedLeft = false;
    if (parent != nullptr && parent->getLeft() == node) {
        removedLeft = true;
    }

    if (child != nullptr) {
        child->setParent(parent);
    }

    if (parent == nullptr) {
        this->root_ = child;
    } else {
        if (removedLeft) {
            parent->setLeft(child);
        } else {
            parent->setRight(child);
        }
    }

    delete node;
    rebalanceRemove(parent, removedLeft);
}


//helpers

template<class Key, class Value>
void AVLTree<Key, Value>::rebalanceInsert(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* parent = node->getParent();
    AVLNode<Key, Value>* current = node;

    while (parent != nullptr) {
        if (current == parent->getLeft()) {
            parent->updateBalance(-1);
        } else {
            parent->updateBalance(1);
        }

        int bal = parent->getBalance();

        if (bal == 0) break;
        else if (bal == -1 || bal == 1) {
            current = parent;
            parent = parent->getParent();
        }
        else if (bal == -2) {
            if (current->getBalance() <= 0) {
                rotateRight(parent);
                parent->setBalance(0);
                current->setBalance(0);
            } else {
                AVLNode<Key, Value>* child = current->getRight();
                rotateLeft(current);
                rotateRight(parent);
                fixDoubleRotationBalances(child, current, parent); 
            }
            break;
        }
        else if (bal == 2) {
            if (current->getBalance() >= 0) {
                rotateLeft(parent);
                parent->setBalance(0);
                current->setBalance(0);
            } else {
                AVLNode<Key, Value>* child = current->getLeft();
                rotateRight(current);
                rotateLeft(parent);
                fixDoubleRotationBalances(child, parent, current); 
            }
            break;
        }
    }
}



template<class Key, class Value>
void AVLTree<Key, Value>::rebalanceRemove(AVLNode<Key, Value>* node, bool removedLeft)
{
    AVLNode<Key, Value>* current = node;
    int diff = removedLeft ? 1 : -1;

    while (current != nullptr) {
        current->updateBalance(diff);
        int balance = current->getBalance();

        if (balance == 1 || balance == -1) break;
        else if (balance == 0) {
            AVLNode<Key, Value>* parent = current->getParent();
            if (parent != nullptr) {
                diff = (current == parent->getLeft()) ? 1 : -1;
            }
            current = parent;
        }
        else if (balance == -2) {
            AVLNode<Key, Value>* left = current->getLeft();
            int leftBal = left->getBalance();

            if (leftBal <= 0) {
                rotateRight(current);
                if (leftBal == 0) {
                    current->setBalance(-1);
                    left->setBalance(1);
                    break;
                } else {
                    current->setBalance(0);
                    left->setBalance(0);
                    current = left->getParent();
                    diff = (current != nullptr && current->getLeft() == left) ? 1 : -1;
                }
            } else {
                AVLNode<Key, Value>* lr = left->getRight();
                rotateLeft(left);
                rotateRight(current);
                fixDoubleRotationBalances(lr, left, current);
                current = lr->getParent();
                diff = (current != nullptr && current->getLeft() == lr) ? 1 : -1;
            }
        }
        else if (balance == 2) {
            AVLNode<Key, Value>* right = current->getRight();
            int rightBal = right->getBalance();

            if (rightBal >= 0) {
                rotateLeft(current);
                if (rightBal == 0) {
                    current->setBalance(1);
                    right->setBalance(-1);
                    break;
                } else {
                    current->setBalance(0);
                    right->setBalance(0);
                    current = right->getParent();
                    diff = (current != nullptr && current->getLeft() == right) ? 1 : -1;
                }
            } else {
                AVLNode<Key, Value>* rl = right->getLeft();
                rotateRight(right);
                rotateLeft(current);
                fixDoubleRotationBalances(rl, current, right);
                current = rl->getParent();
                diff = (current != nullptr && current->getLeft() == rl) ? 1 : -1;
            }
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::fixDoubleRotationBalances(AVLNode<Key, Value>* child, AVLNode<Key, Value>* left, AVLNode<Key, Value>* right)
{
    if (child->getBalance() < 0) {
        left->setBalance(0);
        right->setBalance(1);
    } else if (child->getBalance() > 0) {
        left->setBalance(-1);
        right->setBalance(0);
    } else {
        left->setBalance(0);
        right->setBalance(0);
    }
    child->setBalance(0);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* x)
{
    AVLNode<Key, Value>* y = x->getRight();
    x->setRight(y->getLeft());
    if (y->getLeft() != nullptr) {
        y->getLeft()->setParent(x);
    }
    y->setParent(x->getParent());
    if (x->getParent() == nullptr) {
        this->root_ = y;
    } else if (x == x->getParent()->getLeft()) {
        x->getParent()->setLeft(y);
    } else {
        x->getParent()->setRight(y);
    }
    y->setLeft(x);
    x->setParent(y);
}


template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* x)
{
    AVLNode<Key, Value>* y = x->getLeft();
    x->setLeft(y->getRight());
    if (y->getRight() != nullptr) {
        y->getRight()->setParent(x);
    }
    y->setParent(x->getParent());
    if (x->getParent() == nullptr) {
        this->root_ = y;
    } else if (x == x->getParent()->getLeft()) {
        x->getParent()->setLeft(y);
    } else {
        x->getParent()->setRight(y);
    }
    y->setRight(x);
    x->setParent(y);
}


template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
