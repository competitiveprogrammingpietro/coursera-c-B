//
// C++ for C programmers part B, first homework
//
// August 2022, Pietro Paolini, London.
//

#ifndef COURSERA_C_B_HEXBOARD_H
#define COURSERA_C_B_HEXBOARD_H

#include <vector>
#include <ostream>
#include <memory>

using namespace std;
enum class Player { NONE, X, O };

ostream& operator << (ostream& os, const Player& p)
{
    if (p == Player::X)
    {
        os << "X";
    }
    if (p == Player::O)
    {
        os << "O";
    }
    return os;
}
enum class LINK : int   { BACKWARDS,
                    ABOVE_LEFT,
                    ABOVE_RIGHT,
                    FORWARD,
                    BELOW_RIGHT,
                    BELOW_LEFT };

class HexCell {
public:
    HexCell():m_owner(Player::NONE) { }
    HexCell(vector<bool> & star);
    Player m_owner;
private:
    bool m_star[6];
};

class HexBoard {
public:
    HexBoard(int size);
    bool Move(Player &p, int r, int c);
    bool won(Player &p);
    friend ostream & operator<<(ostream &os, const HexBoard& p);
private:
    int                  m_size;
    vector<HexCell> m_cells;
    HexCell& Cell(int r, int c);
    vector<reference_wrapper<HexCell>> CellStar(int r, int c);
};

#endif //COURSERA_C_B_HEXBOARD_H
