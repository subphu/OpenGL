//  Copyright © 2019 Subph. All rights reserved.
//

#include <iostream>

#include "system.h"

System::System() {}

System::~System() { glfwTerminate(); }

float System::getTime() { return glfwGetTime(); }
