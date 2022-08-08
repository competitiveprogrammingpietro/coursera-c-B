//
// C++ for C programmers part B, first homework
//
// August 2022, Pietro Paolini, London.
//
#include <iostream>
#include <list>
#include "HexBoard.h"

HexCell::HexCell(vector<bool> &star) {
    m_owner = Player::NONE;
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

HexBoard::HexBoard(int size) {
    vector<bool> star(6, true);
    m_cells.reserve(size * size);
    m_size = size;

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
           m_cells[i * size + j] = HexCell(star);

           // Reset all values
           fill(star.begin(), star.end(), true);
        }
    }
    return;
}

// It isn't as daunting as it seems:
//. - . - . - . - . - . - .
// \ / \ / \ / \ / \ / \ / \
//  . - . - . - . - . - . - .
//   \ / \ / \ / \ / \ / \ / \
//    . - . - . - . - . - . - .

ostream &operator<<(ostream &os, const HexBoard &b) {

    os << "X is the horizontal player, O the vertial" << endl;
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
            auto c = cellToChar(b.m_cells[i * b.m_size + j].m_owner);
            os << c << " - ";
        }
        os << cellToChar(b.m_cells[i * b.m_size + b.m_size - 1].m_owner) << endl;

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

bool HexBoard::move(Player &p, int r, int c) {

    // The cell is not free
    if (cell(r, c).m_owner != Player::NONE) {
        return false;
    }

    // Check first moves: X is the horizontal player while O is the vertical
    if (p == Player::X) {
        // Too many levels of indent ? Check Google style
        if (c == 0 || c == m_size - 1)
        {
            cell(r, c).m_owner = p;
            return true;
        }
    }
    if (p == Player::O) {
        if (r == 0 || r == m_size - 1)
        {
            cell(r, c).m_owner = p;
            return true;
        }
    }

    // We need to check that the star of this cell connects to at least
    // one cell that already belongs to the player
    for (auto cell : star(r, c)) {
        if (cell.get().m_owner == p) {
            this->cell(r, c).m_owner = p;
            return true;
        }
    }
    return false;
}

HexCell& HexBoard::cell(int r, int c) {
    if (r > m_size || c > m_size)
    {
        throw "Indexes out of range";
    }
    return m_cells[r * m_size + c];
}

vector<reference_wrapper<HexCell>> HexBoard::star(int r, int c) {
    vector<reference_wrapper<HexCell>> star;

    // TODO: move and forward constructors, this isn't that efficient
    // Above left
    if (r - 1 >= 0)
    {
        star.push_back(cell(r - 1, c));
    }

    // Above right
    if (r - 1 >= 0 && c + 1 < m_size)
    {
        star.push_back(cell(r - 1, c + 1));
    }

    // Backwards
    if (c - 1 >= 0)
    {
        star.push_back(cell(r, c - 1));
    }

    // Forward
    if (c + 1 < m_size)
    {
        star.push_back(cell(r, c + 1));
    }

    // Below left
    if (r + 1 < m_size && c - 1 >= 0)
    {
        star.push_back(cell(r + 1, c - 1));
    }

    // Below right
    if (r + 1 < m_size)
    {
        star.push_back(cell(r + 1, c));
    }
    return star;
}

bool HexBoard::visitUntil(int r, int c, Player p, function<bool(int, int, HexCell &)> f) {
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
        auto cell = this->cell(r, c);

        l.pop_front();

        // Termination case
        if (f(r, c, cell)) {
            return true;
        }

        // Mark it so it is not visited next time
        visited[r * m_size + c] = true;
        for (auto i = 0; i < 6; i++) {
            auto adj = cell.TraverseEdge(r, c, static_cast<LINK>(i));

            // Out of bound
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
            if (this->cell(adj.first, adj.second).m_owner != p) {
                continue;
            }

            // Add to list of cells to be visited
            l.push_back(make_pair(adj.first, adj.second));
        }
    }
    return false;
}

bool HexBoard::hasWon(Player &p) {

    if (p == Player::X) {

        // Check if the other end can be reached starting from the first column of any row
        for (int r = 0; r <  m_size; r++) {
            if (visitUntil(r,
                           0,
                           Player::X,
                           [this](int r, int c, HexCell& cell)
                           { return c == this->m_size - 1; })) {
                return true;
            }
        }
        return false;
    }
    else if (p == Player::O) {

        // Check if the other end can be reached starting from the first row of any column
        for (int c = 0; c <  m_size; c++) {
            if (visitUntil(0,
                           c,
                           Player::O,
                           [this](int r, int c, HexCell& cell)
                           { return r == this->m_size - 1; })) {
                return true;
            }
        }
        return false;
    }
    else {
        throw "Error:NONE player cannot win";
    }
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
        cout << "move player " << current << endl;
        cin >> row;
        cin >> column;
        auto success = c.move(current, row, column);
        if (!success) {
            cout << "Illegal move" << endl;
            continue;
        }
        if (c.hasWon(current)) {
            cout << current << " has won the game!" << endl;
            cout << c;
            break;
        }
        cout << c;
        current = (current == Player::X) ? Player::O : Player::X;
    }
    return 0;
}