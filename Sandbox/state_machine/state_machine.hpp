#pragma once

#include <memory>
#include <stack>
#include "state.h"

using StateRef = std::shared_ptr<State>;

class StateMachine {
    
public:
    StateMachine(const StateRef& state);
    
    static StateRef getCurrentState();
    static StateRef popState();
    
    static void pushState(const StateRef& state);
    
private:
    static std::stack<StateRef> stack;
    
};
