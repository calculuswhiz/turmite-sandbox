#include "Automaton.h"
Automaton::Automaton()
{
    posx = 0;
    posy = 0;
    curState = 0;
    orientation = 0;
    alive = true;
    name = to_string((long)this);
    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;
}

Automaton::Automaton(int x, int y=0, int s=0, int o=0)
{
    posx = x;
    posy = y;
    curState = s;
    orientation = o;
    alive = true;
    name = to_string((long)this);
    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;
}

Automaton::Automaton(const Automaton & a)
{
    posx = a.posx;
    posy = a.posy;
    curState = a.curState;
    orientation = a.orientation;
    alive = a.alive;
    color[0] = a.color[0];
    color[1] = a.color[1];
    color[2] = a.color[2];
    
    name = "undefined";
    states = *(new vector<Statenode *>(a.states));
}

Automaton::~Automaton()
{}

void Automaton::addState(int nsOff, int nsOn, int tOff, int tOn, bool wOff, bool wOn)
{
    Statenode * sn = new Statenode(nsOff, nsOn, tOff, tOn, wOff, wOn);
    states.push_back(sn);
}

int Automaton::shouldWrite(bool input)
{
    return states[curState]->write[input];
}

bool Automaton::transition(bool input)
{
    if(isDead())
    {
        return 1;
    }
    switch(states[curState]->turn[input])
    {
        case(LEFT_TURN):
            orientation = (orientation+3)%4;
        break;
        case(RIGHT_TURN):
            orientation = (orientation+1)%4;
        break;
        case(U_TURN):
            orientation = (orientation+2)%4;
        break;
        default:
            orientation = (orientation)%4;
        break;
    }
    
    switch(orientation)
    {
        case(NORTH):
            posy -= 1;
        break;
        case(EAST):
            posx += 1;
        break;
        case(WEST):
            posx -= 1;
        break;
        default:
            posy += 1;
        break;
    }
    
    curState = states[curState]->next_state[input];
    return 1;
}

void Automaton::kill()
{
    alive = false;
    cout << name << " has died.\n";
}

void Automaton::raise()
{
    alive = true;
    cout << name << " is alive.\n";
}

bool Automaton::isDead()
{
    return !alive;
}

void Automaton::setDirFromString(string orient)
{
    if(orient == "N")
    {
        orientation =  NORTH;
    }
    else if(orient == "S")
    {
        orientation =  SOUTH;
    }
    else if(orient == "E")
    {
        orientation =  EAST;
    }
    else if(orient == "W")
    {
        orientation =  WEST;
    }
}

int Automaton::getTurnFromString(string turn)
{
    if(turn == "N")
        return NO_TURN;
    if(turn == "L")
        return LEFT_TURN;
    if(turn == "R")
        return RIGHT_TURN;
    if(turn == "U")
        return U_TURN;
    return -1;
}

void Automaton::setColor(float r, float g, float b)
{
    color[0]=r;
    color[1]=g;
    color[2]=b;
}
