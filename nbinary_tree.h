#pragma once

#include <iostream>

namespace nstd {
    template <typename T>
    class BinaryTree{

        struct BT_Node{
            T contents;
            BT_Node* left_child;
            BT_Node* right_child;

            BT_Node(const T& _contents, BT_Node* lptr, BT_Node* rptr): contents(_contents), left_child(lptr), right_child(rptr) {}
            ~BT_Node() {}
        };

        BT_Node* minValueNode(BT_Node* node);

        BT_Node* root;
        //unsigned int size;
        const bool disallow_duplicates;
        int (*cmp_func)(const T& a, const T& b);
        BT_Node* removeNode(BT_Node* root, const T& item);
        bool insertRec(BT_Node* root, const T& item);

    public:
        BinaryTree(int (*_cmp_func)(const T& a, const T& b), const bool _disallow_duplicates): root(nullptr), cmp_func(_cmp_func), disallow_duplicates(_disallow_duplicates) {}
        ~BinaryTree()
        {
            while(!isEmpty())
                root = removeNode(root, root->contents);
        }


        bool isEmpty() const { return (root == nullptr); }
        BT_Node* getRoot() { return root; }

        bool insert(const T& item)
        {
            if(root)
                return insertRec(root, item);
            else
            {
                root = new BT_Node(item, nullptr, nullptr);
                return true;
            }
        }
        
        void removeValue(const T& item)
        {
            root = removeNode(root, item); 
        }
        
        void inorder(BT_Node* root)
        {
            if (root != nullptr)
            {
                inorder(root->left_child);
                std::cout<<root->contents<<std::endl;
                inorder(root->right_child);
            }
        }
    };

    template <typename T>
    inline bool BinaryTree<T>::insertRec(typename BinaryTree<T>::BT_Node* root, const T& item)
    {
        
        const int cmp_result= cmp_func(item, root->contents );

        if(cmp_result == 0) // Check if duplicates are allowed and act accordingly
        {
            if(disallow_duplicates)
                return false;

            if(root->right_child)
                return insertRec(root->right_child, item);
            else
            {
                root->right_child = new BT_Node(item, nullptr, nullptr);
                return true;
            }
        }
        else if(cmp_result > 0) // Go right
        {  

            if(root->right_child)
                return insertRec(root->right_child, item);
            else
            {
                root->right_child = new BT_Node(item, nullptr, nullptr);
                return true;
            }
        }
        else // Go left
        {
           if(root->left_child)
                return insertRec(root->left_child, item);
            else
            {
                root->left_child = new BT_Node(item, nullptr, nullptr);
                return true;
            }
        }
    }

    template <typename T>
    inline typename BinaryTree<T>::BT_Node* BinaryTree<T>::removeNode(typename BinaryTree<T>::BT_Node* root, const T& item)
    {
        if (root == nullptr) return root;

        const int cmp_result = cmp_func(item, root->contents);
        if (cmp_result < 0)
            root->left_child = removeNode(root->left_child, item);
    
        else if (cmp_result > 0)
            root->right_child = removeNode(root->right_child, item);
    
        else
        {
            if(root->left_child && root->right_child)
            {
                typename BinaryTree<T>::BT_Node* temp = minValueNode(root->right_child);
                root->contents = temp->contents;
                root->right_child = removeNode(root->right_child, temp->contents);
            }
            else if (root->left_child == nullptr)
            {
                typename BinaryTree<T>::BT_Node* temp = root;
                root = root->right_child;
                delete temp;
                
            }
            else if (root->right_child == nullptr)
            {
                typename BinaryTree<T>::BT_Node* temp = root;
                root = root->left_child;
                delete temp;
               
            }
        }
        return root;
    }

    template <typename T>
    inline typename BinaryTree<T>::BT_Node* BinaryTree<T>::minValueNode(typename BinaryTree<T>::BT_Node* node)
    {
        typename BinaryTree<T>::BT_Node*  current = node;
        while (current->left_child != nullptr)
            current = current->left_child;
        return current;
    }
}