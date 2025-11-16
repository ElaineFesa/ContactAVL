#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "contact.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>

template<typename T>
class AVLTree {
private:
    struct Node {
        T data;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        int height;
        
        Node(const T& value) : data(value), height(1) {}
    };
    
    std::unique_ptr<Node> root;
    
    // Métodos auxiliares
    int height(const Node* node) const {
        return node ? node->height : 0;
    }
    
    int balanceFactor(const Node* node) const {
        return node ? height(node->left.get()) - height(node->right.get()) : 0;
    }
    
    void updateHeight(Node* node) {
        if (node) {
            node->height = std::max(height(node->left.get()), 
                                   height(node->right.get())) + 1;
        }
    }
    
    // Rotações
    std::unique_ptr<Node> rotateRight(std::unique_ptr<Node> y) {
        auto x = std::move(y->left);
        y->left = std::move(x->right);
        x->right = std::move(y);
        
        updateHeight(x->right.get());
        updateHeight(x.get());
        
        return x;
    }
    
    std::unique_ptr<Node> rotateLeft(std::unique_ptr<Node> x) {
        auto y = std::move(x->right);
        x->right = std::move(y->left);
        y->left = std::move(x);
        
        updateHeight(y->left.get());
        updateHeight(y.get());
        
        return y;
    }
    
    std::unique_ptr<Node> balance(std::unique_ptr<Node> node) {
        updateHeight(node.get());
        int bf = balanceFactor(node.get());
        
        // Caso Left-Left
        if (bf > 1 && balanceFactor(node->left.get()) >= 0) {
            return rotateRight(std::move(node));
        }
        
        // Caso Right-Right
        if (bf < -1 && balanceFactor(node->right.get()) <= 0) {
            return rotateLeft(std::move(node));
        }
        
        // Caso Left-Right
        if (bf > 1 && balanceFactor(node->left.get()) < 0) {
            node->left = rotateLeft(std::move(node->left));
            return rotateRight(std::move(node));
        }
        
        // Caso Right-Left
        if (bf < -1 && balanceFactor(node->right.get()) > 0) {
            node->right = rotateRight(std::move(node->right));
            return rotateLeft(std::move(node));
        }
        
        return node;
    }
    
    // Inserção recursiva
    std::unique_ptr<Node> insertRec(std::unique_ptr<Node> node, const T& value) {
        if (!node) {
            return std::make_unique<Node>(value);
        }
        
        if (value < node->data) {
            node->left = insertRec(std::move(node->left), value);
        } else if (value > node->data) {
            node->right = insertRec(std::move(node->right), value);
        } else {
            return node; // Duplicado
        }
        
        return balance(std::move(node));
    }
    
    // Encontrar mínimo
    Node* findMin(Node* node) const {
        while (node && node->left) {
            node = node->left.get();
        }
        return node;
    }
    
    // Remoção recursiva
    std::unique_ptr<Node> removeRec(std::unique_ptr<Node> node, const T& value) {
        if (!node) return nullptr;
        
        if (value < node->data) {
            node->left = removeRec(std::move(node->left), value);
        } else if (value > node->data) {
            node->right = removeRec(std::move(node->right), value);
        } else {
            if (!node->left || !node->right) {
                node = std::move(node->left ? node->left : node->right);
            } else {
                Node* minNode = findMin(node->right.get());
                node->data = minNode->data;
                node->right = removeRec(std::move(node->right), minNode->data);
            }
        }
        
        if (!node) return nullptr;
        return balance(std::move(node));
    }
    
    // Busca recursiva
    Node* searchRec(Node* node, const T& value) const {
        if (!node) return nullptr;
        
        if (value < node->data) {
            return searchRec(node->left.get(), value);
        } else if (value > node->data) {
            return searchRec(node->right.get(), value);
        } else {
            return node;
        }
    }
    
    // Travessias
    void inOrderRec(Node* node, std::vector<T>& result) const {
        if (!node) return;
        
        inOrderRec(node->left.get(), result);
        result.push_back(node->data);
        inOrderRec(node->right.get(), result);
    }
    
    void collectFavoritesRec(Node* node, std::vector<T>& result) const {
        if (!node) return;
        
        collectFavoritesRec(node->left.get(), result);
        if (node->data.isFavorite()) {
            result.push_back(node->data);
        }
        collectFavoritesRec(node->right.get(), result);
    }
    
    // Verificação de balanceamento
    bool isBalancedRec(Node* node) const {
        if (!node) return true;
        
        int bf = std::abs(balanceFactor(node));
        return bf <= 1 && 
               isBalancedRec(node->left.get()) && 
               isBalancedRec(node->right.get());
    }
    
public:
    AVLTree() = default;
    
    // Operações principais
    void insert(const T& value) {
        root = insertRec(std::move(root), value);
    }
    
    void remove(const T& value) {
        root = removeRec(std::move(root), value);
    }
    
    bool contains(const T& value) const {
        return searchRec(root.get(), value) != nullptr;
    }
    
    T* search(const T& value) {
        Node* node = searchRec(root.get(), value);
        return node ? &node->data : nullptr;
    }
    
    const T* search(const T& value) const {
        const Node* node = searchRec(root.get(), value);
        return node ? &node->data : nullptr;
    }
    
    // Travessias
    std::vector<T> inOrder() const {
        std::vector<T> result;
        inOrderRec(root.get(), result);
        return result;
    }
    
    std::vector<T> getFavorites() const {
        std::vector<T> result;
        collectFavoritesRec(root.get(), result);
        return result;
    }
    
    // Verificação de propriedades AVL
    bool isBalanced() const {
        return isBalancedRec(root.get());
    }
    
    bool isEmpty() const {
        return root == nullptr;
    }
    
    int size() const {
        return inOrder().size();
    }
};

#endif