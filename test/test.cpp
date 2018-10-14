#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "tree.h"

TEST_CASE("Tree node constructed", "[tree_node]") {
    tree_node<int> node{1};
    REQUIRE(node.value == 1);
    REQUIRE(node.parent == nullptr);
    REQUIRE(node.left == nullptr);
    REQUIRE(node.right == nullptr);
}
