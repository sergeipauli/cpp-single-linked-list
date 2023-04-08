#pragma once

#include <algorithm>
#include <cassert>
#include <forward_list>
#include <iterator>

template <typename Type>
class SingleLinkedList {
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next) : value(val), next_node(next) {}

        Type value;
        Node* next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = Type;
            using difference_type = std::ptrdiff_t;
            using pointer = ValueType*;
            using reference = ValueType&;

            BasicIterator() = default;

            BasicIterator(const BasicIterator <Type>& other) noexcept {
                node_ = other.node_;
            }

            BasicIterator& operator= (const BasicIterator& rhs) = default;

            [[nodiscard]] bool operator== (const BasicIterator <const Type>& rhs) const noexcept {
                return node_ == rhs.node_;
            }

            [[nodiscard]] bool operator!= (const BasicIterator <const Type>& rhs) const noexcept {
                return node_ != rhs.node_;
            }

            [[nodiscard]] bool operator== (const BasicIterator <Type>& rhs) const noexcept {
                return node_ == rhs.node_;
            }

            [[nodiscard]] bool operator!= (const BasicIterator <Type>& rhs) const noexcept {
                return node_ != rhs.node_;
            }

            BasicIterator& operator++ () noexcept {
                assert(node_ != nullptr); // FIX_1
                node_ = node_ -> next_node;
                return *this;
            }

            BasicIterator operator++ (int) noexcept {
                assert(node_ != nullptr); // FIX_2
                BasicIterator _value(node_);
                node_ = node_ -> next_node;
                return _value;
            }

            [[nodiscard]] reference operator* () const noexcept {
                assert(node_ != nullptr); // FIX_3
                return node_ -> value;
            }

            [[nodiscard]] pointer operator->() const noexcept {
                assert(node_ != nullptr); // FIX_4
                return &node_ -> value;
            }

        private:
            friend class SingleLinkedList;

            explicit BasicIterator(Node* node) : node_(node) {}

            Node* node_ = nullptr;
    };

    public:
        using value_type = Type;
        using reference = value_type&;
        using const_reference = const value_type&;

        using Iterator = BasicIterator <Type>;
        using ConstIterator = BasicIterator <const Type>;

        [[nodiscard]] Iterator begin() noexcept {
            return Iterator(head_.next_node);
        }

        [[nodiscard]] Iterator end() noexcept {
            return Iterator(nullptr);
        }

        [[nodiscard]] ConstIterator begin() const noexcept {
            return ConstIterator(head_.next_node);
        }

        [[nodiscard]] ConstIterator end() const noexcept {
            return ConstIterator(nullptr);
        }

        [[nodiscard]] ConstIterator cbegin() const noexcept {
            return ConstIterator(head_.next_node);
        }

        [[nodiscard]] ConstIterator cend() const noexcept {
            return ConstIterator(nullptr);
        }

        [[nodiscard]] Iterator before_begin() noexcept {
            return Iterator(&head_);
        }

        [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
            return ConstIterator{ const_cast <Node*> (&head_)};
        }

        [[nodiscard]] ConstIterator before_begin() const noexcept {
            return ConstIterator{ const_cast <Node*> (&head_)};
        }

        void swap(SingleLinkedList& other) noexcept {
            std::swap(this -> head_.next_node, other.head_.next_node);
            std::swap(this -> size_, other.size_);
        }

        SingleLinkedList() = default;

        //FIX_5
        SingleLinkedList(std::initializer_list <Type> values) {
            SingleLinkedList tmp;

            for (auto iter = values.end() - 1; iter >= values.begin(); --iter) {
                tmp.PushFront(*iter);
            }

            swap(tmp);
        }

        //FIX_6
        SingleLinkedList(const SingleLinkedList& other) {
            if (size_ == 0 && head_.next_node == nullptr) {
                SingleLinkedList tmp;

                for (auto iter = other.begin(); iter != other.end(); ++iter) {
                    tmp.InsertAfter(tmp.begin(), *iter);
                }

                swap(tmp);
            }
        }

        SingleLinkedList& operator= (const SingleLinkedList& rhs) {
            if (this != &rhs) {
                SingleLinkedList tmp(rhs);

                swap(tmp);
            }

            return *this;
        };

        ~SingleLinkedList() {
            Clear();
        }

        [[nodiscard]] size_t GetSize() const noexcept {
            return size_;
        }

        [[nodiscard]] bool IsEmpty() const noexcept {
            return size_ == 0;  //FIX_7
        }

        void PushFront(const Type& value) {
            head_.next_node = new Node(value, head_.next_node);
            ++size_;
        }

        Iterator InsertAfter(ConstIterator position, const Type& value) {
            assert(position.node_ != nullptr); //FIX_8

            Node* new_node = new Node(value, position.node_ -> next_node);
            position.node_ -> next_node = new_node;

            ++size_;

            return Iterator(new_node);
        }

        void PopFront() noexcept {
            assert(head_.next_node != nullptr); //FIX_9

            Node* next = head_.next_node -> next_node;
            delete head_.next_node;
            head_.next_node = next;

            --size_;
        }

        Iterator EraseAfter(ConstIterator position) noexcept {
            assert(head_.next_node != nullptr); //FIX_10_1


            auto for_delete = position.node_ -> next_node;

            assert(for_delete != nullptr); //FIX_10_2

            auto new_next = position.node_ -> next_node -> next_node;
            delete for_delete;

            position.node_ -> next_node = new_next;

            return Iterator{ position.node_ -> next_node };

            return end();
        }



        void Clear() noexcept {
            Node* buffer;

            while(head_.next_node != nullptr) {
                buffer = head_.next_node; // присваивает первый элемент
                head_.next_node = head_.next_node -> next_node; // head_ ссылается на поле следущюего через указатель и так, пока не будет nullptr
                delete buffer; // очистка памяти
            }

            size_ = 0;
        }

    private:
        // DATA // FIX_11
        Node    head_;                   // первый элемент списка - всегда содержит укащатель на первый элемент списка.
        size_t  size_ = 0;               // размер списка
};

template <typename Type>
void swap(SingleLinkedList <Type>& lhs, SingleLinkedList <Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator== (const SingleLinkedList <Type>& lhs, const SingleLinkedList <Type>& rhs) {
    // FIX_12
    if (lhs.GetSize() == rhs.GetSize()) {
        return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    return false;
}

template <typename Type>
bool operator!= (const SingleLinkedList <Type>& lhs, const SingleLinkedList <Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator< (const SingleLinkedList <Type>& lhs, const SingleLinkedList <Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<= (const SingleLinkedList <Type>& lhs, const SingleLinkedList <Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
bool operator> (const SingleLinkedList <Type>& lhs, const SingleLinkedList <Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>= (const SingleLinkedList <Type>& lhs, const SingleLinkedList <Type>& rhs) {
    return !(lhs < rhs);
}
