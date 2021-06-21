#include "Sequence.h"

Sequence::Sequence() : m_head(nullptr), m_tail(nullptr), m_size(0) {}

Sequence::~Sequence() {
    Node* temp = m_head;

    for (int i = 0; i < m_size - 1; i++) {
        // Assign head to be the node after it
        if (m_head->next != nullptr) {
            m_head = m_head->next;
        }
        // Delete all the nodes up to the last node
        delete temp;
        temp = m_head;
    }

    // After the loop, head should now point to the last node in the sequence.
    // So we can delete head.
    delete m_head;
}

Sequence::Sequence(const Sequence& other) : m_head(nullptr), m_tail(nullptr), m_size(0) {
    // Copy each node from the other Sequence and insert it into our Sequence at the correct position
    int pos = 0;
    for (Node* temp = other.m_head; temp != nullptr; temp = temp->next) {
        insert(pos++, temp->value);
    }
}

Sequence& Sequence::operator=(Sequence rhs) {
    if (this != &rhs) {
        Sequence temp(rhs);
        swap(temp);
    }
    return *this;
}

bool Sequence::empty() const {
    return m_size == 0;
}

int Sequence::size() const {
    return m_size;
}

int Sequence::insert(int pos, const ItemType& value) {
    // Return -1 if the pos is less than 0 or greater than the current size
    if (pos < 0 || pos > m_size) {
        return -1;
    }

    // Create the pointer to the node for the value
    Node* item = new Node;
    item->value = value;
    item->prev = nullptr;
    item->next = nullptr;

    if (pos == 0) {
        // If the list is empty, set the head and tail to the item
        if (m_head == nullptr && m_tail == nullptr) {
            m_head = m_tail = item;
        } else {
            // Otherwise, the list isn't empty, so insert the item before the head
            insertAtFront(item);
        }
    } else if (pos == m_size) {
        // Insert the item after the tail if pos is equal to the current size
        insertAtBack(item);
    } else {
        Node* temp = m_head;
        // Set temp to the node before the one we want to insert at
        for (int i = 0; i < pos - 1; i++) {
            temp = temp->next;
        }

        // Make each node point to the correct node
        item->next = temp->next;
        item->prev = temp;
        temp->next = item;
        item->next->prev = item;
    }

    // Increment the size of the sequence and return the pos
    m_size++;
    return pos;
}

int Sequence::insert(const ItemType& value) {
    int pos = 0;
    for (Node* temp = m_head; temp != nullptr; temp = temp->next) {
        // Loop through the sequence and find the first node such that the value parameter
        // is less than or equal to the node's value, and break out of the loop
        if (value <= temp->value) {
            break;
        }
        pos++;
    }

    // Insert the value as a node at the position specified
    return insert(pos, value);
}

bool Sequence::erase(int pos) {
    if (pos < 0 || pos >= m_size) {
        return false;
    }

    Node* temp = m_head;

    // If pos is 0, we're deleting the node that head points to. Need to be careful
    // in case the sequence is only one node (i.e. head and tail point to the same node)
    if (pos == 0) {
        // If the head's next node isn't null, we can make head its next node
        if (m_head->next != nullptr) {
            m_head = m_head->next;
            m_head->prev = nullptr;
        } else {
            // Otherwise, the sequence is now empty, so make the head and tail null
            m_head = m_tail = nullptr;
        }
        // Free the allocated memory
        delete temp;
    } else {
        // Set temp to the node before the one we want to delete
        for (int i = 0; i < pos - 1; i++) {
            temp = temp->next;
        }

        if (temp->next != nullptr) {
            // The node we want to delete is the one after temp (i.e. at pos)
            Node* nodeToDelete = temp->next;
            if (nodeToDelete->next != nullptr) {
                // Make each node point to the correct node
                temp->next = nodeToDelete->next;
                nodeToDelete->next->prev = temp;
            } else {
                // If the node after the one being deleted is null, then the node being deleted is the tail
                temp->next = nullptr;
                m_tail = temp;
            }

            delete nodeToDelete;
        }
    }

    m_size--;
    return true;
}

int Sequence::remove(const ItemType& value) {
    // Handle the case where the sequence contains only one element.
    // This is the cleanest solution without breaking the following code.
    if (m_size == 1) {
        erase(0);
        return 1;
    }

    int numRemoved = 0;

    if (!empty()) {
        // Loop backwards to prevent possibly modifying the sequence and breaking the function
        Node* temp = m_tail;
        for (int i = m_size - 1; i >= 0; i--) {
            bool deleteHere = temp->value == value;

            // Reassign temp so the loop works as intended
            if (temp->prev != nullptr) {
                temp = temp->prev;
            }

            if (deleteHere && erase(i)) {
                numRemoved++;
            }
        }
    }

    return numRemoved;
}

bool Sequence::get(int pos, ItemType& value) const {
    if (pos < 0 || pos >= m_size) {
        return false;
    }

    // Get the node at the position we want and make temp point to it
    Node* temp = m_head;
    for (int i = 0; i < pos; i++) {
        temp = temp->next;
    }

    // Assign the value parameter to the value of the node temp points to
    value = temp->value;

    return true;
}

bool Sequence::set(int pos, const ItemType& value) {
    if (pos < 0 || pos >= m_size) {
        return false;
    }

    // Get the node at the position we want and make temp point to it
    Node* temp = m_head;
    for (int i = 0; i < pos; i++) {
        temp = temp->next;
    }

    // Assign the value of the node temp points to to the value parameter
    temp->value = value;

    return true;
}

int Sequence::find(const ItemType& value) const {
    int pos = 0;
    for (Node* temp = m_head; temp != nullptr; temp = temp->next) {
        // If the value of the node temp points to equals the value parameter,
        // we have the position of the node we want
        if (temp->value == value) {
            return pos;
        }
        pos++;
    }

    // Return -1 if the value parameter does not exist in the sequence
    return -1;
}

// Trivial function to swap the head and tail pointers as well as the size of two sequences
void Sequence::swap(Sequence& other) {
    Node* tempHead = m_head;
    m_head = other.m_head;
    other.m_head = tempHead;

    Node* tempTail = m_tail;
    m_tail = other.m_tail;
    other.m_tail = tempTail;

    int tempSize = m_size;
    m_size = other.m_size;
    other.m_size = tempSize;
}

int subsequence(const Sequence& seq1, const Sequence& seq2) {
    if (seq1.size() < seq2.size() || seq2.empty()) {
        return -1;
    }

    int pos = -1;

    // These two variables of type ItemType store the values which we compare
    ItemType seq1Item, seq2Item;

    for (int i = 0; i < seq1.size(); i++) {
        bool allMatch = true;

        for (int j = 0; j < seq2.size(); j++) {
            // Store the values to seq1Item and seq2Item respectively
            if (seq1.get(i + j, seq1Item) && seq2.get(j, seq2Item)) {
                // If there is at least one incorrect match, then this isn't a valid subsequence
                if (seq1Item != seq2Item) {
                    allMatch = false;
                }
            }
        }

        // If they all match, we found our position where there exists a subsequence.
        if (allMatch) {
            pos = i;
            break;
        }
    }

    return pos;
}

void interleave(const Sequence& seq1, const Sequence& seq2, Sequence& result) {
    // Clear the result sequence so it has no items besides the one this function adds to it
    if (!result.empty()) {
        for (int i = result.size() - 1; i >= 0; i--) {
            result.erase(i);
        }
    }

    // These two variables of type ItemType store the values which we add to the result Sequence
    ItemType seq1Item, seq2Item;
    int seq1Ctr = 0, seq2Ctr = 0, maxSize = seq1.size() + seq2.size();

    // Stay in this loop if we haven't added every element from seq1 and seq2 to result
    while (seq1Ctr + seq2Ctr < maxSize) {
        // If we haven't added every element from seq1 to result, keep adding
        if (seq1.get(seq1Ctr, seq1Item)) {
            result.insert(seq1Ctr + seq2Ctr, seq1Item);
            seq1Ctr++;
        }

        // If we haven't added every element from seq2 to result, keep adding
        if (seq2.get(seq2Ctr, seq2Item)) {
            result.insert(seq1Ctr + seq2Ctr, seq2Item);
            seq2Ctr++;
        }
    }
}

void Sequence::dump() const {
//    if (empty()) {
//        std::cout << "sequence is empty" << std::endl;
//    } else {
//        int pos = 0;
//        for (Node* temp = m_head; temp != nullptr; temp = temp->next) {
//            std::cout << pos++ << ": " << temp->value << std::endl;
//        }
//    }
//
//    if (m_head != nullptr) {
//        std::cout << "head: " << m_head->value << std::endl;
//    } else {
//        std::cout << "head: null" << std::endl;
//    }
//    if (m_tail != nullptr) {
//        std::cout << "tail: " << m_tail->value << std::endl;
//    } else {
//        std::cout << "tail: null" << std::endl;
//    }
//    std::cout << "size: " << m_size << std::endl;
//    std::cout << std::endl;
}

// Trivial private member function to insert a node at the beginning of a sequence
void Sequence::insertAtFront(Sequence::Node* item) {
    item->next = m_head;
    m_head->prev = item;
    m_head = item;
}

// Trivial private member function to insert a node at the end of a sequence
void Sequence::insertAtBack(Sequence::Node* item) {
    item->prev = m_tail;
    m_tail->next = item;
    m_tail = item;
}