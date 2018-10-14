#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

template <typename T>
struct tree_node {
    tree_node* parent;
    tree_node* left;
    tree_node* right;
    T value;

    tree_node(T value)
        : parent{nullptr}
        , left{nullptr}
        , right{nullptr}
        , value{value} {};
};

#endif // TREE_H_INCLUDED
