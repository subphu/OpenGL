//  Copyright © 2020 Subph. All rights reserved.
//

#include "tutorial.h"

#include "define.h"
#include "system/system.h"

using namespace tutorial;

Tutorial::Tutorial() {
    System &system = System::instance();
    
    system.settingWindow();
    system.createWindow(900, 900, "Tutorial");
    
    system.initGlLibrary();
    system.settingInput();
    
    runInstancing();
    
    system.closeWindow();
}
