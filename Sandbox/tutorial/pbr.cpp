//  Copyright © 2020 Subph. All rights reserved.
//

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>
#include <unistd.h>

#include "tutorial.h"
#include "define.h"
#include "shader/shader.h"
#include "camera/camera.h"
#include "system/system.h"
#include "libraries/stb_image/stb_image.h"
#include "object/mesh.h"

using namespace tutorial;

unsigned int loadTexture(const char *path);

void Tutorial::runPbr() {
    
    System &system = System::instance();
    Size<int> size = system.getFramebufferSize();
    float ratio = (float)size.width / (float)size.height;
    
    glEnable(GL_DEPTH_TEST);
    
    Mesh sphere = Mesh();
    sphere.createSphere(25, 50);
    
    sphere.genVBO();
    sphere.genVAO();
    sphere.genEBO();
    
    Shader shader = Shader();
    shader.addShaderFrom("shader/pbr/pbr.vert", GL_VERTEX_SHADER);
    shader.addShaderFrom("shader/pbr/pbr.frag", GL_FRAGMENT_SHADER);
    shader.compile();
    
    shader.use();
    shader.setUniform1i("albedoMap", 0);
    shader.setUniform1i("normalMap", 1);
    shader.setUniform1i("metallicMap", 2);
    shader.setUniform1i("roughnessMap", 3);
    shader.setUniform1i("aoMap", 4);
    
    std::string texture = "resources/pbr/rustediron/rustediron";
    unsigned int albedo    = loadTexture((texture + "_albedo.png").c_str());
    unsigned int normal    = loadTexture((texture + "_normal.png").c_str());
    unsigned int metallic  = loadTexture((texture + "_metallic.png").c_str());
    unsigned int roughness = loadTexture((texture + "_roughness.png").c_str());
    unsigned int ao        = loadTexture((texture + "_ao.png").c_str());

    int lightTotal = 4;
    glm::vec3 lightColor = glm::vec3(150.0f, 150.0f, 150.0f);
    
    float rows = 7.f;
    float colums = 7.f;
    float spacing = 2.5;
    
    float lag = 0;
    float lastTime = system.getTime();
    float frameDelay = 1.f/60.f;

    Camera camera(glm::vec3(0.0f, 2.0f, 8.0f));
    shader.setUniformMatrix4fv("projection", camera.getProjection(ratio));
    
    system.disableCursor();
    
    shader.setUniform3f("defAlbedo", 0.5f, 0.0f, 0.0f);
    shader.setUniform1f("defAo", 1.0f);

    while (system.getWindowState()) {
        if (system.getKeyState(key_esc)) system.closeWindow();
        
        lag += system.getTime() - lastTime;
        lastTime = system.getTime();
        
        if (lag < frameDelay) {
            usleep((frameDelay - lag) * 1000000);
            lag += system.getTime() - lastTime;
            lastTime = system.getTime();
        }
        
        while (lag > frameDelay) {
            glm::vec3 movement = glm::vec3(0.f, 0.f, 0.f);
            movement.x = system.getKeyState(key_d) - system.getKeyState(key_a);
            movement.y = system.getKeyState(key_q) - system.getKeyState(key_e);
            movement.z = system.getKeyState(key_w) - system.getKeyState(key_s);
            camera.move(movement);
            
            glm::vec2 delta = system.getCursorMovement();
            camera.turn(delta);
            
            lag -= frameDelay;
        }
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.use();
        shader.setUniform3f("viewPos", camera.getPosition());
        shader.setUniformMatrix4fv("view", camera.getViewMatrix());
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedo);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, metallic);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, roughness);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, ao);
        
        glm::mat4 model = glm::mat4(1.0f);
        for (int i = 0; i < rows; i++) {
            shader.setUniform1f("defMetallic", (float)i / (float)rows);
            
            for (int j = 0; j < colums; j++) {
                shader.setUniform1f("defRoughness", glm::clamp((float)j / (float)colums, 0.05f, 1.0f));
                
                int x = j - colums / 2 + 0.5f;
                int y = i - rows   / 2 + 0.5f;

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(x * spacing, y * spacing, 0.f));
                shader.setUniformMatrix4fv("model", model);

                sphere.draw();
            }
        }
        
        for (int i = 0; i < lightTotal; i++) {
            glm::vec3 pos = glm::vec3(0.f, 0.f, 10.f);
            pos.x += sin(lastTime / 2 + i * 1.5708) * 8.f;
            pos.y += cos(lastTime / 2 + i * 1.5708) * 8.f;
            
            shader.setUniform3f("lightPositions[" + std::to_string(i) + "]", pos);
            shader.setUniform3f("lightColors[" + std::to_string(i) + "]", lightColor);
            
            model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            model = glm::scale(model, glm::vec3(0.5f));
            shader.setUniformMatrix4fv("model", model);
            
            glDrawElements(GL_TRIANGLES, (int)sphere.indices.size(), GL_UNSIGNED_INT, 0);
        }
        
        system.swapBuffer();
        system.pollEvents();
    }
    
    sphere.removeBuffer();
}
