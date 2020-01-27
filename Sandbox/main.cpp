//  Copyright © 2019 Subph. All rights reserved.
//

#include <iostream>

#include "define.h"
#include "basic/basic.h"
#include "system/system.h"

std::string NAME = "Sandbox";
GLuint WIDTH = 900;
GLuint HEIGHT = 900;

int main() {
    System &system = System::instance();
    
    system.settingWindow();
    system.createWindow(WIDTH, HEIGHT, NAME.c_str());
    
    system.initGlLibrary();
    system.settingInput();
    
    runInstancing();
    
    system.closeWindow();
    
    return EXIT_SUCCESS;
}
