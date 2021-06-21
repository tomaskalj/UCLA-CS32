#include "Sequence.h"
#include "ScoreList.h"

ScoreList::ScoreList() {}

bool ScoreList::add(unsigned long score) {
    if (score < 0 || score > 100 || m_sequence.size() == DEFAULT_MAX_ITEMS) {
        return false;
    }
    m_sequence.insert(score);
    return true;
}

bool ScoreList::remove(unsigned long score) {
    return m_sequence.erase(m_sequence.find(score));
}

int ScoreList::size() const {
    return m_sequence.size();
}

unsigned long ScoreList::minimum() const {
    if (m_sequence.empty()) {
        return NO_SCORE;
    }

    ItemType minScore;
    m_sequence.get(0, minScore);
    return minScore;
}

unsigned long ScoreList::maximum() const {
    if (m_sequence.empty()) {
        return NO_SCORE;
    }

    ItemType maxScore;
    m_sequence.get(m_sequence.size() - 1, maxScore);
    return maxScore;
}
