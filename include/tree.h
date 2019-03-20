#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include <type_traits>
#include <iterator>
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
    using impl = tree_node_impl<T>;

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
        return reinterpret_cast<tree_node*>(impl::prev_sibling);
    }

    tree_node* next_sibling() const noexcept {
        return reinterpret_cast<tree_node*>(impl::next_sibling);
    }

    tree_node* first_child() const noexcept {
        return reinterpret_cast<tree_node*>(impl::first_child);
    }

    tree_node* last_child() const noexcept {
        return reinterpret_cast<tree_node*>(impl::last_child);
    }

    tree_node* parent() const noexcept {
        return reinterpret_cast<tree_node*>(impl::parent);
    }

    T& value() noexcept {
        return impl::value;
    }

    const T& value() const noexcept {
        return impl::value;
    }

    void push_back_child(tree_node* child) noexcept {
        auto child_impl = static_cast<impl*>(child);
        auto self_impl = static_cast<impl*>(this);

        if (self_impl->first_child == nullptr) {
            self_impl->first_child = child;
        }

        if (self_impl->last_child != nullptr) {
            self_impl->last_child->next_sibling = child;
            child_impl->prev_sibling = self_impl->last_child;
        }

        self_impl->last_child = child;
        child_impl->parent = this;
    }

    void push_front_child(tree_node* child) noexcept {
        auto child_impl = static_cast<impl*>(child);
        auto self_impl = static_cast<impl*>(this);

        if (self_impl->last_child == nullptr) {
            self_impl->last_child = child_impl;
        }

        if (self_impl->first_child != nullptr) {
            self_impl->first_child->prev_sibling = child_impl;
            child_impl->next_sibling = self_impl->first_child;
        }

        self_impl->first_child = child_impl;
        child_impl->parent = this;
    }

    void unlink_child(tree_node* child) noexcept {
        auto child_impl = static_cast<impl*>(child);
        auto self_impl = static_cast<impl*>(this);

        if (child_impl->prev_sibling != nullptr) {
            child_impl->prev_sibling->next_sibling = child_impl->next_sibling;
        }

        if (child_impl->next_sibling != nullptr) {
            child_impl->next_sibling->prev_sibling = child_impl->prev_sibling;
        }

        if (self_impl->first_child == child_impl) {
            self_impl->first_child = child_impl->next_sibling;
        }

        if (self_impl->last_child == child_impl) {
            self_impl->last_child = child_impl->prev_sibling;
        }

        child_impl->parent = nullptr;
        child_impl->prev_sibling = nullptr;
        child_impl->next_sibling = nullptr;
    }
};

template <typename T>
class tree_iterator {
public:
    using value_type = std::remove_cv_t<T>;
    using pointer = T*;
    using reference = T&;
    using difference_type = intptr_t;
    using iterator_category = std::bidirectional_iterator_tag;

    tree_iterator() noexcept
        : curr_node{nullptr} {}

    explicit tree_iterator(tree_node<T>* node) noexcept
        : curr_node{node} {}

    tree_iterator(const tree_iterator& other) noexcept = default;
    tree_iterator(tree_iterator&& other) noexcept = default;

    virtual ~tree_iterator() noexcept = default;

    bool operator == (const tree_iterator& other) const noexcept {
        return curr_node == other.curr_node;
    }

    bool operator != (const tree_iterator& other) const noexcept {
        return !(*this == other);
    }

    T& operator * () const noexcept {
        return curr_node->value();
    }

    T* operator -> () const noexcept {
        return &curr_node->value();
    }

protected:
    tree_node<T>* curr_node;
};

template <typename T>
class pre_order_iterator : public tree_iterator<T> {
public:
    using tree_iterator<T>::curr_node;

    pre_order_iterator() noexcept = default;

    explicit pre_order_iterator(tree_node<T>* node) noexcept
        : tree_iterator<T>{node} {}

    pre_order_iterator(const pre_order_iterator& other) noexcept = default;
    pre_order_iterator(pre_order_iterator&& other) noexcept = default;

    ~pre_order_iterator() noexcept = default;

    pre_order_iterator& operator ++ () noexcept {
        if (curr_node->first_child() != nullptr) {
            curr_node = curr_node->first_child();
        } else {
            while (curr_node != nullptr && curr_node->next_sibling() == nullptr) {
                curr_node = curr_node->parent();
            }

            if (curr_node != nullptr) {
                curr_node = curr_node->next_sibling();
            }
        }

        return *this;
    }

    pre_order_iterator& operator -- () noexcept {
        if (curr_node->prev_sibling() != nullptr) {
            curr_node = curr_node->prev_sibling();
            while (curr_node->last_child() != nullptr) {
                curr_node = curr_node->last_child();
            }
        } else {
            curr_node = curr_node->parent();
        }

        return *this;
    }

    pre_order_iterator operator ++ (int) noexcept {
        pre_order_iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    pre_order_iterator operator -- (int) noexcept {
        pre_order_iterator tmp = *this;
        --(*this);
        return tmp;
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
