//  Copyright © 2020 Subph. All rights reserved.
//

#pragma once

#include "state_machine/state.h"
#include "state_machine/state_machine.hpp"
#include "system/settings.h"
#include "shader/shader.h"
#include "camera/camera.h"
#include "object/mesh.h"

namespace sandbox {
class MainState: public State {
    
public:
    MainState();
    ~MainState();
    
    void load();
    void update();
    void draw();
    
private:
    Settings* settings;
    Camera camera;
    Mesh mesh;
    Shader meshShader;
};
}
