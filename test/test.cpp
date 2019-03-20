#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "tree.h"
#include <vector>
#include <array>

struct no_copy {
    no_copy() = default;
    no_copy(const no_copy&) = delete;
    no_copy(no_copy&&) = default;

    int dummy;
};

struct no_move {
    no_move() = default;
    no_move(const no_move&) = default;
    no_move(no_move&&) = delete;

    int dummy;
};

TEST_CASE("Tree node constructed", "[tree_node]") {
    // compile-time checks
    static_assert(
        std::is_copy_constructible_v<tree_node<int>>,
        "tree_node should propagate copy constructor presence");
    static_assert(
        std::is_move_constructible_v<tree_node<int>>,
        "tree_node should propagate move constructor presence");

    static_assert(
        !std::is_copy_constructible_v<tree_node<no_copy>>,
        "tree_node should propagate copy constructor presence");
    static_assert(
        std::is_move_constructible_v<tree_node<no_copy>>,
        "tree_node should propagate move constructor presence");

    static_assert(
        std::is_copy_constructible_v<tree_node<no_move>>,
        "tree_node should propagate copy constructor presence");
    // Types without a move constructor, but with a copy constructor that accepts const T& arguments, satisfy std::is_move_constructible.
    // TODO: test is broken, check how it could be fixed
    //static_assert(
    //    !std::is_move_constructible_v<tree_node<no_move>>,
    //    "tree_node should propagate move constructor presence");

    tree_node<int> node{1};
    REQUIRE(node.value() == 1);
    REQUIRE(node.parent() == nullptr);
    REQUIRE(node.prev_sibling() == nullptr);
    REQUIRE(node.next_sibling() == nullptr);
    REQUIRE(node.first_child() == nullptr);
    REQUIRE(node.last_child() == nullptr);

    tree_node<int> another_node{node};
    REQUIRE(another_node.value() == 1);
    REQUIRE(another_node.parent() == nullptr);
    REQUIRE(node.prev_sibling() == nullptr);
    REQUIRE(node.next_sibling() == nullptr);
    REQUIRE(node.first_child() == nullptr);
    REQUIRE(node.last_child() == nullptr);

    tree_node<int> other_node{2, &node, nullptr, nullptr, nullptr, nullptr};
    REQUIRE(other_node.parent() == &node);
    REQUIRE(node.prev_sibling() == nullptr);
    REQUIRE(node.next_sibling() == nullptr);
    REQUIRE(node.first_child() == nullptr);
    REQUIRE(node.last_child() == nullptr);

    tree_node<std::vector<int>> vec_node{std::vector{1, 2, 3}, nullptr, nullptr, nullptr, nullptr, nullptr};
    REQUIRE(vec_node.value()[0] == 1);
    REQUIRE(vec_node.value()[1] == 2);
    REQUIRE(vec_node.value()[2] == 3);
    REQUIRE(vec_node.parent() == nullptr);
    REQUIRE(node.prev_sibling() == nullptr);
    REQUIRE(node.next_sibling() == nullptr);
    REQUIRE(node.first_child() == nullptr);
    REQUIRE(node.last_child() == nullptr);
}

TEST_CASE("tree_node are pushed as childs to back", "[tree_node::push_back_child]") {
    tree_node<int> root{1};
    tree_node<int> child1{2};
    tree_node<int> child2{3};
    tree_node<int> child3{4};

    root.push_back_child(&child1);
    REQUIRE(root.first_child() == &child1);
    REQUIRE(root.last_child() == &child1);
    REQUIRE(child1.parent() == &root);
    REQUIRE(child1.prev_sibling() == nullptr);
    REQUIRE(child1.next_sibling() == nullptr);

    root.push_back_child(&child2);
    REQUIRE(root.first_child() == &child1);
    REQUIRE(root.last_child() == &child2);
    REQUIRE(child1.parent() == &root);
    REQUIRE(child2.parent() == &root);
    REQUIRE(child1.prev_sibling() == nullptr);
    REQUIRE(child1.next_sibling() == &child2);
    REQUIRE(child2.prev_sibling() == &child1);
    REQUIRE(child2.next_sibling() == nullptr);

    root.push_back_child(&child3);
    REQUIRE(root.first_child() == &child1);
    REQUIRE(root.last_child() == &child3);
    REQUIRE(child1.parent() == &root);
    REQUIRE(child2.parent() == &root);
    REQUIRE(child3.parent() == &root);
    REQUIRE(child1.prev_sibling() == nullptr);
    REQUIRE(child1.next_sibling() == &child2);
    REQUIRE(child2.prev_sibling() == &child1);
    REQUIRE(child2.next_sibling() == &child3);
    REQUIRE(child3.prev_sibling() == &child2);
    REQUIRE(child3.next_sibling() == nullptr);
}

TEST_CASE("tree_node are pushed as childs to front", "[tree_node::push_front_child]") {
    tree_node<int> root{1};
    tree_node<int> child1{2};
    tree_node<int> child2{3};
    tree_node<int> child3{4};

    root.push_front_child(&child1);
    REQUIRE(root.first_child() == &child1);
    REQUIRE(root.last_child() == &child1);
    REQUIRE(child1.parent() == &root);
    REQUIRE(child1.prev_sibling() == nullptr);
    REQUIRE(child1.next_sibling() == nullptr);

    root.push_front_child(&child2);
    REQUIRE(root.first_child() == &child2);
    REQUIRE(root.last_child() == &child1);
    REQUIRE(child1.parent() == &root);
    REQUIRE(child2.parent() == &root);
    REQUIRE(child1.prev_sibling() == &child2);
    REQUIRE(child1.next_sibling() == nullptr);
    REQUIRE(child2.prev_sibling() == nullptr);
    REQUIRE(child2.next_sibling() == &child1);

    root.push_front_child(&child3);
    REQUIRE(root.first_child() == &child3);
    REQUIRE(root.last_child() == &child1);
    REQUIRE(child1.parent() == &root);
    REQUIRE(child2.parent() == &root);
    REQUIRE(child3.parent() == &root);
    REQUIRE(child1.prev_sibling() == &child2);
    REQUIRE(child1.next_sibling() == nullptr);
    REQUIRE(child2.prev_sibling() == &child3);
    REQUIRE(child2.next_sibling() == &child1);
    REQUIRE(child3.prev_sibling() == nullptr);
    REQUIRE(child3.next_sibling() == &child2);
}

TEST_CASE("tree_node are unlinked from parent", "[tree_node::unlink_child]") {
    tree_node<int> root{1};
    tree_node<int> child1{2};
    tree_node<int> child2{3};
    tree_node<int> child3{4};

    root.push_back_child(&child1);
    root.push_back_child(&child2);
    root.push_back_child(&child3);

    root.unlink_child(&child1);
    REQUIRE(root.first_child() == &child2);
    REQUIRE(root.last_child() == &child3);
    REQUIRE(child1.parent() == nullptr);
    REQUIRE(child1.prev_sibling() == nullptr);
    REQUIRE(child1.next_sibling() == nullptr);
    REQUIRE(child2.prev_sibling() == nullptr);
    REQUIRE(child2.next_sibling() == &child3);

    root.unlink_child(&child2);
    REQUIRE(root.first_child() == &child3);
    REQUIRE(root.last_child() == &child3);
    REQUIRE(child2.parent() == nullptr);
    REQUIRE(child2.prev_sibling() == nullptr);
    REQUIRE(child2.next_sibling() == nullptr);
    REQUIRE(child3.prev_sibling() == nullptr);
    REQUIRE(child3.next_sibling() == nullptr);

    root.unlink_child(&child3);
    REQUIRE(root.first_child() == nullptr);
    REQUIRE(root.last_child() == nullptr);
    REQUIRE(child3.parent() == nullptr);

    root.push_back_child(&child1);
    root.push_back_child(&child2);
    root.push_back_child(&child3);

    root.unlink_child(&child3);
    REQUIRE(root.first_child() == &child1);
    REQUIRE(root.last_child() == &child2);
    REQUIRE(child3.parent() == nullptr);
    REQUIRE(child2.prev_sibling() == &child1);
    REQUIRE(child2.next_sibling() == nullptr);
    REQUIRE(child3.prev_sibling() == nullptr);
    REQUIRE(child3.next_sibling() == nullptr);

    root.unlink_child(&child2);
    REQUIRE(root.first_child() == &child1);
    REQUIRE(root.last_child() == &child1);
    REQUIRE(child2.parent() == nullptr);
    REQUIRE(child1.prev_sibling() == nullptr);
    REQUIRE(child1.next_sibling() == nullptr);

    root.unlink_child(&child1);
    REQUIRE(root.first_child() == nullptr);
    REQUIRE(root.last_child() == nullptr);
    REQUIRE(child1.parent() == nullptr);

    root.push_back_child(&child1);
    root.push_back_child(&child2);
    root.push_back_child(&child3);

    root.unlink_child(&child2);
    REQUIRE(root.first_child() == &child1);
    REQUIRE(root.last_child() == &child3);
    REQUIRE(child2.parent() == nullptr);
    REQUIRE(child1.prev_sibling() == nullptr);
    REQUIRE(child1.next_sibling() == &child3);
    REQUIRE(child3.prev_sibling() == &child1);
    REQUIRE(child3.next_sibling() == nullptr);

    root.unlink_child(&child1);
    REQUIRE(root.first_child() == &child3);
    REQUIRE(root.last_child() == &child3);
    REQUIRE(child1.parent() == nullptr);
    REQUIRE(child3.prev_sibling() == nullptr);
    REQUIRE(child3.next_sibling() == nullptr);

    root.unlink_child(&child3);
    REQUIRE(root.first_child() == nullptr);
    REQUIRE(root.last_child() == nullptr);
    REQUIRE(child3.parent() == nullptr);
}

TEST_CASE("pre_order_iterator iterates over nodes", "[pre_order_iterator]") {
    tree_node<int> root{1};
    tree_node<int> child1{2};
    tree_node<int> child2{3};
    tree_node<int> child3{4};

    tree_node<int> grandchild_1_1{5};
    tree_node<int> grandchild_1_2{6};
    tree_node<int> grandchild_1_3{7};

    tree_node<int> grandchild_2_1{8};
    tree_node<int> grandchild_2_2{9};

    tree_node<int> grandchild_3_1{10};

    root.push_back_child(&child1);
    root.push_back_child(&child2);
    root.push_back_child(&child3);

    child1.push_back_child(&grandchild_1_1);
    child1.push_back_child(&grandchild_1_2);
    child1.push_back_child(&grandchild_1_3);

    child2.push_back_child(&grandchild_2_1);
    child2.push_back_child(&grandchild_2_2);

    child3.push_back_child(&grandchild_3_1);

    std::array<int, 10> required_order = {1, 2, 5, 6, 7, 3, 8, 9, 4, 10};

    {
        pre_order_iterator<int> it{&root};
        for (size_t i = 0; i < 10; i++) {
            REQUIRE(*it == required_order[i]);
            it++;
        }
    }

    {
        pre_order_iterator<int> it{&root};
        for (size_t i = 0; i < 10; i++) {
            REQUIRE(*it == required_order[i]);
            ++it;
        }
    }
}

TEST_CASE("Tree constructed", "[tree]") {
    // compile-time checks
    static_assert(
        std::is_copy_constructible_v<tree<int>>,
        "tree should propagate copy constructor presence");
    static_assert(
        std::is_move_constructible_v<tree<int>>,
        "tree should propagate move constructor presence");

    static_assert(
        !std::is_copy_constructible_v<tree<no_copy>>,
        "tree should propagate copy constructor presence");
    static_assert(
        std::is_move_constructible_v<tree<no_copy>>,
        "tree should propagate move constructor presence");

    static_assert(
        std::is_copy_constructible_v<tree<no_move>>,
        "tree should propagate copy constructor presence");

    tree<int> _1;
    REQUIRE(_1.empty());
    REQUIRE(_1.size() == 0);
}
