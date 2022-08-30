//
// C++ for C programmers part B, first homework
//
// August 2022, Pietro Paolini, London.
//
#include <iostream>
#include <list>
#include <algorithm>
#include <memory>
#include <cassert>
#include "HexBoard.h"

HexCell::HexCell(vector<bool> &star) {

    if (star.size() != 6)
        throw "Error: the star size of a cell MUST be six";

    for (int i = 0; i < star.size(); i++)
    {
        m_star[i] = star[i];
    }
}

pair<int, int> HexCell::TraverseEdge(int r, int c, LINK l) {
    if (l == LINK::BACKWARDS) {
        return make_pair(r, c - 1);
    } else if (l == LINK::FORWARD) {
        return make_pair(r, c + 1);
    } else if (l == LINK::ABOVE_LEFT) {
        return make_pair(r - 1, c);
    } else if (l == LINK::ABOVE_RIGHT) {
        return make_pair(r - 1, c + 1);
    } else if (l == LINK::BELOW_LEFT) {
        return make_pair(r + 1, c - 1);
    } else if (l == LINK::BELOW_RIGHT) {
        return make_pair(r + 1, c);
    } else {
        throw "Invalid link passed";
    }
}

HexGraph::HexGraph(int size) : m_size(size) {
    vector<bool> star(6, true);
    m_graph.reserve(size * size);

    // They are stored line by line
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {

            // First column
            if (j ==0)
            {
                star[int(LINK::BACKWARDS)] = false;
            }

            // Last column
            if (j == size - 1)
            {
                star[int(LINK::FORWARD)] = false;
            }

            // First row
            if (i == 0)
            {
                star[int(LINK::ABOVE_LEFT)] = false;
                star[int(LINK::ABOVE_RIGHT)] = false;
            }

            // Last row
            if (i == size -1)
            {
                star[int(LINK::BELOW_LEFT)] = false;
                star[int(LINK::BELOW_RIGHT)] = false;
            }
            m_graph.push_back(make_shared<HexCell>(star));

            // Reset all values
            fill(star.begin(), star.end(), true);
        }
    }
    return;
}

// Return a reference to the cell at the given row, column
const HexCell& HexGraph::cellAt(int r, int c) const {
    if (r > m_size || c > m_size)
    {
        throw "Indexes out of range";
    }
    return *m_graph[r * m_size + c];
}

// This function implements the classic DFS algorithm.
// In the board there are two graphs, one for each player, the function starts
// visiting cells from the cell at the row and column passed as arguments and
// continue until no more cells can be visited or the predicate passed as a
// function object is satisfied.
// Return true if the predicate was satisfied at least once, false if upon the
// completion of the visit the predicate has never been satisfied.
bool HexGraph::visitUntil(int r,
                          int c,
                          function<bool(int, int)> fvisit,
                          function<bool(int, int)> fstop) const {
    vector<bool> visited(m_size * m_size, false);
    list<pair<int, int>> l;
    if (r >= m_size || c >=m_size || r < 0 || c < 0)
    {
        return false;
    }

    // Mark it as visited
    visited[r * m_size + c] = true;
    l.push_back(make_pair(r, c));

    // Start the visit loop
    while (!l.empty()) {
        auto top = l.front();
        auto r = top.first;
        auto c = top.second;
        auto cell = this->cellAt(r, c);

        l.pop_front();

        // Termination case
        if (fstop(r, c)) {
            return true;
        }

        // Mark it so it is not visited next time
        visited[r * m_size + c] = true;

        // Visit all links
        for (auto i = 0; i < 6; i++) {
            auto adj = cell.TraverseEdge(r, c, static_cast<LINK>(i));

            // Out of bound - edge cell
            if (adj.first >= m_size ||
                adj.second >=m_size ||
                adj.first < 0 ||
                adj.second < 0) {
                continue;
            }

            // Already visited
            if (visited[adj.first * m_size + adj.second]) {
                continue;
            }

            // Does not meet the visiting criteria
            if (!fvisit(adj.first, adj.second)) {
                continue;
            }

            // Add to list of cells to be visited
            l.push_back(make_pair(adj.first, adj.second));
        }
    }
    return false;
}

HexBoard::HexBoard(int size,
                   Player start): m_size(size),
                                  m_current(start),
                                  m_moves(0),
                                  m_board(size * size, Player::NONE),
                                  m_graph(size) {
    return;
}


// Generate an ASCII board using the pattern below:
//. - . - . - . - . - . - .
// \ / \ / \ / \ / \ / \ / \
//  . - . - . - . - . - . - .
//   \ / \ / \ / \ / \ / \ / \
//    . - . - . - . - . - . - .
ostream &operator<<(ostream &os, const HexBoard &b) {

    auto cellToChar = [] (const Player &p) -> char {
        char cell = '.';
        if (p == Player::X) {
            cell = 'X';
        }
        if (p == Player::O) {
            cell = 'O';
        }
        return cell;
    };

    auto edgeToChar = [] (int i) -> char {
        if (i % 2 == 0)
            return '\\';
        else
            return '/';
    };

    // They are stored line by line
    for (int i = 0; i < b.m_size; i++) {
        os << string(i*2, ' ');
        for (int j = 0; j < b.m_size - 1; j++) {
            auto c = cellToChar(b.m_board[i * b.m_size + j]);
            os << c << " - ";
        }
        os << cellToChar(b.m_board[i * b.m_size + b.m_size - 1]) << endl;

        // Don't print edges below the last line
        if (i == b.m_size - 1) {
            continue;
        }

        // Now print connection lines
        os << string(i*2 + 1, ' ');

        for (int z = 0; z < b.m_size * 2 - 1; z++) {
           os << edgeToChar(z) << ' ';
        }
        os << endl;
    }
    return os;
}

// Extends the set of cells belonging to a given player, thus 'moving' it
// into a cell. Returns true if the move was successfull false otherwise.
// A move can be invalid if the cell is already taken
bool HexBoard::move(int r, int c) {

    // Cell is already taken
    if (playerAt(r, c) != Player::NONE) {
        return false;
    }
    m_board[r * m_size + c] = m_current;
    m_current = (m_current == Player::X) ? Player::O : Player::X;
    m_moves++;
    return true;
}

void HexBoard::undoMove(int r, int c) {
    m_board[r * m_size + c] = Player::NONE;
    m_current = (m_current == Player::X) ? Player::O : Player::X;
    m_moves--;
}

Player HexBoard::won() {

    // Check if X has won : the other end can be reached starting from the
    // first column of any row
    for (int r = 0; r < m_size; r++) {

        // Does not belong to X hence we can't start visiting from here
        if (playerAt(r, 0) != Player::X)
            continue;

        // Start a graph visit from this cell
        if (m_graph.visitUntil(r,
                               0,
                               [this](int r, int c) { return this->m_board[r * this->m_size + c] == Player::X; },
                               [this](int r, int c) { return c == this->m_size - 1; })) {
            return Player::X;
        }
    }

    // Check if O has won: the other end can be reached starting from the
    // first row of any column
    for (int c = 0; c < m_size; c++) {

        if (playerAt(0, c) != Player::O)
            continue;

        if (m_graph.visitUntil(0,
                               c,
                               [this](int r, int c) { return this->m_board[r * m_size + c] == Player::O; },
                               [this](int r, int c) { return r == this->m_size - 1; })) {
            return Player::O;
        }
    }
    return Player::NONE;
}

Player HexBoard::playerAt(int r, int c) const {
    if (r > m_size || c > m_size)
    {
        throw "Indexes out of range";
    }
    return m_board[r * m_size + c];
}


pair<int, int> HexAI::nextMove() {
    int maxr, maxc;
    double max = 0;

    assert(m_player != Player::NONE);

    // Each un-assigned cell on the board is a plausible move: go through them
    // all the run the proper number of trials based on the difficulty level
    // set. The move giving out the best likelihood of winning the game is the
    // next move.
    int l = m_board.size() * m_board.size();
    for (auto i = 0; i < l; i++) {

        int r = i / m_board.size();
        int c = i % m_board.size();

        // This is not a plausible move.
        if (m_board.playerAt(r, c)!= Player::NONE) {
            continue;
        }

        // Simulate the move: this changes the next player
        m_board.move(r, c);

        // Run trials
        int success = 0;
        int total =  static_cast<std::underlying_type<LEVEL>::type>(m_level);
        for (auto i = 0;i < total; i++) {
            if (trial() == m_player) {
                success++;
            }
        }

        double ratio = ((double) success) / total;

        // Success !
        if (ratio == 1) {
            return make_pair(r, c);
        }
        cout << "Success ratio for " << r << "," << c << " = " << success << "/" << total << "=" << ratio << endl;
        if (ratio > max) {
            max = ratio;
            maxr = r;
            maxc = c;
        }
        m_board.undoMove(r, c);
    }
    return make_pair(maxr, maxc);
}


// Comment this
Player HexAI::trial() {
    int length = m_board.size() * m_board.size();
    vector<int> emptyCellsIndex(length - m_board.movesNumber());
    int counter = 0;
    for (int i = 0; i < length; i++) {

        // Fill up empty cell indexes
        int r, c;
        r = i / m_board.size();
        c = i % m_board.size();

        if (m_board.playerAt(r, c) == Player::NONE)
            emptyCellsIndex[counter++] = i;
    }

    // Shuffle them around
    random_shuffle(emptyCellsIndex.begin(), emptyCellsIndex.end());

    // Create temporary board to play the trial on. Should be too expensive as
    // the graph is a vector of smart pointers
    HexBoard trialBoard = m_board;

    // Play the rest of the game
    for (auto p : emptyCellsIndex) {
        int r = p / trialBoard.size();
        int c = p % trialBoard.size();
        bool res = trialBoard.move(r, c);

        // All move MUST be legit. They should be as all indexes in the
        // emptyCellIndex contains empty cells.
        assert(res);
    }
    return trialBoard.won();
}


int main(int argn, char **argv) {
    int size;
    cout << "Size of the board:" << endl;
    cin >> size;

    char ai = 'N';
    cout << "If you wish 'O' to be the AI please enter 'Y', any other key otherwise" << endl;
    cin >> ai;
    auto aiPlayer = ai == 'Y' ? Player::O : Player::NONE;

    LEVEL aiLevel = LEVEL::EASY;
    if (aiPlayer != Player::NONE) {
        char charAiLevel;
        cout << "Select the level of difficulty [H = hard, M = medium, E = easy]. Default E" << endl;
        cin >> charAiLevel;
        switch (charAiLevel) {
            case 'H':
                aiLevel = LEVEL::HARD;
                break;
            case 'M':
                aiLevel = LEVEL::MEDIUM;
                break;
            case 'E':
                aiLevel = LEVEL::EASY;
                break;
            default:
                 cout << "Invalid level code, defaulting to easy." << endl;
                break;
        }
    }

    HexBoard c(size, Player::X);
    int row, column;

    while (true) {
        cout << c << endl;
        cout << "X plays horizontally, O vertically" << endl;

        // AI plays
        Player current = c.current();
        if (current == aiPlayer) {

            HexAI ai(aiLevel, aiPlayer, c);
            auto aiMove = ai.nextMove();
            row = aiMove.first;
            column = aiMove.second;
        } else {

            // Human plays
            cout << "Next move '" << current << "'" << endl;
            cin >> row;
            cin >> column;
        }
        auto success = c.move(row, column);
        if (!success) {
            cout << "Illegal move" << endl;
            continue;
        }
        cout << "Player " << current << " takes (" << row << "," << column << ")" << endl;
        auto p = c.won();
        if (p != Player::NONE) {
            cout << p << " has won the game!" << endl;
            cout << c;
            break;
        }
    }
    return 0;
}