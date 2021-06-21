#include "newSequence.h"

Sequence::Sequence(int maxSize) : m_size(0), m_maxSize(maxSize) {
    m_items = new ItemType[maxSize];
}

Sequence::Sequence(const Sequence& other) {
    m_size = other.m_size;
    m_maxSize = other.m_maxSize;
    m_items = new ItemType[m_maxSize];

    for (int i = 0; i < m_maxSize; i++) {
        m_items[i] = other.m_items[i];
    }
}

Sequence& Sequence::operator=(const Sequence& rhs) {
    if (this != &rhs) {
        Sequence temp(rhs);
        swap(temp);
    }
    return *this;
}

Sequence::~Sequence() {
    delete [] m_items;
}

bool Sequence::empty() const {
    return m_size == 0;
}

int Sequence::size() const {
    return m_size;
}

int Sequence::insert(int pos, const ItemType& value) {
    if (pos < 0 || pos > m_size || m_size == m_maxSize) {
        return -1;
    }

    for (int i = m_size - 1; i >= pos; i--) {
        m_items[i + 1] = m_items[i];
    }
    m_items[pos] = value;
    m_size++;

    return pos;
}

int Sequence::insert(const ItemType& value) {
    if (m_size == m_maxSize) {
        return -1;
    }

    int pos = -1;
    for (int i = 0; i < m_size; i++) {
        if (value <= m_items[i]) {
            pos = i;
            break;
        }
    }

    if (pos == -1) {
        pos = m_size;
    }

    return insert(pos, value);
}

bool Sequence::erase(int pos) {
    if (pos < 0 || pos >= m_size) {
        return false;
    }

    for (int i = pos; i < m_size - 1; i++) {
        m_items[i] = m_items[i + 1];
    }
    m_size--;

    return true;
}

int Sequence::remove(const ItemType& value) {
    int numRemoved = 0;

    for (int i = m_size - 1; i >= 0; i--) {
        if (m_items[i] == value) {
            if (erase(i)) {
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

    value = m_items[pos];
    return true;
}

bool Sequence::set(int pos, const ItemType& value) {
    if (pos < 0 || pos >= m_size) {
        return false;
    }

    m_items[pos] = value;
    return true;
}

int Sequence::find(const ItemType& value) const {
    for (int i = 0; i < m_size; i++) {
        if (m_items[i] == value) {
            return i;
        }
    }

    return -1;
}

void Sequence::swap(Sequence& other) {
    ItemType* tempItems = m_items;
    m_items = other.m_items;
    other.m_items = tempItems;

    int tempSize = m_size;
    m_size = other.m_size;
    other.m_size = tempSize;

    int tempMaxSize = m_maxSize;
    m_maxSize = other.m_maxSize;
    other.m_maxSize = tempMaxSize;
}

void Sequence::dump() const {
//    std::cout << "size: " << m_size << std::endl;
//    for (int i = 0; i < m_size; i++) {
//        std::cout << i << ": " << m_items[i] << std::endl;
//    }
//    std::cout << std::endl;
}
