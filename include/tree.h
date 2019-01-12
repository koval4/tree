#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include <type_traits>
#include <utility>
#include <memory>
#include <cassert>

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
    tree_node_impl* prev_sibling;
    tree_node_impl* next_sibling;
    tree_node_impl* first_child;
    tree_node_impl* last_child;
    T value;

    template <typename U = T,
              std::enable_if_t<
                  !std::is_same_v<tree_node_impl<T>, std::decay_t<U>> &&
                  !std::is_convertible_v<U, T> &&
                  std::is_constructible_v<T, U&&>, int> = 0>
    explicit tree_node_impl(U&& value) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : parent{nullptr}
        , prev_sibling{nullptr}
        , next_sibling{nullptr}
        , first_child{nullptr}
        , last_child{nullptr}
        , value{std::forward<U>(value)} {}

    template <typename U = T,
              std::enable_if_t<
                  !std::is_same_v<tree_node_impl<T>, std::decay_t<U>> &&
                  std::is_convertible_v<U, T> &&
                  std::is_constructible_v<T, U&&>, int> = 0>
    tree_node_impl(U&& value) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : parent{nullptr}
        , prev_sibling{nullptr}
        , next_sibling{nullptr}
        , first_child{nullptr}
        , last_child{nullptr}
        , value{std::forward<U>(value)} {}

    template <typename U = T,
              std::enable_if_t<
                  !std::is_same_v<tree_node_impl<T>, std::decay_t<U>> &&
                  std::is_constructible_v<T, U&&>, int> = 0>
    tree_node_impl(U&& value,
                   tree_node_impl<T>* parent,
                   tree_node_impl<T>* prev_sibling,
                   tree_node_impl<T>* next_sibling,
                   tree_node_impl<T>* first_child,
                   tree_node_impl<T>* last_child) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : parent{parent}
        , prev_sibling{prev_sibling}
        , next_sibling{next_sibling}
        , first_child{first_child}
        , last_child{last_child}
        , value{std::forward<U>(value)} {}

    tree_node_impl(const tree_node_impl& other) noexcept(std::is_nothrow_copy_constructible_v<T>)
        : parent{other.parent}
        , prev_sibling{other.prev_sibling}
        , next_sibling{other.next_sibling}
        , first_child{other.first_child}
        , last_child{other.last_child}
        , value{other.value} {}

    tree_node_impl(tree_node_impl&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
        : parent{other.parent}
        , prev_sibling{other.prev_sibling}
        , next_sibling{other.next_sibling}
        , first_child{other.first_child}
        , last_child{other.last_child}
        , value{std::move(value)} {}
};

template <typename T>
struct tree_node : private tree_node_impl<T>, private detail::enable_special_members<T> {
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
              tree_node<T>* prev_sibling,
              tree_node<T>* next_sibling,
              tree_node<T>* first_child,
              tree_node<T>* last_child) noexcept(std::is_nothrow_constructible_v<T, U&&>)
        : tree_node_impl<T>{
            std::forward<U>(value),
            parent,
            prev_sibling,
            next_sibling,
            first_child,
            last_child
        } {}

    tree_node* prev_sibling() const noexcept {
        return reinterpret_cast<tree_node*>(tree_node_impl<T>::prev_sibling);
    }

    tree_node* next_sibling() const noexcept {
        return reinterpret_cast<tree_node*>(tree_node_impl<T>::next_sibling);
    }

    tree_node* first_child() const noexcept {
        return reinterpret_cast<tree_node*>(tree_node_impl<T>::first_child);
    }

    tree_node* last_child() const noexcept {
        return reinterpret_cast<tree_node*>(tree_node_impl<T>::last_child);
    }

    tree_node* parent() const noexcept {
        return reinterpret_cast<tree_node*>(tree_node_impl<T>::parent);
    }

    T& value() noexcept {
        return tree_node_impl<T>::value;
    }

    const T& value() const noexcept {
        return tree_node_impl<T>::value;
    }
};

template <typename T, typename Allocator = std::allocator<tree_node<T>>>
struct tree_storage {
    using allocator_traits = std::allocator_traits<Allocator>;

    tree_storage(Allocator alloc = Allocator{}) noexcept
        : alloc{std::move(alloc)}
        , root{nullptr} {}

    template <typename U,
              std::enable_if_t<
                  !std::is_same_v<tree_storage<T>, std::decay_t<U>> &&
                  !std::is_convertible_v<U, T> &&
                  std::is_constructible_v<tree_node<T>, U&&>, int> = 0>
    explicit tree_storage(U&& value, Allocator alloc = Allocator{}) noexcept(std::is_nothrow_constructible_v<tree_node<T>, U&&>)
        : alloc{std::move(alloc)}
        , root{create_node(this->alloc, std::forward<U>(value))} {}

    template <typename U,
              std::enable_if_t<
                  !std::is_same_v<tree_storage<T>, std::decay_t<U>> &&
                  std::is_convertible_v<U, T> &&
                  std::is_constructible_v<tree_node<T>, U&&>, int> = 0>
    tree_storage(U&& value, Allocator alloc = Allocator{}) noexcept(std::is_nothrow_constructible_v<tree_node<T>, U&&>)
        : alloc{std::move(alloc)}
        , root{create_node(this->alloc, std::forward<U>(value))} {}

    tree_storage(const tree_storage& other) noexcept(std::is_nothrow_copy_constructible_v<tree_node<T>>)
        : alloc{other.alloc}
        , root{create_node(this->alloc, other.root)} {}

    tree_storage(tree_storage&& other) noexcept(std::is_nothrow_move_constructible_v<tree_node<T>>)
        : alloc{std::move(other.alloc)}
        , root{other.root} {}

    virtual ~tree_storage() noexcept {
        clear();
    }

    void clear_node_impl(tree_node<T>* node) noexcept {
        assert(node != nullptr);
        if (node->first_child() != nullptr) {
            tree_node<T>* curr_node = node->first_child();
            while (curr_node != nullptr) {
                tree_node<T>* tmp = curr_node->next_sibling();
                clear_node_impl(curr_node);
                curr_node = tmp;
            }
        }

        allocator_traits::destroy(alloc, node);
        allocator_traits::deallocate(alloc, node, 1);
    }

    template <typename... Args>
    static tree_node<T>* create_node(Allocator& alloc, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args&&...>) {
        tree_node<T>* node = allocator_traits::allocate(alloc, 1);
        allocator_traits::construct(alloc, std::forward<Args>(args)...);
        return node;
    }

    void clear() noexcept {
        if (root != nullptr) {
            clear_node_impl(root);
        }
    }

    Allocator alloc;
    tree_node<T>* root;
};

template <typename T, typename Allocator = std::allocator<tree_node<T>>>
class tree
    : private tree_storage<T, Allocator>
    , private detail::enable_special_members<T> {
    using base = tree_storage<T, Allocator>;

public:
    using allocator_type  = Allocator;
    using value_type      = T;
    using node_type       = tree_node<T>;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;

    tree() = default;

    explicit tree(Allocator alloc) noexcept
        : base{std::move(alloc)} {};

    size_type size() const noexcept {
        return 0;
    }

    bool empty() const noexcept {
        return base::root == nullptr;
    }

    void clear() noexcept {
        base::clear();
    }
};

#endif // TREE_H_INCLUDED
