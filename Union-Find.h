//
// Created by Tom Guy on 1/1/2022.
//
#include <cstdio>
#include <exception>

#ifndef GROUP_CPP_UNION_FIND_H
#define GROUP_CPP_UNION_FIND_H

template <class T>
class Union_Find
{
    class Node
    {
        T* object;
        int size_of_group = 1;
        Node* father = nullptr;
        friend class Union_Find;
        void resetNode();
        ~Node();
    };
    Node** objects;
    int size = 0;
    Node* find_aux(int item);

public:
    explicit Union_Find(int size);
    T* find(int item);
    void Union(int item_1, int item_2);
    Union_Find(const Union_Find& other) = delete;
    Union_Find& operator=(const Union_Find& other) = delete;
    ~Union_Find();
};

template<class T>
void Union_Find<T>::Node::resetNode()
{
    object = nullptr;
    size_of_group = 0;
    father = nullptr;
}

template<class T>
Union_Find<T>::Node::~Node()
{
    delete object;
    resetNode();
}

template<class T>
Union_Find<T>::Union_Find(int size)
{
    if (size<=0)
        throw std::exception();
    objects = new Node*[size];
    this->size = size;
    for(int i=0; i<size; i++)
    {
        objects[i] = new Node;
        objects[i]->father = nullptr;
        objects[i]->size_of_group = 1;
        objects[i]->object = new T;
    }
}

template<class T>
void Union_Find<T>::Union(int item_1, int item_2)
{

    Node* item_1_location = find_aux(item_1);
    Node* item_2_location = find_aux(item_2);
    if(item_1_location->object==item_2_location->object) {
        return;
    }
    if(item_1_location->size_of_group <= item_2_location->size_of_group)
    {
        merge(item_1_location->object, item_2_location->object); //merge is T method
        item_1_location->father = item_2_location;
        item_2_location->size_of_group += item_1_location->size_of_group;
    }
    else
    {
        merge(item_2_location->object, item_1_location->object);
        item_2_location->father = item_1_location;
        item_1_location->size_of_group += item_2_location->size_of_group;
    }
}

template<class T>
T *Union_Find<T>::find(int item)
{
    return find_aux(item)->object;
}

template<class T>
typename Union_Find<T>::Node *Union_Find<T>::find_aux(int item)
{
    Node* location = objects[item];
    if(!location)
        throw std::exception();
    Node* root;
    Node* temp = location;
    while(temp->father)
    {
        temp = temp->father;
    }
    root = temp;
    while(location->father)
    {
        temp = location->father;
        location->father = root;
        location = temp;
    }
    return root;
}

template<class T>
Union_Find<T>::~Union_Find()
{
    for(int i=0; i<size; i++)
    {
        delete objects[i];
    }
    delete[] objects;
}

#endif //GROUP_CPP_UNION_FIND_H
