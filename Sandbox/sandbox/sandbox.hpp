//  Copyright © 2020 Subph. All rights reserved.
//

#pragma once

#include "settings.h"

namespace sandbox {
class Sandbox {
    
public:
    Sandbox();
    
private:
    float lag = 0;
    float lastTime = 0;
    
    Settings settings;
    
    void createSettings();
    void run();
    void updateTime();
    void sleepTime();
    
};
}
