//  Copyright © 2020 Subph. All rights reserved.
//

#pragma once

#include "state_machine/state.h"
#include "state_machine/state_machine.hpp"
#include "shader/shader.h"

namespace sandbox {
class MainState: public State {
    
public:
    MainState();
    ~MainState();
    
    void load();
    void update();
    void draw();
    
private:
    
};
}
