//  Copyright © 2019 Subph. All rights reserved.
//

#include "state_machine.hpp"

std::stack<StateRef> StateMachine::stack = std::stack<StateRef>();

StateMachine::StateMachine(const StateRef& initialState) {
    pushState(initialState);
}

StateRef StateMachine::getCurrentState() {
    return stack.top();
}

StateRef StateMachine::popState() {
    auto state = getCurrentState();
    stack.pop();
    return state;
}

void StateMachine::pushState(const StateRef& state) {
    stack.emplace(state);
}
