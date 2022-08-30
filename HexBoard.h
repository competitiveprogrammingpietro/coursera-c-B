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

enum class LEVEL : int {
    EASY = 1000,
    MEDIUM = 2000,
    HARD = 3000
};

class HexCell {
public:
    HexCell(vector<bool>& star);
    pair<int, int> TraverseEdge(int r, int c, LINK l);

private:
    bool m_star[6];
};

class HexGraph {
public:
    HexGraph(int size);
    const HexCell& cellAt(int r, int c) const;
    bool visitUntil(int r,
                    int c,
                    function<bool(int, int)> fvisit,
                    function<bool(int, int)> fstop) const;
private:
    vector<shared_ptr<HexCell>> m_graph;
    int                         m_size;
};

class HexBoard {
public:
    HexBoard(int size, Player start);
    Player won();
    void play();
    int size()       { return m_size; }
    int movesNumber() { return m_moves; }
    Player current() { return m_current; }
    bool move(int r, int c);
    void undoMove(int r, int c);
    Player playerAt(int r, int c) const ;
    friend ostream &operator<<(ostream &os, const HexBoard &p);

protected:
    int             m_moves;
    int             m_size;
    vector<Player>  m_board;
    HexGraph        m_graph;
    Player          m_current;
};

class HexAI {
public:
    HexAI(LEVEL l, HexBoard& board): m_board(board), m_level(l) { };
    pair<int, int> nextMove();

protected:
    Player trial();
    LEVEL    m_level;
    HexBoard m_board;
};
#endif //COURSERA_C_B_HEXBOARD_H
