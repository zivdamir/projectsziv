#include <iostream>
#include <cassert>
#include <cmath>
#ifndef HEAP_LIBRARY_H
#define HEAP_LIBRARY_H


template<typename T>
class Heap {
private:
    friend class Node;

    class Node {
    private:
        friend class Heap;

        Node *father;
        Node *l_son;
        Node *r_son;
        T *data;
    public:
        Node(Node *father = nullptr, Node *lSon = nullptr, Node *rSon = nullptr, T *data = nullptr) : father(father),
                                                                                                      l_son(lSon),
                                                                                                      r_son(rSon),
                                                                                                      data(data) {}

        ~Node() {

            if(this->father!= nullptr)
            {
                auto father_temp= this->father;
                if(this==father_temp->r_son)
                {
                    father_temp->r_son=nullptr;
                }
                else
                {
                    assert(father_temp->l_son==this);
                    father_temp->l_son=nullptr;
                }
            }
            l_son = r_son = father = nullptr;
            delete data;// might cause some issues
            data = nullptr;
        }

        Node(const Node &node) = default;

        Node &operator=(Node &node) = default;

        bool operator==(const Node &node);

        bool operator<=(const Node &node);

        bool operator>=(const Node &node);

        static void link(Node *father, Node *son, bool is_left);//could still be helpful

        friend void swap_between_nodes(Heap::Node *node1, Heap::Node *node2) {
            auto temp = node1->data;
            node1->data = node2->data;
            node2->data = temp;
        }


        //we want a way to compare both nodes. way to build and destroy,and a way to swap between them. pretty simple
    };

    Node *root;
    bool is_min;

    Node *look_for_node(int index);

    void sift_down(Node *node);

    void sift_up(Node *node);

    void PostOrderSiftDown(Node *node);

    void PostOrderDelete(Node *node);

    int *fromIntToBinary(int number) {
        assert(number > 0);
        int i;
        //std::cout << "the number is:" << number << std::endl;
        int size = (int) floor(log2(number)) + 1;
        int *array = new int[size]();
        for (i = size - 1; i >= 0; i--) {
            array[i] = number % 2;
            number = number / 2;
        }

        return array;
    }


    int size = 0;


    Node *next_available_leaf_father;//use for insertion.
    Node *current_available_leaf_father;//use for delmax.
    void swap_between_root_and_leaf();

    void delete_current_leaf();

    void update_current_and_next_leaf_father();

public:
    Heap(Node *root = nullptr, bool is_min = true, int size = 0) : root(root), is_min(is_min) {};

    ~Heap();
    friend T *HeapSort(Heap &heap){
        int count = 0;
        T* array= (T*)malloc(sizeof(T)*heap.size);
        if(!array)
        {
            return NULL;
        }

        int j=heap.size-1;
        int size_aux=heap.size;
        for (int i = 0; i < size_aux; ++i) {
            auto root_val=heap.find_extermum();
            array[i]=*root_val;
            heap.del_extermum();
        }
        return array;
    }

    void del_extermum();

    void insert(T *data);

    T *find_extermum();

    void changeKey(Node *vertex, T *value);//todo in min_heap , we always decrease, in max_heaps, we always increase.
};


template<typename T>
T *Heap<T>::find_extermum() {
    return this->root->data;
}


template<typename T>
void Heap<T>::sift_down(Heap::Node *node) {
    //heap assumption: a leaf is a heap
    if (!node->l_son && !node->r_son) {
        return;
    }
    //maybe in case when one of the sons is null ,just give it to the other one.
    auto minimal_between_two_sons=node,maximal_son=node;
    if(!node->r_son|| !node->l_son) //if both exist,will give false, so we're ok.
    {
        minimal_between_two_sons=maximal_son=(node->r_son==nullptr)?node->l_son:node->r_son;
    }
    else {
        minimal_between_two_sons = (*(node->l_son) <= *(node->r_son)) ? node->l_son : node->r_son;
        maximal_son = (*(node->l_son) >= *(node->r_son)) ? node->l_son : node->r_son;
    }
    switch (this->is_min) {
        case true:
            //min-heap- father's key is always the lesser from his 2 sons
            switch (*minimal_between_two_sons <= *node) {
                case true:
                    swap_between_nodes(minimal_between_two_sons, node);
                    sift_down(minimal_between_two_sons);
                case false:
                    return; //we've finished
            }

        case false:
            //max-heap- father's key is always the bigger from his 2 sons
            switch (*maximal_son >= *node) {
                case true:
                    swap_between_nodes(maximal_son, node);
                    sift_down(maximal_son);
                case false:
                    return; //we've finished
            }

    }

}

template<typename T>
void Heap<T>::sift_up(Heap::Node *node) { //done
    if (node->father == nullptr) // if we're hitting the root. discontinue.
    {
        return;
    }
    if (is_min) {
        //min-heap == father<=son
        if (*node->father >= *node) {
            swap_between_nodes(node->father, node);
            sift_up(node->father);
        } else return; // heap is assumed.
    } else {
        if (*node->father <= *node) //max-heap == father>=son
        {
            swap_between_nodes(node->father, node);
            sift_up(node->father);
        } else return;

    }
}

template<typename T>
void Heap<T>::PostOrderSiftDown(Heap::Node *node) {
    if (!node) return;
    PostOrderSiftDown(node->l_son);
    PostOrderSiftDown(node->r_son);
    sift_down(node);
}

template<typename T>
void Heap<T>::PostOrderDelete(Heap::Node *node) {
    if (!node) return;
    PostOrderDelete(node->l_son);
    PostOrderDelete(node->r_son);
    delete node;
}

template<typename T>
Heap<T>::~Heap() {
    PostOrderDelete(root);
}

template<typename T>
void Heap<T>::insert(T *data) {
    assert(size >= 0);
    if (size == 0) {
        root = new Node(nullptr, nullptr, nullptr, data);
        next_available_leaf_father = root;
        current_available_leaf_father = root;
        //tzarih leithashev be mikre ha katze bo size==1 be remove;
        size++;
    } else // size>0
    {
        if (!next_available_leaf_father->l_son) {
            next_available_leaf_father->l_son = new Node(next_available_leaf_father, nullptr, nullptr, data);
            current_available_leaf_father = next_available_leaf_father;
            size++;
            sift_up(current_available_leaf_father->l_son);
            next_available_leaf_father = look_for_node((int) floor(double((size + 1) / 2)));
            return;
        } else {
            if (!next_available_leaf_father->r_son) {
                next_available_leaf_father->r_son = new Node(next_available_leaf_father, nullptr, nullptr, data);
                size++;
                current_available_leaf_father = next_available_leaf_father;
                sift_up(current_available_leaf_father->r_son);
                next_available_leaf_father = look_for_node((int) floor(double((size + 1) / 2)));
                return;
            }

        }
    }

}

template<typename T>

void Heap<T>::del_extermum() {
    if (size == 0) {
        std::cout << "heap is empty" << std::endl;
        return;
    }
    T *data_aux = root->data;
    if (size == 1) {
        auto temp=root;
        root=nullptr;
        delete temp;
        update_current_and_next_leaf_father();
        size = 0;
        return;
    }
    swap_between_root_and_leaf();
    delete_current_leaf();
    sift_down(root);
    size--;
    update_current_and_next_leaf_father();
}



template<typename T>
void Heap<T>::changeKey(Heap::Node *vertex, T *value) {
  // f
}

template<typename T>
typename Heap<T>::Node *Heap<T>::look_for_node(int index) {
    //convention - always start from 1
    if (index < 1) return nullptr; //maybe index > size, we need to check that..
    if (index == 1) return root;
    else {
        int *array = fromIntToBinary(index);
        int size_aux = (int) floor(log2(index)) + 1;
        auto temp = root;
        for (int i = 1; i < size_aux; i++) {
            switch (array[i]) {
                case 1:
                    //go right
                    temp = temp->r_son;
                    continue;
                case 0:
                    temp = temp->l_son;
                    continue;
            }
        }
        delete[] array;
        return temp;
    }
}

template<typename T>
void Heap<T>::swap_between_root_and_leaf() {
    auto leaf_father = current_available_leaf_father;
    assert(leaf_father != nullptr);
    //we always prioritize the right leaf,before the left leaf
    if (!leaf_father->r_son) {
        assert(leaf_father->l_son);
        swap_between_nodes(root, leaf_father->l_son);
        return;
    }
    swap_between_nodes(root, leaf_father->r_son);
}

template<typename T>
void Heap<T>::delete_current_leaf() {
    assert(current_available_leaf_father);
    if (!current_available_leaf_father->r_son) {

        assert(current_available_leaf_father->l_son);
        delete current_available_leaf_father->l_son;
    } else {
        delete current_available_leaf_father->r_son;
    }
}

template<typename T>
void Heap<T>::update_current_and_next_leaf_father() {
    current_available_leaf_father = look_for_node(floor(double(size / 2)));
    next_available_leaf_father = look_for_node(floor((double) ((size + 1) / 2)));
}


template<typename T>
bool Heap<T>::Node::operator==(const Heap::Node &node) {
    if (node.data && this->data) return this->data == node.data;
    else return false;
}

template<typename T>
bool Heap<T>::Node::operator>=(const Heap::Node &node) {
    // null>= 1 ->false

    // 1>= null true
    // a>=b depends.
    if (!this->data && node.data) {
        return false;
    }
    if (this->data && !node.data) {
        return true;
    }
    if (this->data && node.data) {
        return *(this->data) >= *(node.data);
    }
}

template<typename T>
bool Heap<T>::Node::operator<=(const Heap::Node &node) {
    if (!this->data && node.data) {
        return false;
    }
    if (this->data && !node.data) {
        return true;
    }
    if (this->data && node.data) {
        return *(this->data) <= *(node.data);
    }
}

template<typename T>
void Heap<T>::Node::link(Heap::Node *father, Heap::Node *son, bool is_left) {
    if (father == son) {
        return;
    }
    if (!father && !son) {
        std::cout << "lol two nullptrs" << std::endl;
        return;
    }
    if (father == nullptr && son != nullptr) {
        son->father = nullptr;
        return;
    }
    switch (is_left) {
        case true:
            switch (son == nullptr) {
                case true:
                    father->l_son = nullptr;
                    return;
                case false:
                    father->l_son = son;
                    son->father = father;
                    return;
            }
        case false:
            switch (son == nullptr) {
                case true:
                    father->r_son = nullptr;
                    return;
                case false:
                    father->r_son = son;
                    son->father = father;
                    return;
            }
    }

}


#endif //HEAP_LIBRARY_H
//if(node1->father==node2)
//    {
//        //node2 is node1's father..
//        auto node1_l_temp=node1->l_son;
//        auto node1_r_temp=node1->r_son;
//        auto node2_father=node2->father;
//        auto node2_r_son=node2->r_son;
//        auto node2_l_son=node2->l_son;
//        switch(node1)
//        {
//            case(node2->l_son):
//                node2->r_son=node1_r_temp;
//                node2->l_son=node1_l_temp;
//                node2->father=node1;
//                node1->l_son=node2;
//                node1->r_son=node2_r_son;
//                node1->father=node2_father;
//            case(node2->r_son):
//                node2->r_son=node1_r_temp;
//                node2->l_son=node1_l_temp;
//                node2->father=node1;
//                node1->r_son=node2;
//                node1->l_son=node2_l_son;
//                node1->father=node2_father;
//        }
//        link(node2,node2->r_son);
//        link(node2,node2->l_son);
//        link(node2->father,node2);
//        link(node1,node1->r_son);
//        link(node1,node1->l_son);
//        link(node1->father,node1);
//        return;
//    }
//    if(node2->father==node1)
//    {
//        swap_between_nodes<T>(node2,node1);
//    }
//    auto l_temp = node1->l_son;
//    auto r_temp = node1->r_son;
//    auto father_temp = node1->father;
//    if (node2->father != nullptr) {
//        switch (node2) {
//            case node2->father->l_son:
//                link(node2->father, node1, true);
//            case node2->father->r_son:
//                link(node2->father, node1, false);
//        }
//    }
//    link(node1, node2->l_son, true);
//    link(node1, node2->r_son, false);
//    link(node2, l_temp, true);
//    link(node2, r_temp, false);
//    if (father_temp != nullptr) {
//        link(father_temp, node2, (father_temp->l_son == node1));
//    } else {
//        node2->father = nullptr;
//    }
