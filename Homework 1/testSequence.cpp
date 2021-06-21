#include "Sequence.h"
#include <iostream>
#include <cassert>

using namespace std;

int main() {
    {
        Sequence s;
        assert(s.empty());
        assert(s.find(42) == -1);
        assert(s.insert(42) == 0);
        assert(s.size() == 1 && s.find(42) == 0);
        cout << "Passed all tests" << endl;
    }

    {
        Sequence a;
        Sequence b;

        for (long l = 0; l <= 10; l += 5) {
            assert(a.insert(l) != -1);
        }

        for (long l = 0; l <= 20; l += 5) {
            assert(b.insert(l) != -1);
        }

        a.swap(b);

        assert(a.size() == 5);
        assert(b.size() == 3);

        cout << "All tests passed ggg" << endl;
    }

    {
        Sequence s;
        for (long l = 1; l <= 10; l++) {
            assert(s.insert(l) != -1);
        }

        assert(s.erase(0));
        assert(s.size() == 9);

        assert(s.erase(0));
        assert(s.size() == 8);

        ItemType v;
        assert(s.get(0, v));
        assert(v == 3);

        for (int i = 0; i < 2; i++) {
            assert(s.insert(3) != -1);
        }
        assert(s.size() == 10);
        assert(s.remove(3) == 3);
        assert(s.size() == 7);

        assert(s.set(2, 100));
        assert(s.find(100) == 2);

        cout << "All tests passed hhh" << endl;
    }
}