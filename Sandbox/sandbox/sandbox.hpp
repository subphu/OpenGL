//  Copyright © 2020 Subph. All rights reserved.
//

#pragma once

#include <iostream>

namespace sandbox {
class Sandbox {
    
public:
    Sandbox();
    
private:
    float lag = 0;
    float lastTime = 0;
    
    void run();
    void updateTime();
    void sleepTime();
    
};
}
