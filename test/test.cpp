#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "tree.h"
#include <vector>

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
