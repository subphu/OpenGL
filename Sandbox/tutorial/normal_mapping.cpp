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

void Tutorial::runNormalMapping() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_FRAMEBUFFER_SRGB);
    
    System &system = System::instance();
    Size<int> size = system.getFramebufferSize();
    float ratio = (float)size.width / (float)size.height;
    
    Shader shader = Shader();
    shader.addShaderFrom("shader/tutorial/normal_map.vert", GL_VERTEX_SHADER);
    shader.addShaderFrom("shader/tutorial/normal_map.frag", GL_FRAGMENT_SHADER);
    shader.compile();
    
    Shader lightShader = Shader();
    lightShader.addShaderFrom("shader/tutorial/light.vert", GL_VERTEX_SHADER);
    lightShader.addShaderFrom("shader/tutorial/light.frag", GL_FRAGMENT_SHADER);
    lightShader.compile();
    
    unsigned long lightVerticesSize;
    float * lightVertices = Mesh::generateCube(lightVerticesSize);
    
    float vertices[] = {
        -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
         1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
        -1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
        -1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
         1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
         1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f
    };
    
    for (int i = 0; i < 84; i += 42) {
        int l = 14;
        glm::vec3 pos1(vertices[i+0*l], vertices[i+0*l+1], vertices[i+0*l+2]);
        glm::vec3 pos2(vertices[i+1*l], vertices[i+1*l+1], vertices[i+1*l+2]);
        glm::vec3 pos3(vertices[i+2*l], vertices[i+2*l+1], vertices[i+2*l+2]);
        
        glm::vec3 nm(vertices[i+3], vertices[i+4], vertices[i+5]);
        
        glm::vec2 uv1(vertices[i+0*l+6], vertices[i+0*l+7]);
        glm::vec2 uv2(vertices[i+1*l+6], vertices[i+1*l+7]);
        glm::vec2 uv3(vertices[i+2*l+6], vertices[i+2*l+7]);

        glm::vec3 tangent, bitangent;
        
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent = glm::normalize(tangent);

        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent = glm::normalize(bitangent);
        
        for (int j = i; j < i+42; j += 14) {
            vertices[j+8 ] = tangent.x;
            vertices[j+9 ] = tangent.y;
            vertices[j+10] = tangent.z;
            vertices[j+11] = bitangent.x;
            vertices[j+12] = bitangent.y;
            vertices[j+13] = bitangent.z;
        }
    }
    
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.0f,  1.2f,  3.0f),
        glm::vec3( 3.0f,  2.5f, -0.0f),
        glm::vec3(-3.0f,  2.0f, -0.0f),
        glm::vec3( 0.0f,  1.5f, -3.0f)
    };
    
    glm::vec3 pointLightColors[] = {
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    };
    
    GLuint planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glBindVertexArray(planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    
    GLuint lightVAO, lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glGenBuffers(1, &lightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, lightVerticesSize, lightVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    unsigned int diffuseMap = loadTexture("resources/texture/brickwall.jpg");
    unsigned int normalMap = loadTexture("resources/texture/brickwall_n.jpg");

    shader.use();
    shader.setUniform1i("diffuseMap", 0);
    shader.setUniform1i("normalMap", 1);
    
    
    float lag = 0;
    float lastTime = system.getTime();
    float frameDelay = 1.f/60.f;

    Camera camera(glm::vec3(0.0f, 2.0f, 8.0f));
    
    system.disableCursor();

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
        shader.setUniformMatrix4fv("projection", camera.getProjection(ratio));
        
        shader.setUniform3f("dirLight.direction", -0.2f, -1.0f, -0.3f);
        shader.setUniform3f("dirLight.light.ambient", 0.05f, 0.05f, 0.05f);
        shader.setUniform3f("dirLight.light.diffuse", 0.4f, 0.4f, 0.4f);
        shader.setUniform3f("dirLight.light.specular", 0.5f, 0.5f, 0.5f);
        
        for(int i = 0; i < 4; i++) {
            std::string idx = std::to_string(i);
            shader.setUniform3f("lights[" + idx + "].position", pointLightPositions[i]);
            shader.setUniform3f("lights[" + idx + "].light.ambient", pointLightColors[i] * 0.05f);
            shader.setUniform3f("lights[" + idx + "].light.diffuse", pointLightColors[i] * 0.9f);
            shader.setUniform3f("lights[" + idx + "].light.specular", pointLightColors[i]);
            shader.setUniform1f("lights[" + idx + "].constant.linear", 0.05);
            shader.setUniform1f("lights[" + idx + "].constant.quadratic", 0.01);
        }
        
        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap);
        
        float time = system.getTime();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(10.0f));
        model = glm::rotate(model, glm::radians(15.f), glm::vec3(cos(time), 0.f, sin(time)));
        shader.setUniformMatrix4fv("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        
        lightShader.use();
        lightShader.setUniformMatrix4fv("projection", camera.getProjection(ratio));
        lightShader.setUniformMatrix4fv("view", camera.getViewMatrix());
        lightShader.setUniform2f("screenSize", size.width, size.height);
        
        glBindVertexArray(lightVAO);
        for (unsigned int i = 0; i < 4; i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lightShader.setUniformMatrix4fv("model", model);
            lightShader.setUniform3f("lightColor", pointLightColors[i]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        system.swapBuffer();
        system.pollEvents();
    }
    
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &lightVBO);
}
