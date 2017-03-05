#ifndef STATENODE_H
#define STATENODE_H

#define NO_TURN     0
#define RIGHT_TURN  1
#define U_TURN      2
#define LEFT_TURN   3

class Statenode
{
public:
    Statenode();
    Statenode(int nsOff, int nsOn, int tOff, int tOn, bool wOff, bool wOn);
    ~Statenode();
    
    int next_state [2];
    int turn [2];
    bool write [2];
};

#endif
