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

    friend void replace(tree_node* old_node, tree_node* new_node) noexcept {
        auto old_impl = static_cast<impl*>(old_node);
        auto new_impl = static_cast<impl*>(new_node);
        impl* parent = old_impl->parent;

        new_impl->parent = parent;
        if (parent != nullptr) {
            if (parent->first_child == old_impl) {
                parent->first_child = new_impl;
            }
            if (parent->last_child == old_impl) {
                parent->last_child = new_impl;
            }
        }

        new_impl->prev_sibling = old_impl->prev_sibling;
        if (old_impl->prev_sibling != nullptr) {
            old_impl->prev_sibling->next_sibling = new_impl;
        }
        new_impl->next_sibling = old_impl->next_sibling;
        if (old_impl->next_sibling != nullptr) {
            old_impl->next_sibling->prev_sibling = new_impl;
        }

        old_impl->parent       = nullptr;
        old_impl->prev_sibling = nullptr;
        old_impl->next_sibling = nullptr;
    }

    friend void insert_sibling(tree_node* old_node, tree_node* new_node) noexcept {
        auto old_impl = static_cast<impl*>(old_node);
        auto new_impl = static_cast<impl*>(new_node);
        impl* parent = old_impl->parent;

        assert(parent != nullptr);
        new_impl->parent = parent;
        if (parent->first_child == old_impl) {
            parent->first_child = new_impl;
        }

        new_impl->prev_sibling = old_impl->prev_sibling;
        new_impl->next_sibling = old_impl;

        if (old_impl->prev_sibling != nullptr) {
            old_impl->prev_sibling->next_sibling = new_impl;
        }
        old_impl->prev_sibling = new_impl;
    }
};

template <typename T, typename Allocator>
class tree;

template <typename T>
class tree_iterator {
public:
    using value_type = std::remove_cv_t<T>;
    using pointer = T*;
    using reference = T&;
    using difference_type = intptr_t;
    using iterator_category = std::bidirectional_iterator_tag;

    template <typename, typename>
    friend class tree;

    tree_iterator() noexcept
        : curr_node{nullptr} {}

    explicit tree_iterator(tree_node<T>* node, tree_node<T>* prev_node) noexcept
        : curr_node{node}
        , prev_node{prev_node} {}

    tree_iterator(const tree_iterator& other) noexcept = default;
    tree_iterator(tree_iterator&& other) noexcept = default;

    virtual ~tree_iterator() noexcept = default;

    bool operator == (const tree_iterator& other) const noexcept {
        return curr_node == other.curr_node
            && prev_node == other.prev_node;
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
    tree_node<T>* prev_node;
};

template <typename T>
class pre_order_iterator : public tree_iterator<T> {
public:
    using tree_iterator<T>::curr_node;
    using tree_iterator<T>::prev_node;

    pre_order_iterator() noexcept = default;

    explicit pre_order_iterator(tree_node<T>* node, tree_node<T>* prev_node) noexcept
        : tree_iterator<T>{node, prev_node} {}

    pre_order_iterator(const pre_order_iterator& other) noexcept = default;
    pre_order_iterator(pre_order_iterator&& other) noexcept = default;

    ~pre_order_iterator() noexcept = default;

    pre_order_iterator& operator ++ () noexcept {
        prev_node = curr_node;
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
        curr_node = prev_node;
        if (prev_node != nullptr) {
            if (prev_node->prev_sibling() != nullptr) {
                prev_node = prev_node->prev_sibling();
                while (prev_node->last_child() != nullptr) {
                    prev_node = prev_node->last_child();
                }
            } else {
                prev_node = prev_node->parent();
            }
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
        allocator_traits::construct(alloc, node, std::forward<Args>(args)...);
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

template <typename T>
class pre_order_view;

namespace insertion {
    struct vert_tag {};
    struct hor_tag {};

    inline constexpr vert_tag vert;
    inline constexpr hor_tag hor;
}

template <typename T, typename Allocator = std::allocator<tree_node<T>>>
class tree
    : private tree_storage<T, Allocator>
    , private detail::enable_special_members<T> {
    using base = tree_storage<T, Allocator>;

    friend class pre_order_view<T>;

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

    tree()
        : base{}
        , node_count{0} {};

    explicit tree(Allocator alloc) noexcept
        : base{std::move(alloc)}
        , node_count{0} {};

    size_type size() const noexcept {
        return node_count;
    }

    bool empty() const noexcept {
        return base::root == nullptr;
    }

    void clear() noexcept {
        base::clear();
    }

    template <typename Iterator>
    void insert(insertion::vert_tag, Iterator it, const T& value) noexcept(std::is_nothrow_constructible_v<T, const T&>) {
        tree_node<T>* old_node = it.curr_node;
        tree_node<T>* new_node = base::create_node(base::alloc, value);
        insert_node_vert(old_node, new_node);
        node_count++;
    }

    template <typename Iterator>
    void insert(insertion::vert_tag, Iterator it, T&& value) noexcept(std::is_nothrow_constructible_v<T, T&&>) {
        tree_node<T>* old_node = it.curr_node;
        tree_node<T>* new_node = base::create_node(base::alloc, std::move(value));
        insert_node_vert(old_node, new_node);
        node_count++;
    }

    template <typename Iterator>
    void insert(insertion::hor_tag, Iterator it, const T& value) noexcept(std::is_nothrow_constructible_v<T, const T&>) {
        tree_node<T>* old_node = it.curr_node;
        tree_node<T>* new_node = base::create_node(base::alloc, value);
        insert_node_hor(old_node, new_node);
        node_count++;
    }

    template <typename Iterator>
    void insert(insertion::hor_tag, Iterator it, T&& value) noexcept(std::is_nothrow_constructible_v<T, T&&>) {
        tree_node<T>* old_node = it.curr_node;
        tree_node<T>* new_node = base::create_node(base::alloc, std::move(value));
        insert_node_hor(old_node, new_node);
        node_count++;
    }

    template <typename Iterator>
    void append_child(Iterator parent_it, const T& value) noexcept(std::is_nothrow_constructible_v<T, const T&>) {
        assert(parent_it.curr_node != nullptr);
        parent_it.curr_node->push_back_child(base::create_node(base::alloc, value));
        node_count++;
    }

    template <typename Iterator>
    void append_child(Iterator parent_it, T&& value) noexcept(std::is_nothrow_constructible_v<T, T&&>) {
        assert(parent_it.curr_node != nullptr);
        parent_it.curr_node->push_back_child(base::create_node(base::alloc, std::move(value)));
        node_count++;
    }

    template <typename Iterator>
    void prepend_child(Iterator parent_it, T&& value) noexcept(std::is_nothrow_constructible_v<T, T&&>) {
        assert(parent_it.curr_node != nullptr);
        parent_it.curr_node->push_front_child(base::create_node(base::alloc, std::move(value)));
        node_count++;
    }

    template <typename Iterator>
    void erase_subtree(Iterator node_it) noexcept {
        assert(node_it.curr_node != nullptr);
        tree_node<T>* node   = node_it.curr_node;
        tree_node<T>* parent = node->parent();

        parent->unlink_child(node);

        node_count -= count_nodes(node);
        base::clear_node_impl(node);
    }

private:
    void insert_node_vert(tree_node<T>* old_node, tree_node<T>* new_node) noexcept {
        if (old_node != nullptr) {
            tree_node<T>* parent = old_node->parent();
            if (parent != nullptr) {
                replace(old_node, new_node);
            } else {
                base::root = new_node;
            }
            new_node->push_back_child(old_node);
        } else {
            tree_node<T>* last_node = find_last_node();
            if (last_node != nullptr) {
                last_node->push_back_child(new_node);
            } else {
                base::root = new_node;
            }
        }
    }

    void insert_node_hor(tree_node<T>* old_node, tree_node<T>* new_node) noexcept {
        if (old_node != nullptr) {
            tree_node<T>* parent = old_node->parent();
            assert(parent != nullptr);
            insert_sibling(old_node, new_node);
        } else {
            tree_node<T>* last_node = find_last_node();
            if (last_node != nullptr) {
                tree_node<T>* parent = last_node->parent();
                parent->push_back_child(new_node);
            } else {
                base::root = new_node;
            }
        }
    }

    tree_node<T>* find_last_node() const noexcept {
        if (base::root != nullptr) {
            tree_node<T>* node = base::root;
            while (node->last_child() != nullptr) {
                node = node->last_child();
            }
            return node;
        } else {
            return nullptr;
        }
    }

    size_t count_nodes(const tree_node<T>* node) const noexcept {
        size_t result = 1;
        const tree_node<T>* curr = node->first_child();
        while (curr != nullptr) {
            result += count_nodes(curr);
            curr = curr->next_sibling();
        }
        return result;
    }

    size_t node_count;
};

template <typename T>
class pre_order_view {
public:
    using iterator               = pre_order_iterator<T>;
    using const_iterator         = pre_order_iterator<const T>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using value_type      = typename tree<T>::value_type;
    using reference       = typename tree<T>::reference;
    using const_reference = typename tree<T>::const_reference;
    using pointer         = typename tree<T>::pointer;
    using const_pointer   = typename tree<T>::const_pointer;
    using size_type       = typename tree<T>::size_type;
    using difference_type = typename tree<T>::difference_type;

    pre_order_view(const tree<T>& tree)
        : viewable{tree} {}

    iterator begin() const noexcept {
        return iterator{viewable.root, nullptr};
    }

    iterator end() const noexcept {
        return iterator{nullptr, viewable.find_last_node()};
    }

    const_iterator cbegin() const noexcept {
        return const_iterator{viewable.root, nullptr};
    }

    const_iterator cend() const noexcept {
        return const_iterator{nullptr, viewable.find_last_node()};
    }

    reverse_iterator rbegin() const noexcept {
        return std::make_reverse_iterator(end());
    }

    reverse_iterator rend() const noexcept {
        return std::make_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const noexcept {
        return std::make_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const noexcept {
        return std::make_reverse_iterator(cbegin());
    }

    reference front() noexcept {
        return *begin();
    }

    const_reference front() const noexcept {
        return *begin();
    }

    reference back() noexcept {
        return *(--end());
    }

    const_reference back() const noexcept {
        return *(--end());
    }

    size_type size() const noexcept {
        return viewable.size();
    }

    bool empty() const noexcept {
        return viewable.empty();
    }

private:
    const tree<T>& viewable;
};

#endif // TREE_H_INCLUDED
