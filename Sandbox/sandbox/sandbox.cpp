//  Copyright © 2020 Subph. All rights reserved.
//

#include "sandbox.hpp"

#include "define.h"
#include "system/system.h"
#include "state_machine/state_machine.hpp"

#include "settings.h"

namespace sandbox {
    
    Sandbox::Sandbox() {
        System &system = System::instance();
        Settings &settings = Settings::instance();
        Size<int> size = settings.windowSize;
        
        system.settingWindow();
        system.createWindow(size.width, size.height, settings.name);
        
        settings.bufferSize = system.getFramebufferSize();
        settings.ratio = (float)settings.bufferSize.width / (float)settings.bufferSize.height;
        
        system.initGlLibrary();
        system.settingInput();
        
        run();
        
        system.closeWindow();
    }
    
    void Sandbox::run() {
        using StateRef = std::shared_ptr<State>;
        
    }
}
