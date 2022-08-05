//
// Created by pedr0 on 05/08/22.
//

#ifndef COURSERA_C_B_HEXBOARD_H
#define COURSERA_C_B_HEXBOARD_H

#include <vector>
#include <ostream>

enum class Player { NONE, X, O };
enum class LINK : int   { BACKWARDS,
                    ABOVE_LEFT,
                    ABOVE_RIGHT,
                    FORWARD,
                    BELOW_RIGHT,
                    BELOW_LEFT };

class HexCell {
public:
    HexCell():m_owner(Player::NONE) { }
    HexCell(std::vector<bool> & star);
    Player m_owner;
private:
    bool m_star[6];
};

class HexBoard {
    HexBoard(int size);
    friend std::ostream & operator<<(std::ostream &os, const HexBoard& p);
private:
    int                  m_size;
    std::vector<HexCell> m_cells;
};

#endif //COURSERA_C_B_HEXBOARD_H
