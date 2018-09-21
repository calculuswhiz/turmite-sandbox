#ifndef AUTOMATON_H
#define AUTOMATON_H

#include <vector>
#include <string>
#include <iostream>
#include "Statenode.h"
using namespace std;

#define NORTH   0
#define EAST    1
#define SOUTH   2
#define WEST    3

class Automaton
{
public:
    Automaton();
    Automaton(int x, int y, int s, int o);
    Automaton(const Automaton & a);
    ~Automaton();
    void addState(int nsOff, int nsOn, int tOff, int tOn, bool wOff, bool wOn);
    int shouldWrite(bool input);
    bool transition(bool input);
    void kill();
    void raise();
    bool isDead();
    void setDirFromString(string orient);
    int getTurnFromString(string turn);
    void setColor(float r, float g, float b);

    int posx;
    int posy;
    int curState;
    int orientation;
    string name;
    float color[3];
    
    bool silent;
    int initialOrientation;
    
private:
    vector<Statenode *> states;
    bool alive;
};

#endif