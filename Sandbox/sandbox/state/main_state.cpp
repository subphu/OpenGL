//  Copyright © 2020 Subph. All rights reserved.
//

#include "main_state.hpp"
#include "define.h"
#include "shader/shader.h"
#include "system/system.h"
#include "system/settings.h"

namespace sandbox {

MainState::~MainState() {}
MainState::MainState() {
    settings = System::instance().settings;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_FRAMEBUFFER_SRGB);
    
    mesh.createCube();
    mesh.genBuffer();
    
    meshShader.addShaderFrom("shader/tutorial/light.vert", GL_VERTEX_SHADER);
    meshShader.addShaderFrom("shader/tutorial/light.frag", GL_FRAGMENT_SHADER);
    meshShader.compile();
}

void MainState::load() {
    
}

void MainState::update() {
    
}

void MainState::draw() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    meshShader.use();
    meshShader.setUniform3f("lightColor", glm::vec3(0.1f));
    meshShader.setUniformMatrix4fv("projection", camera.getProjection(settings->ratio()));
    meshShader.setUniformMatrix4fv("view", camera.getViewMatrix());
    meshShader.setUniform2f("screenSize", settings->bufferSize().width, settings->bufferSize().height);
    meshShader.setUniformMatrix4fv("model", mesh.getMatrix());
    mesh.draw();
}

}
