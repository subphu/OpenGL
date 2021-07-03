//  Copyright © 2020 Subph. All rights reserved.
//

#include "tutorial.h"

#include "define.h"
#include "system/system.h"

using namespace tutorial;

Tutorial::Tutorial() {
    System &system = System::instance();
    
    system.settingWindow();
    system.createWindow(1280, 720, "Tutorial");
    
    system.initGlLibrary();
    system.settingInput();
    system.settingUI();;
    
    runPbrIbl();
    
    system.closeWindow();
}
