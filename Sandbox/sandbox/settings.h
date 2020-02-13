//  Copyright © 2020 Subph. All rights reserved.
//

#pragma once

namespace sandbox {
    class Settings {
        
    public:
        const char* name = "Sandbox";
        Size<int> windowSize = { 900, 900 };
        Size<int> bufferSize = { 900, 900 };
        float ratio = 1.f;
        
        static Settings& instance() {
            static Settings instance;
            return instance;
        }
        
    private:
        Settings() { }
        ~Settings() { }

        Settings(Settings const&);
        void operator=(Settings const&);
    };
}
