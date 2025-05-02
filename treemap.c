#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"

typedef struct TreeNode TreeNode;


struct TreeNode {
    Pair* pair;
    TreeNode * left;
    TreeNode * right;
    TreeNode * parent;
};

struct TreeMap {
    TreeNode * root;
    TreeNode * current;
    int (*lower_than) (void* key1, void* key2);
};

int is_equal(TreeMap* tree, void* key1, void* key2){
    if(tree->lower_than(key1,key2)==0 &&  
        tree->lower_than(key2,key1)==0) return 1;
    else return 0;
}


TreeNode * createTreeNode(void* key, void * value) {
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (new == NULL) return NULL;
    new->pair = (Pair *)malloc(sizeof(Pair));
    new->pair->key = key;
    new->pair->value = value;
    new->parent = new->left = new->right = NULL;
    return new;
}

TreeMap * createTreeMap(int (*lower_than) (void* key1, void* key2)) {
    TreeMap * new = (TreeMap *)malloc(sizeof(TreeMap));
    if (new == NULL) return NULL;
    new->root = NULL;
    new->current = NULL;
    new->lower_than = lower_than;
    return new;
}


void insertTreeMap(TreeMap * tree, void* key, void * value) {
    if (tree == NULL) return;
    TreeNode * nuevo = createTreeNode(key, value);
    if (nuevo == NULL) return;
    if(tree->root == NULL) {
        tree->root = nuevo;
        tree->current = nuevo;
        return;
    }
    TreeNode * current = tree->root;
    TreeNode * parent = NULL;
    while (current != NULL) {
        parent = current;
        if (!tree->lower_than(key, current->pair->key) && //evitar duplicado
            !tree->lower_than(current->pair->key, key)) {
            free(nuevo);
            return;
        }
        if (tree->lower_than(key, current->pair->key)) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    nuevo->parent = parent;
    if (tree->lower_than(key, parent->pair->key)) {
        parent->left = nuevo;
    } else {
        parent->right = nuevo;
    }
    tree->current = nuevo;

}

TreeNode * minimum(TreeNode * x){
    if (x == NULL) return NULL;
    while (x->left != NULL) {
        x = x->left;
    }
    return x;
}


void removeNode(TreeMap * tree, TreeNode* node) {
    if (node == NULL || tree == NULL) return;

    if (node->left == NULL && node->right == NULL) { //sin hijos
        if (node->parent == NULL) {
            tree->root = NULL;
        } else if (node->parent->left == node) {
            node->parent->left = NULL;
        } else {
            node->parent->right = NULL;
        }
    } else if (node->left == NULL || node->right == NULL) { // un hijo
        TreeNode* hijo = (node->left != NULL) ? node->left : node->right;
        if (node->parent == NULL) {
            tree->root = hijo;
        } else if (node->parent->left == node) {
            node->parent->left = hijo;
        } else {
            node->parent->right = hijo;
        }
        hijo->parent = node->parent;
    } else { // dos hijos
        TreeNode* successor = minimum(node->right);
        node->pair = successor->pair;
        removeNode(tree, successor);
        return;
    }
    free(node);

}

void eraseTreeMap(TreeMap * tree, void* key){
    if (tree == NULL || tree->root == NULL) return;

    if (searchTreeMap(tree, key) == NULL) return;
    TreeNode* node = tree->current;
    removeNode(tree, node);

}




Pair * searchTreeMap(TreeMap * tree, void* key) {
    if (tree == NULL || tree->root == NULL) return NULL;
    TreeNode * current = tree->root;
    while (current != NULL) {
        if (tree->lower_than(key, current->pair->key)) {
            current = current->left;
        } else if (tree->lower_than(current->pair->key, key)) {
            current = current->right;
        } else {
            tree->current = current;
            return current->pair;
        }
    }
    return NULL;
}


Pair * upperBound(TreeMap * tree, void* key) {
    if (tree == NULL || tree->root == NULL) return NULL;
    TreeNode * current = tree->root;
    TreeNode * result = NULL;
    while (!current == NULL) {
        if (tree->lower_than(key, current->pair->key)) {
            result = current;
            current = current->left;
        } else {
            current = current->right;
        }
    }
    if (result != NULL) {
        tree->current = result;
        return result->pair;
    }
    return NULL;
}

Pair * firstTreeMap(TreeMap * tree) {
    if (tree == NULL || tree->root == NULL) return NULL;
    TreeNode * node = minimum(tree->root);
    if (node != NULL) {
        tree->current = node;
        return node->pair;
    }
    return NULL;
}

Pair * nextTreeMap(TreeMap * tree) {
    if (tree == NULL || tree->current == NULL) return NULL;
    TreeNode * current = tree->current;
    if (current->right != NULL) {
        tree->current = minimum(current->right);
        return tree->current->pair;
    }
    TreeNode * parent = current->parent;
    while(parent !=NULL && current == parent->right) {
        current = parent;
        parent = parent->parent;
    }
    tree->current = parent;
    if (parent != NULL) return parent->pair;

    return NULL;
}
