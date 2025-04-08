#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool testPaths(Node* node, int currentLeaf, int& leafDepth) {
    if (node == nullptr) return true;

    if (node->left == nullptr && node->right == nullptr) {
        if (leafDepth == -1) {
            leafDepth = currentLeaf; 
            return true;
        }
        return currentLeaf == leafDepth; 
    }

    bool leftTest = testPaths(node->left, currentLeaf + 1, leafDepth);
    bool rightTest = testPaths(node->right, currentLeaf + 1, leafDepth);

    return leftTest && rightTest;
}


bool equalPaths(Node * root)
{
    // Add your code below
    int leafDepth = -1;  
    return testPaths(root, 0, leafDepth);

}

