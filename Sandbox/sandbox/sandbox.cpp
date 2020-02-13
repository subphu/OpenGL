//  Copyright © 2020 Subph. All rights reserved.
//

#include <unistd.h>

#include "sandbox.hpp"

#include "define.h"
#include "system/system.h"
#include "state_machine/state_machine.hpp"

#include "settings.h"
#include "state/main_state.hpp"

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
    
    StateMachine stateMachine = StateMachine(StateRef(new MainState()));
    StateRef state = stateMachine.getCurrentState();
    state->load();
    
    System &system = System::instance();
    Settings &settings = Settings::instance();
    
    lastTime = system.getTime();
    lag = settings.frameTime;
    
    while (system.getWindowState()) {
        system.pollEvents();
        state = stateMachine.getCurrentState();
        state->update();
        lag -= settings.frameTime;
    
        if (lag < settings.frameTime) {
            state->draw();
            updateTime();
            sleepTime();
            system.swapBuffer();
        }
    }
    
}

void Sandbox::sleepTime() {
    float frameTime = Settings::instance().frameTime;
    if (frameTime < lag) return;
    usleep((frameTime - lag) * 1000000);
    updateTime();
}

void Sandbox::updateTime() {
    float currentTime = System::instance().getTime();
    lag += currentTime - lastTime;
    lastTime = currentTime;
}


}
