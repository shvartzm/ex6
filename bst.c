#include <stdlib.h>
#include "bst.h"

BST* createBST(int (*cmp)(void*, void*), void (*print)(void*), void (*freeData)(void*)){
    BST *bt = (BST*) malloc(sizeof(BST)); // mem alloc it
    bt -> root = NULL; // init the bst
    bt ->compare = cmp;
    bt ->print = print;
    bt ->freeData = freeData;
    return bt;
}

BSTNode* bstInsert(BSTNode* root, void* data, int (*cmp)(void*, void*)){
      if (root == NULL){ // first
        BSTNode *bst = (BSTNode*) malloc(sizeof(BSTNode)); // if first, needs to be allocated first
        bst ->data = data;
        bst->left = NULL;
        bst -> right = NULL;
        return bst;
      }
      if (cmp(data,root->data) < 0){ // go left
        root -> left = bstInsert(root->left, data,cmp);
      }
      else if(cmp(data,root->data) > 0){ // go right
        root -> right = bstInsert(root-> right,data,cmp);
      }
      return root;
}

void* bstFind(BSTNode* root, void* data, int (*cmp)(void*, void*)){
    if (root == NULL){// make sure we are not doing -> on NULL
        return NULL;
    }
    int result = cmp(data,root-> data);

    if(result == 0){
        return root;
    }else if (result < 0)
    {
        return bstFind(root-> left,data,cmp); // basically work as a binary search
    }
    return bstFind(root -> right, data,cmp);
    
}

void bstInorder(BSTNode* root, void (*print)(void*)){
    if (root == NULL){
        return;
    }
    bstInorder(root -> left,print); // LEFT, ROOT, RIGHT
    print(root -> data);
    bstInorder(root -> right,print);
}
void bstPreorder(BSTNode* root, void (*print)(void*)){
    if(root == NULL){
        return;
    }
    print(root-> data); // ROOT,LEFT,RIGHT
    bstPreorder(root -> left,print);
    bstPreorder(root -> right,print);
}
void bstPostorder(BSTNode* root, void (*print)(void*)){
    if(root == NULL){
        return;
    }
    bstPostorder(root -> left,print); // LEFT,RIGHT,ROOT
    bstPostorder(root -> right,print);
    print(root-> data);
}
void bstFree(BSTNode* root, void (*freeData)(void*)){
    if(root == NULL){ // make sure we dont free NULL
        return;
    }
    bstFree(root -> left,freeData); // free all the lefties
    bstFree(root -> right,freeData); // free all the righties
    freeData(root-> data); // free itself
    free(root); // free the root
}

