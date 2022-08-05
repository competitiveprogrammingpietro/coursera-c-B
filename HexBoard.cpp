//
// Created by pedr0 on 05/08/22.
//
#include "HexBoard.h"

HexCell::HexCell(std::vector<bool> &star) {
    m_owner = Player::NONE;
    for (int i = 0; i < star.size(); i++)
    {
        m_star[i] = star[i];
    }
}

HexBoard::HexBoard(int size) {
    std::vector<bool> star(6, false);
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
           std::fill(star.begin(), star.end(), true);
        }
    }
    return;
}

// It isn't as daunting as it seems:
//. - . - . - . - . - . - .
// \ / \ / \ / \ / \ / \ / \
//  . - . - . - . - . - . - .
std::ostream &operator<<(std::ostream &os, const HexBoard &b) {

    // They are stored line by line
    for (int i = 0; i < b.m_size; i++)
    {
        for (int j = 0; j < b.m_size; j++)
        {
            char cell = '.';
            if (b.m_cells[i * b.m_size + j].m_owner == Player::X)
            {
                cell = 'X';
            }
            if (b.m_cells[i * b.m_size + j].m_owner == Player::O)
            {
                cell = 'O';
            }

        }
    }
}
