#include <stack>
#include <string>

using namespace std;

class Coord {
    public:
        Coord(int rr, int cc) : m_row(rr), m_col(cc) {}
        int r() const { return m_row; }
        int c() const { return m_col; }
    private:
        int m_row;
        int m_col;
};

bool pathExists(string maze[], int nRows, int nCols, int sr, int sc, int er, int ec) {
    stack<Coord> coordStack;

    Coord startingPos(sr, sc);
    coordStack.push(startingPos);
    maze[sr][sc] = 'V'; // V for visited

    while (!coordStack.empty()) {
        Coord top = coordStack.top();
        coordStack.pop();

        int row = top.r(), col = top.c();

        if (row == er && col == ec) {
            return true;
        }

        if (row - 1 >= 0 && maze[row - 1][col] == '.') {
            Coord northCoord(row - 1, col);
            coordStack.push(northCoord);
            maze[row - 1][col] = 'V';
        }

        if (col + 1 <= nCols - 1 && maze[row][col + 1] == '.') {
            Coord eastCoord(row, col + 1);
            coordStack.push(eastCoord);
            maze[row][col + 1] = 'V';
        }

        if (row + 1 <= nRows - 1 && maze[row + 1][col] == '.') {
            Coord southCoord(row + 1, col);
            coordStack.push(southCoord);
            maze[row + 1][col] = 'V';
        }

        if (col - 1 >= 0 && maze[row][col - 1] == '.') {
            Coord westCoord(row, col - 1);
            coordStack.push(westCoord);
            maze[row][col - 1] = 'V';
        }
    }

    return false;
}