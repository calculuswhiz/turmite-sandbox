#include "Statenode.h"

Statenode::Statenode(int nsOff, int nsOn, int tOff, int tOn, bool wOff, bool wOn)
{
    next_state[0] = nsOff;
    next_state[1] = nsOn;
    turn[0] = tOff;
    turn[1] = tOn;
    write[0] = wOff;
    write[1] = wOn;
}
