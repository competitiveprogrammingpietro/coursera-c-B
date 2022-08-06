//
// C++ for C programmers part B, first homework
//
// August 2022, Pietro Paolini, London.
//
#include <iostream>
#include "HexBoard.h"

HexCell::HexCell(vector<bool> &star) {
    m_owner = Player::NONE;
    for (int i = 0; i < star.size(); i++)
    {
        m_star[i] = star[i];
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
        if (p == Player::X)
        {
            cell = 'X';
        }
        if (p == Player::O)
        {
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
    for (int i = 0; i < b.m_size; i++)
    {
        os << string(i*2, ' ');
        for (int j = 0; j < b.m_size - 1; j++)
        {
            auto c = cellToChar(b.m_cells[i * b.m_size + j].m_owner);
            os << c << " - ";
        }
        os << cellToChar(b.m_cells[i * b.m_size + b.m_size - 1].m_owner) << endl;

        // Don't print edges below the last line
        if (i == b.m_size - 1)
        {
            continue;
        }

        // Now print connection lines
        os << string(i*2 + 1, ' ');

        for (int z = 0; z < b.m_size * 2 - 1; z++)
        {
           os << edgeToChar(z) << ' ';
        }
        os << endl;
    }
}

bool HexBoard::Move(Player &p, int r, int c) {

    // The cell is not free
    if (Cell(r, c).m_owner != Player::NONE)
    {
        return false;
    }

    // Check first moves: X is the horizontal player while O is the vertical
    if (p == Player::X)
    {
        // Too many levels of indent ? Check Google style
        if (c == 0 || c == m_size - 1)
        {
            Cell(r, c).m_owner = p;
            return true;
        }
    }
    if (p == Player::O)
    {
        if (r == 0 || r == m_size - 1)
        {
            Cell(r, c).m_owner = p;
            return true;
        }
    }

    // We need to check that the star of this cell connects to at least
    // one cell that already belongs to the player
    for (auto cell : CellStar(r, c))
    {
        if (cell.get().m_owner == p)
        {
            Cell(r, c).m_owner = p;
            return true;
        }
    }
    return false;
}

HexCell& HexBoard::Cell(int r, int c) {
    if (r > m_size || c > m_size)
    {
        throw "Indexes out of range";
    }
    return m_cells[r * m_size + c];
}

vector<reference_wrapper<HexCell>> HexBoard::CellStar(int r, int c) {
    vector<reference_wrapper<HexCell>> star;

    // TODO: Move and forward constructors, this isn't that efficient
    // Above left
    if (r - 1 >= 0 && c - 1 >= 0)
    {
        star.push_back(Cell(r - 1, c - 1));
    }

    // Above right
    if (r - 1 >= 0 && c + 1 < m_size)
    {
        star.push_back(Cell(r - 1, c + 1));
    }

    // Backwards
    if (c - 1 >= 0)
    {
        star.push_back(Cell(r, c - 1));
    }

    // Forward
    if (c + 1 < m_size)
    {
        star.push_back(Cell(r, c + 1));
    }

    // Below left
    if (r + 1 < m_size && c - 1 >= 0)
    {
        star.push_back(Cell(r + 1, c - 1));
    }

    // Below right
    if (r + 1 < m_size && c + 1 < m_size)
    {
        star.push_back(Cell(r + 1, c + 1));
    }
    return star;
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
        cout << "Move player " << current << endl;
        cin >> row;
        cin >> column;
        auto success = c.Move(current, row, column);
        if (!success)
        {
            cout << "Illegal move" << endl;
            continue;
        }
        cout << c;
        current = (current == Player::X) ? Player::O : Player::X;
    }
}