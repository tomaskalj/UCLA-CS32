#include "newSequence.h"
#include <string>
#include <iostream>
#include <cassert>

using namespace std;

int main() {
    {
        Sequence a(1000);   // a can hold at most 1000 items
        Sequence b(5);      // b can hold at most 5 items
        Sequence c;         // c can hold at most DEFAULT_MAX_ITEMS items
        ItemType v = 15;

        // No failures inserting 5 items into b
        for (int k = 0; k < 5; k++)
            assert(b.insert(v) != -1);

        // Failure if we try to insert a sixth item into b
        assert(b.insert(v) == -1);

        // When two Sequences' contents are swapped, their capacities are
        // swapped as well:
        a.swap(b);
        assert(a.insert(v) == -1 && b.insert(v) != -1);

        cout << "All tests passed fff" << endl;
    }

    {
        Sequence a(3);
        Sequence b(5);

        for (long l = 0; l <= 10; l += 5) {
            assert(a.insert(l) != -1);
        }
        assert(a.insert(50) == -1);

        for (long l = 0; l <= 20; l += 5) {
            assert(b.insert(l) != -1);
        }
        assert(b.insert(50) == -1);

        a.swap(b);

        assert(a.size() == 5);
        assert(b.size() == 3);

        cout << "All tests passed ggg" << endl;
    }

    {
        Sequence s(10);
        for (long l = 1; l <= 10; l++) {
            assert(s.insert(l) != -1);
        }
        assert(s.insert(11) == -1);

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