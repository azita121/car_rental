#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <iostream>
#include <queue>
#include <functional>
using namespace std;

// Forward declarations
template<typename T>
class LinkedList;

// Node for Linked List
template<typename T>
class ListNode {
public:
    T data;
    ListNode<T>* next;
    
    ListNode(const T& data) : data(data), next(nullptr) {}
};

// Singly Linked List implementation
template<typename T>
class LinkedList {
private:
    ListNode<T>* head;
    int size;
    
public:
    LinkedList() : head(nullptr), size(0) {}
    
    ~LinkedList() {
        clear();
    }
    
    void append(const T& data) {
        ListNode<T>* newNode = new ListNode<T>(data);
        if (!head) {
            head = newNode;
        } else {
            ListNode<T>* current = head;
            while (current->next) {
                current = current->next;
            }
            current->next = newNode;
        }
        size++;
    }
    
    void remove(const T& data) {
        if (!head) return;
        
        if (head->data == data) {
            ListNode<T>* temp = head;
            head = head->next;
            delete temp;
            size--;
            return;
        }
        
        ListNode<T>* current = head;
        while (current->next && current->next->data != data) {
            current = current->next;
        }
        
        if (current->next) {
            ListNode<T>* temp = current->next;
            current->next = current->next->next;
            delete temp;
            size--;
        }
    }
    
    bool contains(const T& data) const {
        ListNode<T>* current = head;
        while (current) {
            if (current->data == data) return true;
            current = current->next;
        }
        return false;
    }
    
    int getSize() const { return size; }
    
    bool isEmpty() const { return size == 0; }
    
    void clear() {
        while (head) {
            ListNode<T>* temp = head;
            head = head->next;
            delete temp;
        }
        size = 0;
    }
    
    // Iterator support
    class Iterator {
    private:
        ListNode<T>* current;
        
    public:
        Iterator(ListNode<T>* node) : current(node) {}
        
        T& operator*() { return current->data; }
        Iterator& operator++() {
            if (current) current = current->next;
            return *this;
        }
        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }
    };
    
    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }
};

// AVL Tree Node
template<typename Key, typename Value>
class AVLNode {
public:
    Key key;
    Value value;
    AVLNode<Key, Value>* left;
    AVLNode<Key, Value>* right;
    int height;
    
    AVLNode(const Key& k, const Value& v) 
        : key(k), value(v), left(nullptr), right(nullptr), height(1) {}
};

// AVL Tree implementation for fast username lookup
template<typename Key, typename Value>
class AVLTree {
private:
    AVLNode<Key, Value>* root;
    
    int getHeight(AVLNode<Key, Value>* node) {
        return node ? node->height : 0;
    }
    
    int getBalance(AVLNode<Key, Value>* node) {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }
    
    AVLNode<Key, Value>* rightRotate(AVLNode<Key, Value>* y) {
        AVLNode<Key, Value>* x = y->left;
        AVLNode<Key, Value>* T2 = x->right;
        
        x->right = y;
        y->left = T2;
        
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        
        return x;
    }
    
    AVLNode<Key, Value>* leftRotate(AVLNode<Key, Value>* x) {
        AVLNode<Key, Value>* y = x->right;
        AVLNode<Key, Value>* T2 = y->left;
        
        y->left = x;
        x->right = T2;
        
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        
        return y;
    }
    
    AVLNode<Key, Value>* insertNode(AVLNode<Key, Value>* node, const Key& key, const Value& value) {
        if (!node) {
            return new AVLNode<Key, Value>(key, value);
        }
        
        if (key < node->key) {
            node->left = insertNode(node->left, key, value);
        } else if (key > node->key) {
            node->right = insertNode(node->right, key, value);
        } else {
            // Key already exists, update value
            node->value = value;
            return node;
        }
        
        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        
        int balance = getBalance(node);
        
        // Left Left Case
        if (balance > 1 && key < node->left->key) {
            return rightRotate(node);
        }
        
        // Right Right Case
        if (balance < -1 && key > node->right->key) {
            return leftRotate(node);
        }
        
        // Left Right Case
        if (balance > 1 && key > node->left->key) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        
        // Right Left Case
        if (balance < -1 && key < node->right->key) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        
        return node;
    }
    
    AVLNode<Key, Value>* findNode(AVLNode<Key, Value>* node, const Key& key) const {
        if (!node || node->key == key) {
            return node;
        }
        
        if (key < node->key) {
            return findNode(node->left, key);
        }
        
        return findNode(node->right, key);
    }
    
    void clearTree(AVLNode<Key, Value>* node) {
        if (node) {
            clearTree(node->left);
            clearTree(node->right);
            delete node;
        }
    }
    
public:
    AVLTree() : root(nullptr) {}
    
    ~AVLTree() {
        clearTree(root);
    }
    
    void insert(const Key& key, const Value& value) {
        root = insertNode(root, key, value);
    }
    
    Value* find(const Key& key) const {
        AVLNode<Key, Value>* node = findNode(root, key);
        return node ? &(node->value) : nullptr;
    }
    
    bool contains(const Key& key) const {
        return find(key) != nullptr;
    }
    
    void clear() {
        clearTree(root);
        root = nullptr;
    }
};

// Priority Queue wrapper using STL (for reservation queues)
template<typename T, typename Compare = less<T>>
class PriorityQueue {
private:
    priority_queue<T, vector<T>, Compare> pq;
    
public:
    void push(const T& item) {
        pq.push(item);
    }
    
    void pop() {
        if (!pq.empty()) {
            pq.pop();
        }
    }
    
    T top() const {
        return pq.top();
    }
    
    bool empty() const {
        return pq.empty();
    }
    
    int size() const {
        return pq.size();
    }
};

#endif // DATA_STRUCTURES_H
