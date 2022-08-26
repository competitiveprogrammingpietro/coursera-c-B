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
    EASY = 100,
    MEDIUM = 200,
    HARD = 300
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
    vector<HexCell> m_graph;
    int             m_size;
};

class HexBoard {
public:
    HexBoard(int size, Player start, Player ai);
    Player won();
    Player current();
    Player next();
    void play();
    bool move(int r, int c);
    Player playerAt(int r, int c) const ;
    vector<Player> board() const { return m_board; };
    friend ostream &operator<<(ostream &os, const HexBoard &p);
protected:
    Player won(vector<Player>& board);
private:
    pair<int, int> moveAI();
    Player trial(Player next, vector<Player>& board);
    int             m_moves;
    int             m_size;
    vector<Player>  m_board;
    HexGraph        m_graph;
    Player          m_current;
    Player          m_ai;
};
#endif //COURSERA_C_B_HEXBOARD_H
