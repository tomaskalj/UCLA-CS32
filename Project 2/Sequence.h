#ifndef PROJECT_2_SEQUENCE_H
#define PROJECT_2_SEQUENCE_H

#include <string>

using ItemType = std::string;

class Sequence {
    public:
        Sequence();
        ~Sequence();
        Sequence(const Sequence& other);
        Sequence &operator=(const Sequence rhs);
        bool empty() const;
        int size() const;
        int insert(int pos, const ItemType& value);
        int insert(const ItemType& value);
        bool erase(int pos);
        int remove(const ItemType& value);
        bool get(int pos, ItemType& value) const;
        bool set(int pos, const ItemType& value);
        int find(const ItemType& value) const;
        void swap(Sequence& other);
        void dump() const;
    private:
        struct Node {
            ItemType value;
            Node* prev;
            Node* next;
        };

        void insertAtFront(Node* item);
        void insertAtBack(Node* item);

        Node* m_head;
        Node* m_tail;
        int m_size;
};


#endif //PROJECT_2_SEQUENCE_H