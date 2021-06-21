#include <iostream>
#include "History.h"
#include "globals.h"

using namespace std;

History::History(int nRows, int nCols) : m_rows(nRows), m_cols(nCols) {
    for (int r = 0; r < nRows; r++) {
        for (int c = 0; c < nCols; c++) {
            // Set the char at each (r, c) to a period
            unconverted[r][c] = '.';
        }
    }
}

bool History::record(int r, int c) {
    // We can't record something happening at a (r, c)
    // out of bounds, so return false if this is the case
    if (r < 1 || r > m_rows || c < 1 || c > m_cols) {
        return false;
    }

    char& gridChar = unconverted[r - 1][c - 1];
    if (gridChar == '.') {
        gridChar = 'A';
    } else if (gridChar < 'Z') {
        // The number of Flatulans unconverted at a player's (r, c) cannot exceed Z
        gridChar++;
    }

    return true;
}

void History::display() const {
    char grid[MAXROWS][MAXCOLS];
    int r, c;

    // Initialize the grid with corresponding values in the unconverted array

    for (r = 0; r < m_rows; r++) {
        for (c = 0; c < m_cols; c++) {
            grid[r][c] = unconverted[r][c];
        }
    }

    // Clear screen and display the grid to the user

    clearScreen();
    for (r = 0; r < m_rows; r++) {
        for (c = 0; c < m_cols; c++) {
            cout << grid[r][c];
        }
        cout << endl;
    }
    cout << endl;
}