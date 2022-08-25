//
// C++ for C programmers part B, first homework
//
// August 2022, Pietro Paolini, London.
//
#include <iostream>
#include <list>
#include <algorithm>
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

HexBoard::HexBoard(int size): m_size(size),
                              m_owners(size * size, Player::NONE) {
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
           m_graph.push_back(HexCell(star));

           // Reset all values
           fill(star.begin(), star.end(), true);
        }
    }
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
            auto c = cellToChar(b.m_owners[i * b.m_size + j]);
            os << c << " - ";
        }
        os << cellToChar(b.m_owners[i * b.m_size + b.m_size - 1]) << endl;

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
bool HexBoard::move(Player &p, int r, int c) {

    // Cell is already taken
    if (playerAt(r, c) != Player::NONE) {
        return false;
    }
    m_owners[r * m_size + c] = p;
    m_moves++;
    return true;
}

// Return a reference to the cell at the given row, column
const HexCell& HexBoard::cellAt(int r, int c) const {
    if (r > m_size || c > m_size)
    {
        throw "Indexes out of range";
    }
    return m_graph[r * m_size + c];
}

// Return a vector of references to cells adjacent to the one at the row and
// column passed as arguments
vector<pair<int, int>> HexBoard::star(int r, int c) {
    vector<pair<int, int>> star;

    // TODO: move and forward constructors, this isn't that efficient
    // Above left
    if (r - 1 >= 0)
    {
        star.push_back(make_pair(r - 1, c));
    }

    // Above right
    if (r - 1 >= 0 && c + 1 < m_size)
    {
        star.push_back(make_pair(r - 1, c + 1));
    }

    // Backwards
    if (c - 1 >= 0)
    {
        star.push_back(make_pair(r, c - 1));
    }

    // Forward
    if (c + 1 < m_size)
    {
        star.push_back(make_pair(r, c + 1));
    }

    // Below left
    if (r + 1 < m_size && c - 1 >= 0)
    {
        star.push_back(make_pair(r + 1, c - 1));
    }

    // Below right
    if (r + 1 < m_size)
    {
        star.push_back(make_pair(r + 1, c));
    }
    return star;
}

// This function implements the classic DFS algorithm.
// In the board there are two graphs, one for each player, the function starts 
// visiting cells from the cell at the row and column passed as arguments and 
// continue until no more cells can be visited or the predicate passed as a
// function object is satisfied.
// Return true if the predicate was satisfied at least once, false if upon the
// completion of the visit the predicate has never been satisfied.
bool HexBoard::visitUntil(int r, int c, Player p, function<bool(int, int)> f) const {
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
        if (f(r, c)) {
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

            // Does not belong to this player
            if (this->playerAt(adj.first, adj.second) != p) {
                continue;
            }

            // Add to list of cells to be visited
            l.push_back(make_pair(adj.first, adj.second));
        }
    }
    return false;
}


Player HexBoard::won() {

    // Check if X has won : the other end can be reached starting from the
    // first column of any row
    for (int r = 0; r < m_size; r++) {

        // Does not belong to X hence we can't start visiting from here
        if (playerAt(r, 0) != Player::X)
            continue;

        // Start a graph visit from this cell
        if (visitUntil(r,
                       0,
                       Player::X,
                       [this](int r, int c) { return c == this->m_size - 1; })) {
            return Player::X;
        }
    }

    // Check if O has won: the other end can be reached starting from the
    // first row of any column
    for (int c = 0; c < m_size; c++) {

        if (playerAt(0, c) != Player::O)
            continue;

        if (visitUntil(0,
                       c,
                       Player::O,
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
    return m_owners[r * m_size + c];
}

Player HexTrial::single(Player next) {

    // TODO: exception study it a little bit better
    if (next == Player::NONE) {
        throw "Error can't do a trial with next move assigned to player NONE";
    }

    vector<Player> board = m_board.board();
    vector<int> emptyCellsIndex(m_board.size() - m_board.numberOfMoves());
    int c = 0;
    for (int i = 0; i < m_board.size(); i++) {

        // Fill up empty cell indexes
        if (board[i] == Player::NONE)
            emptyCellsIndex[c++] = i;
    }

    // Shuffle them around
    random_shuffle(emptyCellsIndex.begin(), emptyCellsIndex.end());

    // Play the rest of the game
    for (auto p : emptyCellsIndex) {
        //board[p].m_owner = next;
        next = (next == Player::X) ? Player::O : Player::X;
    }
    return Player::NONE;
}


int main(int argn, char **argv) {
    int size;
    Player current = Player::X;

    cout << "Size of the board:" << endl;
    cin >> size;
    HexBoard c(size);
    cout << c;

    int row, column;
    while (true) {
        cout << "X plays horizontally, O vertically" << endl;
        cout << "Next move '" << current << "'" << endl;
        cin >> row;
        cin >> column;
        auto success = c.move(current, row, column);
        if (!success) {
            cout << "Illegal move" << endl;
            continue;
        }
        auto p = c.won();
        if (p != Player::NONE) {
            cout << p << " has won the game!" << endl;
            cout << c;
            break;
        }
        cout << c;
        current = (current == Player::X) ? Player::O : Player::X;
    }
    return 0;
}