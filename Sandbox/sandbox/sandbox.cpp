//  Copyright © 2020 Subph. All rights reserved.
//

#include "sandbox.hpp"
#include "define.h"
#include "system/system.h"
#include "state_machine/state_machine.hpp"

#include "settings.h"
#include "state/main_state.hpp"

#include <unistd.h>

namespace sandbox {
    
Sandbox::Sandbox() {
    createSettings();
    System &system = System::instance();
    system.settings = &settings;
    system.settingWindow();
    system.createWindow();
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
    if (settings.frameTime < lag) return;
    usleep((settings.frameTime - lag) * 1000000);
    updateTime();
}

void Sandbox::updateTime() {
    float currentTime = System::instance().getTime();
    lag += currentTime - lastTime;
    lastTime = currentTime;
}

void Sandbox::createSettings() {
    settings.name = "Sandbox";
    settings.setWindowSize({ 1200, 800 });
}

}
