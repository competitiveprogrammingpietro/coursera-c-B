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
#include <functional>

using namespace std;
enum class Player {
    NONE, X, O
};

ostream &operator<<(ostream &os, const Player &p) {
    if (p == Player::X) {
        os << "X";
    }
    if (p == Player::O) {
        os << "O";
    }
    return os;
}

enum class LINK : int {
    BACKWARDS,
    ABOVE_LEFT,
    ABOVE_RIGHT,
    FORWARD,
    BELOW_RIGHT,
    BELOW_LEFT
};

class HexCell {
public:
    HexCell(vector<bool>& star);
    pair<int, int> TraverseEdge(int r, int c, LINK l);

private:
    bool m_star[6];
};

class HexBoard {

public:
    HexBoard(int size);
    Player won();
    bool move(Player &p, int r, int c);
    int  numberOfMoves() const { return m_moves; };
    int size() const { return m_size; };
    vector<Player> board() const { return m_owners; };
    friend ostream &operator<<(ostream &os, const HexBoard &p);
private:
    const HexCell &cellAt(int r, int c) const;
    Player playerAt(int r, int c) const ;
    vector<pair<int, int>> star(int r, int c);
    bool visitUntil(int r,
                    int c,
                    Player p,
                    function<bool(int, int)> fv) const;
    int             m_moves;
    int             m_size;
    vector<Player>  m_owners;
    vector<HexCell> m_graph;
};

class HexTrial {
public:
    HexTrial(HexBoard &b): m_board(b) { };
    double run(int number, int r, int c);
private:
    Player single(Player next);
    const HexBoard& m_board;
};
#endif //COURSERA_C_B_HEXBOARD_H
