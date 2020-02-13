//  Copyright © 2020 Subph. All rights reserved.
//

#include "main_state.hpp"
#include "define.h"
#include "shader/shader.h"
#include "system/system.h"

namespace sandbox {

MainState::MainState() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_FRAMEBUFFER_SRGB);
    
}

MainState::~MainState() {
    
}

void MainState::load() {
    
}

void MainState::update() {
    
}

void MainState::draw() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
}

}
