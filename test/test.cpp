#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "tree.h"
#include <vector>
#include <array>
#include <algorithm>

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
        pre_order_iterator<int> it{&root, nullptr};
        for (size_t i = 0; i < 10; i++) {
            REQUIRE(*it == required_order[i]);
            it++;
        }
    }

    {
        pre_order_iterator<int> it{&root, nullptr};
        for (size_t i = 0; i < 10; i++) {
            REQUIRE(*it == required_order[i]);
            ++it;
        }
    }

    {
        pre_order_iterator<int> it{nullptr, &grandchild_3_1};
        for (size_t i = 0; i < 10; i++) {
            --it;
            REQUIRE(*it == required_order[required_order.size() - 1 - i]);
        }
    }

    {
        pre_order_iterator<int> it{nullptr, &grandchild_3_1};
        for (size_t i = 0; i < 10; i++) {
            it--;
            REQUIRE(*it == required_order[required_order.size() - 1 - i]);
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

TEST_CASE("Nodes are inserted into tree", "[tree::insert]") {
    tree<int> _1;
    pre_order_view view{_1};

    {
        _1.insert(insertion::vert, std::begin(view), 1);
        REQUIRE(_1.size() == 1);
        REQUIRE(view.front() == 1);
    }

    {
        _1.insert(insertion::vert, std::end(view), 2);
        REQUIRE(_1.size() == 2);
        REQUIRE(view.back() == 2);
    }

    {
        _1.insert(insertion::hor, std::end(view), 3);
        REQUIRE(_1.size() == 3);
        REQUIRE(view.back() == 3);
    }

    {
        _1.insert(insertion::hor, std::end(view), 4);
        REQUIRE(_1.size() == 4);
        REQUIRE(view.back() == 4);
    }

    {
        auto it = std::find(std::begin(view), std::end(view), 2);
        _1.insert(insertion::vert, it, 5);

        std::array required_order = {1, 5, 2, 3, 4};
        REQUIRE(view.size() == 5);
        REQUIRE(std::equal(std::begin(view), std::end(view), std::begin(required_order)));
    }

    {
        auto it = std::find(std::begin(view), std::end(view), 2);
        _1.insert(insertion::hor, it, 6);

        std::array required_order = {1, 5, 6, 2, 3, 4};
        REQUIRE(view.size() == 6);
        REQUIRE(std::equal(std::begin(view), std::end(view), std::begin(required_order)));
    }

    {
        auto it = std::find(std::begin(view), std::end(view), 2);
        _1.insert(insertion::hor, it, 7);

        std::array required_order = {1, 5, 6, 7, 2, 3, 4};
        REQUIRE(view.size() == 7);
        REQUIRE(std::equal(std::begin(view), std::end(view), std::begin(required_order)));
    }

    {
        auto it = std::find(std::begin(view), std::end(view), 3);
        _1.insert(insertion::vert, it, 8);

        std::array required_order = {1, 5, 6, 7, 2, 8, 3, 4};
        REQUIRE(view.size() == 8);
        REQUIRE(std::equal(std::begin(view), std::end(view), std::begin(required_order)));
    }

    {
        auto it = std::find(std::begin(view), std::end(view), 3);
        _1.insert(insertion::hor, it, 9);

        std::array required_order = {1, 5, 6, 7, 2, 8, 9, 3, 4};
        REQUIRE(view.size() == 9);
        REQUIRE(std::equal(std::begin(view), std::end(view), std::begin(required_order)));
    }

    {
        auto it = std::find(std::begin(view), std::end(view), 4);
        _1.insert(insertion::vert, it, 10);

        std::array required_order = {1, 5, 6, 7, 2, 8, 9, 3, 10, 4};
        REQUIRE(view.size() == 10);
        REQUIRE(std::equal(std::begin(view), std::end(view), std::begin(required_order)));
    }

    {
        auto it = std::find(std::begin(view), std::end(view), 8);
        _1.insert(insertion::vert, it, 11);

        std::array required_order = {1, 5, 6, 7, 2, 11, 8, 9, 3, 10, 4};
        REQUIRE(view.size() == 11);
        REQUIRE(std::equal(std::begin(view), std::end(view), std::begin(required_order)));
    }
}

TEST_CASE("Nodes are appended and prepended to tree", "[tree::append, tree::prepend]") {
    tree<int> _1;
    pre_order_view view{_1};
    _1.insert(insertion::vert, std::begin(view), 1);

    {
        _1.append_child(std::begin(view), 2);

        std::array required_order = {1, 2};
        REQUIRE(_1.size() == 2);
        REQUIRE(std::equal(std::begin(view), std::end(view), std::begin(required_order)));
    }

    {
        _1.append_child(std::begin(view), 3);

        std::array required_order = {1, 2, 3};
        REQUIRE(_1.size() == 3);
        REQUIRE(std::equal(std::begin(view), std::end(view), std::begin(required_order)));
    }

    {
        _1.append_child(std::begin(view), 4);

        std::array required_order = {1, 2, 3, 4};
        REQUIRE(_1.size() == 4);
        REQUIRE(std::equal(std::begin(view), std::end(view), std::begin(required_order)));
    }

    {
        auto it = std::find(std::begin(view), std::end(view), 2);
        _1.prepend_child(it, 5);

        std::array required_order = {1, 2, 5, 3, 4};
        REQUIRE(_1.size() == 5);
        REQUIRE(std::equal(std::begin(view), std::end(view), std::begin(required_order)));
    }

    {
        auto it = std::find(std::begin(view), std::end(view), 2);
        _1.prepend_child(it, 6);

        std::array required_order = {1, 2, 6, 5, 3, 4};
        REQUIRE(_1.size() == 6);
        REQUIRE(std::equal(std::begin(view), std::end(view), std::begin(required_order)));
    }

    {
        auto it = std::find(std::begin(view), std::end(view), 2);
        _1.prepend_child(it, 7);

        std::array required_order = {1, 2, 7, 6, 5, 3, 4};
        REQUIRE(_1.size() == 7);
        REQUIRE(std::equal(std::begin(view), std::end(view), std::begin(required_order)));
    }

    {
        auto it = std::find(std::begin(view), std::end(view), 3);
        _1.prepend_child(it, 8);

        std::array required_order = {1, 2, 7, 6, 5, 3, 8, 4};
        REQUIRE(_1.size() == 8);
        REQUIRE(std::equal(std::begin(view), std::end(view), std::begin(required_order)));
    }

    {
        auto it = std::find(std::begin(view), std::end(view), 3);
        _1.append_child(it, 9);

        std::array required_order = {1, 2, 7, 6, 5, 3, 8, 9, 4};
        REQUIRE(_1.size() == 9);
        REQUIRE(std::equal(std::begin(view), std::end(view), std::begin(required_order)));
    }

    {
        auto it = std::find(std::begin(view), std::end(view), 4);
        _1.append_child(it, 10);

        std::array required_order = {1, 2, 7, 6, 5, 3, 8, 9, 4, 10};
        REQUIRE(_1.size() == 10);
        REQUIRE(std::equal(std::begin(view), std::end(view), std::begin(required_order)));
    }
}
