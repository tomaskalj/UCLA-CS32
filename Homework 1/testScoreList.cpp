#include "Sequence.h"
#include "ScoreList.h"
#include <iostream>
#include <cassert>

using namespace std;

int main() {
    {
        ScoreList scoreList;

        const int NUM_SCORES = 7;
        int scores[NUM_SCORES] = {42, 14, 2, 82, 63, 74, 28};
        for (int i = 0; i < NUM_SCORES; i++) {
            scoreList.add(scores[i]);
        }

        assert(scoreList.size() == NUM_SCORES);
        assert(scoreList.minimum() == 2);
        assert(scoreList.maximum() == 82);
        assert(scoreList.remove(74));
        assert(scoreList.size() == 6);

        cout << "All tests passed aaa" << endl;
    }

    {
        ScoreList scoreList;

        const int NUM_SCORES = 6;
        int scores[NUM_SCORES] = {15, 15, 15, 30, 45, 100};
        for (int i = 0; i < NUM_SCORES; i++) {
            scoreList.add(scores[i]);
        }

        assert(scoreList.size() == NUM_SCORES);
        assert(scoreList.minimum() == 15);
        assert(scoreList.maximum() == 100);
        assert(scoreList.remove(15));
        assert(scoreList.size() == 5);
        assert(scoreList.remove(15));
        assert(scoreList.size() == 4);

        cout << "All tests passed bbb" << endl;
    }
}