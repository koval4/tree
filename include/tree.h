#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include <type_traits>
#include <utility>

namespace detail {
    template <typename T, bool = std::is_copy_constructible_v<T>, bool = std::is_move_constructible_v<T>>
    struct enable_special_members;

    template <typename T>
    struct enable_special_members<T, true, true> {
        enable_special_members() noexcept = default;

        enable_special_members(const enable_special_members&) noexcept = default;
        enable_special_members(enable_special_members&&) noexcept = default;
    };

    template <typename T>
    struct enable_special_members<T, false, true> {
        enable_special_members() noexcept = default;

        enable_special_members(const enable_special_members&) noexcept = delete;
        enable_special_members(enable_special_members&&) noexcept = default;
    };

    template <typename T>
    struct enable_special_members<T, true, false> {
        enable_special_members() noexcept = default;

        enable_special_members(const enable_special_members&) noexcept = default;
        enable_special_members(enable_special_members&&) noexcept = delete;
    };

    template <typename T>
    struct enable_special_members<T, false, false> {
        enable_special_members() noexcept = default;

        enable_special_members(const enable_special_members&) noexcept = delete;
        enable_special_members(enable_special_members&&) noexcept = delete;
    };
}

template <typename T>
struct tree_node_impl {
    tree_node_impl* parent;
    tree_node_impl* left;
    tree_node_impl* right;
    T value;

    template <typename U = T,
              std::enable_if_t<
                  !std::is_same_v<tree_node_impl<T>, std::decay_t<U>> &&
                  !std::is_convertible_v<U, T> &&
                  std::is_constructible_v<T, U&&>, int> = 0>
    explicit tree_node_impl(U&& value) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : parent{nullptr}
        , left{nullptr}
        , right{nullptr}
        , value{std::forward<U>(value)} {}

    template <typename U = T,
              std::enable_if_t<
                  !std::is_same_v<tree_node_impl<T>, std::decay_t<U>> &&
                  std::is_convertible_v<U, T> &&
                  std::is_constructible_v<T, U&&>, int> = 0>
    tree_node_impl(U&& value) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : parent{nullptr}
        , left{nullptr}
        , right{nullptr}
        , value{std::forward<U>(value)} {}

    template <typename U = T,
              std::enable_if_t<
                  !std::is_same_v<tree_node_impl<T>, std::decay_t<U>> &&
                  std::is_constructible_v<T, U&&>, int> = 0>
    tree_node_impl(U&& value,
              tree_node_impl<T>* parent,
              tree_node_impl<T>* left,
              tree_node_impl<T>* right) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : parent{parent}
        , left{left}
        , right{right}
        , value{std::forward<U>(value)} {}

    tree_node_impl(const tree_node_impl& other) noexcept(std::is_nothrow_copy_constructible_v<T>)
        : parent{other.parent}
        , left{other.parent}
        , right{other.parent}
        , value{other.value} {}

    tree_node_impl(tree_node_impl&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
        : parent{other.parent}
        , left{other.left}
        , right{other.right}
        , value{std::move(value)} {}
};

template <typename T>
struct tree_node : tree_node_impl<T>, private detail::enable_special_members<T> {
    template <typename U = T,
              std::enable_if_t<
                  !std::is_same_v<tree_node<T>, std::decay_t<U>> &&
                  !std::is_convertible_v<U, T> &&
                  std::is_constructible_v<T, U&&>, int> = 0>
    explicit tree_node(U&& value) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : tree_node_impl<T>{std::forward<U>(value)} {}

    template <typename U = T,
              std::enable_if_t<
                  !std::is_same_v<tree_node<T>, std::decay_t<U>> &&
                  std::is_convertible_v<U, T> &&
                  std::is_constructible_v<T, U&&>, int> = 0>
    tree_node(U&& value) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : tree_node_impl<T>{std::forward<U>(value)} {}

    template <typename U = T,
              std::enable_if_t<
                  !std::is_same_v<tree_node<T>, std::decay_t<U>> &&
                  std::is_constructible_v<T, U&&>, int> = 0>
    tree_node(U&& value,
              tree_node<T>* parent,
              tree_node<T>* left,
              tree_node<T>* right) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : tree_node_impl<T>{std::forward<U>(value), parent, left, right} {}
};

#endif // TREE_H_INCLUDED
