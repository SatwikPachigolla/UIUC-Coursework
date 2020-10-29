/**
 * @file avltree.cpp
 * Definitions of the binary tree functions you'll be writing for this lab.
 * You'll need to modify this file.
 */



template <class K, class V>
V AVLTree<K, V>::find(const K& key) const
{
    return find(root, key);
}

template <class K, class V>
V AVLTree<K, V>::find(Node* subtree, const K& key) const
{
    if (subtree == NULL)
        return V();
    else if (key == subtree->key)
        return subtree->value;
    else {
        if (key < subtree->key)
            return find(subtree->left, key);
        else
            return find(subtree->right, key);
    }
}

template <class K, class V>
void AVLTree<K, V>::rotateLeft(Node*& t)
{
    functionCalls.push_back("rotateLeft"); // Stores the rotation name (don't remove this)
    // your code here
    Node* temp = t->right;
    t->right = temp->left;
    temp->left = t;
    t->height = 1+std::max(heightOrNeg1(t->left), heightOrNeg1(t->right));
    temp->height = 1+std::max(heightOrNeg1(temp->left), heightOrNeg1(temp->right));
    t=temp;

}

template <class K, class V>
void AVLTree<K, V>::rotateLeftRight(Node*& t)
{
    functionCalls.push_back("rotateLeftRight"); // Stores the rotation name (don't remove this)
    // Implemented for you:
    rotateLeft(t->left);
    rotateRight(t);
}

template <class K, class V>
void AVLTree<K, V>::rotateRight(Node*& t)
{
    functionCalls.push_back("rotateRight"); // Stores the rotation name (don't remove this)
    // your code here
    Node* temp = t->left;
    t->left = temp->right;
    temp->right = t;
    t->height = 1+std::max(heightOrNeg1(t->left), heightOrNeg1(t->right));
    temp->height = 1+std::max(heightOrNeg1(temp->left), heightOrNeg1(temp->right));
    t=temp;
}

template <class K, class V>
void AVLTree<K, V>::rotateRightLeft(Node*& t)
{
    functionCalls.push_back("rotateRightLeft"); // Stores the rotation name (don't remove this)
    // your code here
    rotateRight(t->right);
    rotateLeft(t);
}

template <class K, class V>
void AVLTree<K, V>::rebalance(Node*& subtree)
{
    // your code here
    if(subtree==NULL){
      return;
    }
    subtree->height = 1+std::max(heightOrNeg1(subtree->left), heightOrNeg1(subtree->right));
    if(heightOrNeg1(subtree->left)-heightOrNeg1(subtree->right)>1){
      if(heightOrNeg1(subtree->left->right) - heightOrNeg1(subtree->left->left) > 0){
        rotateLeftRight(subtree);
      }
      else{
        rotateRight(subtree);
      }
    }
    else if(heightOrNeg1(subtree->right)-heightOrNeg1(subtree->left)>1){
      if(heightOrNeg1(subtree->right->left) - heightOrNeg1(subtree->right->right) > 0){
        rotateRightLeft(subtree);
      }
      else{
      rotateLeft(subtree);
      }
    }

}

template <class K, class V>
void AVLTree<K, V>::insert(const K & key, const V & value)
{
    insert(root, key, value);
}

template <class K, class V>
void AVLTree<K, V>::insert(Node*& subtree, const K& key, const V& value)
{
    // your code here
    if(subtree == NULL){
      subtree = new Node(key, value);
      subtree->height = 0;
    }
    else{
      if(subtree->key>key){
        if(subtree->left !=NULL){
          insert(subtree->left, key, value);
        }
        else{
          subtree->left = new Node(key, value);
          subtree->left->height = 0;
        }
      }
      else if(subtree->key<key){
        if(subtree->right !=NULL){
          insert(subtree->right, key, value);
        }
        else{
          subtree->right = new Node(key, value);
          subtree->right->height = 0;
        }
      }
    }
    rebalance(subtree);
}

template <class K, class V>
void AVLTree<K, V>::remove(const K& key)
{
    remove(root, key);
}

template <class K, class V>
void AVLTree<K, V>::remove(Node*& subtree, const K& key)
{
    if (subtree == NULL)
        return;

    if (key < subtree->key) {
        remove(subtree->left, key);
    } else if (key > subtree->key) {
        remove(subtree->right, key);
    } else {
        if (subtree->left == NULL && subtree->right == NULL) {
            /* no-child remove */
            // your code here
            delete subtree;
            subtree = NULL;
        } else if (subtree->left != NULL && subtree->right != NULL) {
            /* two-child remove */
            // your code here
            Node* newsubtree = subtree->left;
            Node* temp = subtree;
            while(newsubtree->right != NULL){
                temp=newsubtree;
                newsubtree=newsubtree->right;
              }
            newsubtree->right = subtree->right;
            temp->right = newsubtree->left;
            if(temp!=subtree){
              newsubtree->left = subtree->left;
            }
            delete subtree;
            subtree = newsubtree;




        } else {
            /* one-child remove */
            // your code here
            Node* temp = subtree;
            if(subtree->left!=NULL){
              subtree = subtree->left;
            }
            else{
              subtree = subtree->right;
            }
            delete temp;
        }
        // your code here
    }
    rebalance(subtree);
}
