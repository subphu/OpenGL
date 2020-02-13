//  Copyright © 2020 Subph. All rights reserved.
//

#pragma once

#include <iostream>

#include "define.h"

namespace sandbox {
    class Settings {
        
    public:
        std::string name = "Sandbox";
        unsigned int width = 900;
        unsigned int height = 900;
        
        
        static Settings& instance() {
            static Settings instance;
            return instance;
        }
        
    private:
        Settings();
        ~Settings();

        Settings(Settings const&);
        void operator=(Settings const&);
    };
}
