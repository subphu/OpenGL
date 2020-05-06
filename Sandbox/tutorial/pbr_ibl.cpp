//  Copyright © 2020 Subph. All rights reserved.
//

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>
#include <math.h>
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

void Tutorial::runPbrIbl() {
    
    System &system = System::instance();
    Size<int> size = system.getFramebufferSize();
    float ratio = (float)size.width / (float)size.height;

    Camera camera(glm::vec3(0.0f, 0.0f, 12.0f));
    
    glEnable(GL_DEPTH_TEST);
    // set depth function to less than AND equal for skybox depth trick.
    glDepthFunc(GL_LEQUAL);
    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    
    Mesh sphere = Mesh();
    sphere.createSphere(30, 60);
    sphere.genVBO();
    sphere.genVAO();
    sphere.genEBO();

    Mesh cube = Mesh();
    cube.createCube();
    cube.genVBO();
    cube.genVAO();
    cube.genEBO();

    Mesh quad = Mesh();
    quad.createQuad();
    quad.genVBO();
    quad.genVAO();
    quad.genEBO();
    
    Shader shader = Shader();
    shader.addShaderFrom("shader/pbr/pbr_ibl.vert", GL_VERTEX_SHADER);
    shader.addShaderFrom("shader/pbr/pbr_ibl.frag", GL_FRAGMENT_SHADER);
    shader.compile();
    
    Shader cubemapShader = Shader();
    cubemapShader.addShaderFrom("shader/pbr/cubemap.vert", GL_VERTEX_SHADER);
    cubemapShader.addShaderFrom("shader/pbr/equirectangular.frag", GL_FRAGMENT_SHADER);
    cubemapShader.compile();
    
    Shader irradianceShader = Shader();
    irradianceShader.addShaderFrom("shader/pbr/cubemap.vert", GL_VERTEX_SHADER);
    irradianceShader.addShaderFrom("shader/pbr/irradiance.frag", GL_FRAGMENT_SHADER);
    irradianceShader.compile();
    
    Shader prefilterShader = Shader();
    prefilterShader.addShaderFrom("shader/pbr/cubemap.vert", GL_VERTEX_SHADER);
    prefilterShader.addShaderFrom("shader/pbr/prefilter.frag", GL_FRAGMENT_SHADER);
    prefilterShader.compile();
    
    Shader brdfShader = Shader();
    brdfShader.addShaderFrom("shader/pbr/brdf.vert", GL_VERTEX_SHADER);
    brdfShader.addShaderFrom("shader/pbr/brdf.frag", GL_FRAGMENT_SHADER);
    brdfShader.compile();
    
    Shader backgroundShader = Shader();
    backgroundShader.addShaderFrom("shader/pbr/background.vert", GL_VERTEX_SHADER);
    backgroundShader.addShaderFrom("shader/pbr/background.frag", GL_FRAGMENT_SHADER);
    backgroundShader.compile();
    
    unsigned int albedo[4];
    unsigned int normal[4];
    unsigned int metallic[4];
    unsigned int roughness[4];
    unsigned int ao[4];
    
    std::string texture = "resources/pbr/rustediron/rustediron";
    albedo[0]    = loadTexture((texture + "_albedo.png").c_str());
    normal[0]    = loadTexture((texture + "_normal.png").c_str());
    metallic[0]  = loadTexture((texture + "_metallic.png").c_str());
    roughness[0] = loadTexture((texture + "_roughness.png").c_str());
    ao[0]        = loadTexture((texture + "_ao.png").c_str());
    
    texture = "resources/pbr/roughrockface/roughrockface";
    albedo[1]    = loadTexture((texture + "_albedo.png").c_str());
    normal[1]    = loadTexture((texture + "_normal.png").c_str());
    metallic[1]  = loadTexture((texture + "_metallic.png").c_str());
    roughness[1] = loadTexture((texture + "_roughness.png").c_str());
    ao[1]        = loadTexture((texture + "_ao.png").c_str());
    
    texture = "resources/pbr/goldscuffed/goldscuffed";
    albedo[2]    = loadTexture((texture + "_albedo.png").c_str());
    normal[2]    = loadTexture((texture + "_normal.png").c_str());
    metallic[2]  = loadTexture((texture + "_metallic.png").c_str());
    roughness[2] = loadTexture((texture + "_roughness.png").c_str());
    ao[2]        = loadTexture((texture + "_ao.png").c_str());
    
    texture = "resources/pbr/greasypan/greasypan";
    albedo[3]    = loadTexture((texture + "_albedo.png").c_str());
    normal[3]    = loadTexture((texture + "_normal.png").c_str());
    metallic[3]  = loadTexture((texture + "_metallic.png").c_str());
    roughness[3] = loadTexture((texture + "_roughness.png").c_str());
    ao[3]        = loadTexture((texture + "_ao.png").c_str());
    
    // load HDR environment map
    stbi_set_flip_vertically_on_load(true);
    int width, height, colorChannels;
    std::string hdrPath = "resources/hdr/Arches_E_PineTree/Arches_E_PineTree.hdr";
    float *data = stbi_loadf(hdrPath.c_str(), &width, &height, &colorChannels, 0);
    
    unsigned int hdrTexture = 0;
    if (data) {
        LOG "Read hdr" ENDL;
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else { std::cout << "Failed to load HDR image" << std::endl; }
    
    // setup framebuffer
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // set up projection and view matrices for capturing data onto the 6 cubemap face directions
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // setup cubemap to render to and attach to framebuffer
    unsigned int cubemap;
    glGenTextures(1, &cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    for (unsigned int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 1024, 1024, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // convert HDR equirectangular environment map to cubemap equivalent
    cubemapShader.use();
    cubemapShader.setUniform1i("equirectangularMap", 0);
    cubemapShader.setUniformMatrix4fv("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, 1024, 1024); // configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; i++) {
        cubemapShader.setUniformMatrix4fv("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cube.draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
    unsigned int irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
    
    // solve diffuse integral by convolution to create an irradiance (cube)map.
    irradianceShader.use();
    irradianceShader.setUniform1i("environmentMap", 0);
    irradianceShader.setUniformMatrix4fv("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; i++) {
        irradianceShader.setUniformMatrix4fv("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cube.draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
    unsigned int prefilterMap;
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minifcation filter to mip_linear
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    prefilterShader.use();
    prefilterShader.setUniform1i("environmentMap", 0);
    prefilterShader.setUniformMatrix4fv("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; mip++) {
        // resize framebuffer according to mip-level size.
        unsigned int mipWidth  = 128 * std::pow(0.5, mip);
        unsigned int mipHeight = 128 * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader.setUniform1f("roughness", roughness);
        for (unsigned int i = 0; i < 6; i++) {
            prefilterShader.setUniformMatrix4fv("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            cube.draw();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // generate a 2D LUT from the BRDF equations used.
    unsigned int brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 1024, 1024, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, 1024, 1024);
    brdfShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    quad.draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // initialize static shader uniforms before rendering
    backgroundShader.use();
    backgroundShader.setUniform1i("environmentMap", 0);
    backgroundShader.setUniformMatrix4fv("projection", camera.getProjection(ratio));
    glViewport(0, 0, size.width, size.height);
    
    shader.use();
    shader.setUniform1i("irradianceMap", 0);
    shader.setUniform1i("prefilterMap", 1);
    shader.setUniform1i("brdfLUT", 2);
    shader.setUniform1i("albedoMap", 3);
    shader.setUniform1i("normalMap", 4);
    shader.setUniform1i("metallicMap", 5);
    shader.setUniform1i("roughnessMap", 6);
    shader.setUniform1i("aoMap", 7);
    
    shader.setUniform1f("defAo", 1.0f);
    shader.setUniformMatrix4fv("projection", camera.getProjection(ratio));
    
    
    int lightTotal = 4;
    glm::vec3 lightColor = glm::vec3(200.0f, 200.0f, 200.0f);
    
    float rows = 7.f;
    float colums = 7.f;
    float spacing = 2.5;
    
    float lag = 0;
    float lastTime = system.getTime();
    float frameDelay = 1.f/60.f;
    
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
        
        // bind pre-computed IBL data
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
        
        glm::mat4 model = glm::mat4(1.0f);
        shader.setUniform3f("defAlbedo", 1.0f, 1.0f, 1.0f);
        for (float i = 0.; i < rows; i++) {
            shader.setUniform1f("defMetallic", i / rows);
            
            for (float j = 0.; j < colums; j++) {
                float maxi = rows - 1;
                float maxj = colums - 1;
                float r = pow(fmin(i / maxi, j / maxj), 2) * 0.6 + 0.01;
                float g = pow(fmin(1 - (i / maxi), j / maxj), 2) * 0.6 + 0.01;
                float b = pow(fmin(i / maxi, 1 - (j / maxj)), 2) * 0.6 + 0.01;
                
                shader.setUniform3f("defAlbedo", r, g, b);
                shader.setUniform1i("useTexture", 0);
                shader.setUniform1f("defRoughness", glm::clamp(j / colums, 0.05f, 1.0f));
                
                float x = j - colums / 2 + 0.5f;
                float y = i - rows   / 2 + 0.5f;

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(x * spacing, y * spacing, 0.f));
                shader.setUniformMatrix4fv("model", model);
                sphere.draw();
            }
        }
        
        int ctr = 0;
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, albedo[ctr]);
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, normal[ctr]);
                glActiveTexture(GL_TEXTURE5);
                glBindTexture(GL_TEXTURE_2D, metallic[ctr]);
                glActiveTexture(GL_TEXTURE6);
                glBindTexture(GL_TEXTURE_2D, roughness[ctr]);
                glActiveTexture(GL_TEXTURE7);
                glBindTexture(GL_TEXTURE_2D, ao[ctr]);
                ctr++;
                
                shader.setUniform1i("useTexture", 1);
                
                float x = j - .5f;
                float y = i - .5f;

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(x * spacing, y * spacing, 5.f));
                shader.setUniformMatrix4fv("model", model);
                sphere.draw();
            }
        }

        shader.setUniform3f("defAlbedo", 1.0f, 1.0f, 1.0f);
        for (int i = 0; i < lightTotal; i++) {
            shader.setUniform1i("useTexture", 0);
            glm::vec3 pos = glm::vec3(0.f, 0.f, 12.f);
            pos.x += sin(lastTime / 2 + i * 1.5708) * 5.f;
            pos.y += cos(lastTime / 2 + i * 1.5708) * 5.f;
            
            shader.setUniform3f("lightPositions[" + std::to_string(i) + "]", pos);
            shader.setUniform3f("lightColors[" + std::to_string(i) + "]", lightColor);
            
            model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            model = glm::scale(model, glm::vec3(0.5f));
            shader.setUniformMatrix4fv("model", model);
            
            sphere.draw();
        }
        
        backgroundShader.use();
        backgroundShader.setUniformMatrix4fv("view", camera.getViewMatrix());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
        cube.draw();
        
        system.swapBuffer();
        system.pollEvents();
    }
    
    sphere.removeBuffer();
}
