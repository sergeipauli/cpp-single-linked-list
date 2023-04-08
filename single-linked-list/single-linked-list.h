#pragma once

#include <iterator>
#include <algorithm>
#include <forward_list>

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
                node_ = node_->next_node;
                return *this;
            }

            BasicIterator operator++ (int) noexcept {
                BasicIterator _value(node_);
                node_ = node_->next_node;
                return _value;
            }

            [[nodiscard]] reference operator* () const noexcept {
                return node_->value;
            }

            [[nodiscard]] pointer operator->() const noexcept {
                return &node_->value;
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
            return Iterator(last_node_);
        }

        [[nodiscard]] ConstIterator begin() const noexcept {
            return ConstIterator(head_.next_node);
        }

        [[nodiscard]] ConstIterator end() const noexcept {
            return ConstIterator(last_node_);
        }

        [[nodiscard]] ConstIterator cbegin() const noexcept {
            return ConstIterator(head_.next_node);
        }

        [[nodiscard]] ConstIterator cend() const noexcept {
            return ConstIterator(last_node_);
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

        SingleLinkedList() {
            head_.next_node = last_node_;
        }

        SingleLinkedList(std::initializer_list <Type> values) {
            for (auto iter = values.end() - 1; iter >= values.begin(); --iter) {
                this -> PushFront(*iter);
            }
        }

        SingleLinkedList(const SingleLinkedList& other) {
            if (size_ == 0 && head_.next_node == nullptr) {
                SingleLinkedList tmp_1, tmp_2;

                for (auto iter = other.begin(); iter != other.end(); ++iter) {
                    tmp_1.PushFront(*iter);
                }

                for (auto iter = tmp_1.begin(); iter != tmp_1.end(); ++iter) {
                    tmp_2.PushFront(*iter);
                }

                swap(tmp_2);
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
            return !size_;
        }

        void PushFront(const Type& value) {
            head_.next_node = new Node(value, head_.next_node);
            ++size_;
        }

        Iterator InsertAfter(ConstIterator position, const Type& value) {
            Node* new_node = new Node(value, position.node_ -> next_node);
            position.node_ -> next_node = new_node;

            ++size_;

            return Iterator(new_node);
        }

        void PopFront() noexcept {
            if (!IsEmpty()) {
                Node* next = head_.next_node -> next_node;
                delete head_.next_node;
                head_.next_node = next;

                --size_;
            }
        }

        Iterator EraseAfter(ConstIterator position) noexcept {
            if (!IsEmpty()) {
                auto for_delete = position.node_ -> next_node;
                auto new_next = position.node_ -> next_node -> next_node;
                delete for_delete;

                position.node_ -> next_node = new_next;

                return Iterator{ position.node_ -> next_node };
            }

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
        // DATA //
        Node*   last_node_ = nullptr;    // крайний элемент списка - всегда указывает на nullptr
        Node    head_;                   // первый элемент списка - всегда содержит укащатель на первый элемент списка.
        size_t  size_ = 0;               // размер списка
};

template <typename Type>
void swap(SingleLinkedList <Type>& lhs, SingleLinkedList <Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator== (const SingleLinkedList <Type>& lhs, const SingleLinkedList <Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
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
