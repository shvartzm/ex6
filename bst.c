#include <stdlib.h>
#include "bst.h"

BST* createBST(int (*cmp)(void*, void*), void (*print)(void*), void (*freeData)(void*)){
    BST *bt = (BST*) malloc(sizeof(BST));
    bt -> root = NULL;
    bt ->compare = cmp;
    bt ->print = print;
    bt ->freeData = freeData;
    return bt;
}

BSTNode* bstInsert(BSTNode* root, void* data, int (*cmp)(void*, void*)){
      if (root == NULL){
        BSTNode *bst = (BSTNode*) malloc(sizeof(BSTNode));
        bst ->data = data;
        bst->left = NULL;
        bst -> right = NULL;
        return bst;
      }
      if (cmp(data,root->data) < 0){
        root -> left = bstInsert(root->left, data,cmp);
      }
      else if(cmp(data,root->data) > 0){
        root -> right = bstInsert(root-> right,data,cmp);
      }
      return root;
}

void* bstFind(BSTNode* root, void* data, int (*cmp)(void*, void*)){
    if (root == NULL){
        return NULL;
    }
    int result = cmp(data,root-> data);

    if(result == 0){
        return root;
    }else if (result < 0)
    {
        return bstFind(root-> left,data,cmp);
    }
    return bstFind(root -> right, data,cmp);
    
}

void bstInorder(BSTNode* root, void (*print)(void*)){
    if (root == NULL){
        return;
    }
    bstInorder(root -> left,print);
    print(root -> data);
    bstInorder(root -> right,print);
}
void bstPreorder(BSTNode* root, void (*print)(void*)){
    if(root == NULL){
        return;
    }
    print(root-> data);
    bstPreorder(root -> left,print);
    bstPreorder(root -> right,print);
}
void bstPostorder(BSTNode* root, void (*print)(void*)){
    if(root == NULL){
        return;
    }
    bstPostorder(root -> left,print);
    bstPostorder(root -> right,print);
    print(root-> data);
}
void bstFree(BSTNode* root, void (*freeData)(void*)){
    if(root == NULL){
        return;
    }
    bstFree(root -> left,freeData);
    bstFree(root -> right,freeData);
    freeData(root-> data);
    free(root);
}

