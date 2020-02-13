//  Copyright © 2020 Subph. All rights reserved.
//

#include "sandbox.hpp"

#include "define.h"
#include "system/system.h"
#include "state_machine/state_machine.hpp"

#include "settings.h"

namespace sandbox {
    
    Sandbox::Sandbox() {
        
    }
    
    void Sandbox::run() {
        Settings &settings = Settings::instance();
        System &system = System::instance();
        
        using StateRef = std::shared_ptr<State>;
        
        
    }
}
