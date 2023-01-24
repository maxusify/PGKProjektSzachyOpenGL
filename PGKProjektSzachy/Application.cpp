#include <fstream>
#include "Application.hpp"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLFWwindow *Application::window;
Program Application::program;
Program Application::p_Shadows;
Program Application::p_Selection;
Scene Application::scene;
Game Application::game;

double Application::lastTime;
int Application::nbFrames;
int Application::nbFramesLastSecond;
int Application::w_Height;
int Application::w_Width;
int Application::w_MidX;
int Application::w_MidY;
int Application::fb_Width, Application::fb_Height;

bool Application::isSkyBoxOn = false;
bool Application::isShadowOn = false;
bool Application::isLightOn[2];
bool Application::isReflectionOn = false;

/**
 * Start programu
*/
void Application::start()
{
    // Wstępna konfiguracja efektów
    isLightOn[0] = true;
    isLightOn[1] = false;
    isSkyBoxOn = true;
    isShadowOn = true;
    isReflectionOn = true;
    
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    std::cout << "Zainicjalizowano GLFW..." << std::endl;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,  GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    w_Height = 720;
    w_Width = 1280;

    w_MidX = w_Width / 2;
    w_MidY = w_Height / 2;

    window = glfwCreateWindow(
        w_Width, w_Height, 
        "Szachy - Programowanie Grafiki Komputerowej", 
        NULL, NULL
    );
    
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    std::cout << "Utworzono okno..." << std::endl;

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();
    std::cout << "Wersja OpenGL: " << glGetString(GL_VERSION) << std::endl;

    initOpenGL();
    std::cout << "Inicjalizacja komponentow OpenGL..." << std::endl;

    initGame();
    std::cout << "Inicjalizacja obiektu gry..." << std::endl;

    glfwSwapInterval(0);
    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetWindowSizeCallback(window, resizeCallback);

    lastTime = glfwGetTime();
    nbFrames = 0;
    nbFramesLastSecond = 100;

    std::cout << "Pomyslnie zainicjalizowano program!" << std::endl;

    while (!glfwWindowShouldClose(window))
    {
        display();

        // Wyliczanie FPS
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0)
        {
            nbFramesLastSecond = nbFrames;
            nbFrames = 0;
            lastTime += 1.0;
        }

        scene.move(nbFramesLastSecond);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

/**
 * Inicjalizuje komponenty i programy OpenGL
*/
void Application::initOpenGL()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    // Programy
    program.init();
    p_Shadows.initForShadowMap();
    p_Selection.initForSelection();

    scene.initScene(w_Width, w_Height);

    program.use();

    scene.setPerspective(w_Width, w_Height);

    glUniformMatrix4fv(
        glGetUniformLocation(program.getId(), "projection_matrix"), 1, GL_FALSE, scene.getProjectionMatrixArray()
    );
    
    glfwGetFramebufferSize(window, &fb_Width, &fb_Height);
    
    for (unsigned int i = 0; i < scene.getLightCount(); ++i)
    {
        const Light& light = scene.getLight(i);

        std::string pos = "lights[" + std::to_string(i) + "].position";
        std::string dcolor = "lights[" + std::to_string(i) + "].diffuse_color";
        std::string scolor = "lights[" + std::to_string(i) + "].specular_color";

        glUniform3fv(
            glGetUniformLocation(program.getId(), pos.c_str()), 1, &light.getPos()[0]);
        glUniform3fv(
            glGetUniformLocation(program.getId(), dcolor.c_str()), 1, &light.getDiffuseColor()[0]);
        glUniform3fv(
            glGetUniformLocation(program.getId(), scolor.c_str()), 1, &light.getSpecColor()[0]);
    }
}

/**
 * Inicjalizuje grę w szachy
*/
void Application::initGame() {
    game.init(&scene);
}

/**
 * Renderuje poszczególne elementy aplikacji
*/
void Application::display()
{
    for (unsigned int i = 0; i < scene.getLightCount(); ++i)
        renderShadow(i);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Renderowanie
    renderSkybox();
    renderScene();

    glfwSwapBuffers(window);
    glfwPollEvents();
}

/**
 * Renderuje cienie
*/
void Application::renderShadow(int light_index)
{
    p_Shadows.use();
    
    Light &light = scene.getLight(light_index);
    glBindFramebuffer(GL_FRAMEBUFFER , light.getShadowBufferId());
    
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, scene.getShadowSize(), scene.getShadowSize());
    
    const glm::mat4& shadowProjMat = scene.getShadowProjectionMatrix();
    const glm::mat4& shadowViewMat = light.getViewMatrix();

    // Renderuj każdy VAO
    for (unsigned int i = 0; i < scene.size(); ++i)
    {
        const Vao &vao = scene[i];
        const glm::mat4& model_matrix =  vao.getModelMatrix();
        glm::mat4 depthMVP = shadowProjMat * shadowViewMat * model_matrix;

        glUniformMatrix4fv(
            glGetUniformLocation(p_Shadows.getId(), "MVP_matrix"), 
            1, 
            GL_FALSE, 
            &depthMVP[0][0]
        );
       
        glBindVertexArray(vao.getId());
        glDrawArrays(GL_TRIANGLES, 0, vao.getVertexCount());
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER , 0);
}

/**
 * Renderuje scenę
*/
void Application::renderScene()
{
    program.use();

    scene.setView();

    const glm::mat4& viewMat = scene.getViewMatrix();
    const glm::mat4& invViewMat = glm::inverse(viewMat);
    const glm::mat4& projMat = scene.getProjectionMatrix();

    glUniformMatrix4fv(
        glGetUniformLocation(program.getId(), "view_matrix"), 
        1, 
        GL_FALSE, 
        scene.getViewMatrixArray()
    );
    glUniformMatrix4fv(
        glGetUniformLocation(program.getId(), "inv_view_matrix"), 
        1, 
        GL_FALSE, 
        glm::value_ptr(invViewMat)
    );
    glUniform1i(
        glGetUniformLocation(program.getId(), "shadow_enabled"), 
        isShadowOn
    );
    glUniform1i(
        glGetUniformLocation(program.getId(), "reflection_enabled"), 
        isReflectionOn
    );

    for (unsigned int i = 0; i < scene.getLightCount(); ++i)
    {
        std::string uniform = "light_enabled[" + std::to_string(i) + "]";
        glUniform1i(glGetUniformLocation(program.getId(), uniform.c_str()), isLightOn[i]);
    }

    for (unsigned int i = 0 ; i < scene.getLightCount(); ++i)
    {
        std::string uniform = "shadow_text[" + std::to_string(i) + "]";
        glActiveTexture(GL_TEXTURE10 + i);
        glBindTexture(GL_TEXTURE_2D, scene.getLight(i).getShadowTextureId());
        glUniform1i(
            glGetUniformLocation(program.getId(), uniform.c_str()), 
            10 + i
        );
    }

    glViewport(0, 0, fb_Width, fb_Height);
    
    const glm::mat4& shadowProjMat = scene.getShadowProjectionMatrix();
    
    glUniform1i(
        glGetUniformLocation(program.getId(), "skybox_enabled"), 
        0
    );

    for (unsigned int i = 0; i < scene.size(); ++i)
    {
        Vao &vao = scene[i];
        glm::mat4 model_matrix =  vao.getModelMatrix();
        
        if (vao.isMovementRequested() 
            || vao.isJumpMovementRequested() 
            || vao.isEjectMovementRequested())
            vao.updateMovement();

        if (scene.selected() && scene.getSelected() == i)
        {
            glUniform3fv(glGetUniformLocation(program.getId(), "diffuse_color"), 1, scene.getSelectectionColor());
        }
        else
            glUniform3fv(glGetUniformLocation(program.getId(), "diffuse_color"), 1, vao.getDiffuseColorArray());

        for (unsigned int j = 0; j < scene.getLightCount(); ++j)
        {
            const glm::mat4& shadowViewMat = scene.getLight(j).getViewMatrix();

            glm::mat4 depthMVP = shadowProjMat * shadowViewMat * model_matrix;
            glm::mat4 depthBiasMVP = scene.getBiasMatrix() * depthMVP;
            
            std::string uniform = "bias_matrix[" + std::to_string(j) + "]";
            glUniformMatrix4fv(
                glGetUniformLocation(program.getId(), uniform.c_str()), 
                1, 
                GL_FALSE, 
                glm::value_ptr(depthBiasMVP)
            );
        }

        glm::mat4 mv = viewMat * model_matrix;
        glm::mat4 mvp = projMat * mv;

        glUniformMatrix4fv(
            glGetUniformLocation(program.getId(), "proj_view_model"), 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(
            glGetUniformLocation(program.getId(), "view_model"), 1, GL_FALSE, glm::value_ptr(mv));
        glUniform1i(
            glGetUniformLocation(program.getId(), "texture_enabled"), vao.isTextureEnabled());
        glUniform1i(
            glGetUniformLocation(program.getId(), "skybox_enabled"), 0);

        glUniformMatrix4fv(
            glGetUniformLocation(program.getId(), "normal_matrix"), 1, GL_FALSE, scene.getNormalMatrixArray(i));

        if (vao.isTextureEnabled())
        {
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, vao.getTextureId());
            glUniform1i(
                glGetUniformLocation(program.getId(), "object_texture"), 3);
        }
        
        glBindVertexArray(vao.getId());
        glDrawArrays(GL_TRIANGLES, 0, vao.getVertexCount());
    }
    
    glBindVertexArray(0);
}

/**
 * Renderuje podświetlenie
*/
void Application::renderSelection(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    scene.setView();
    p_Selection.use();
    
    const glm::mat4& viewMat = scene.getViewMatrix();
    const glm::mat4& projMat = scene.getProjectionMatrix();
    
    glViewport(0, 0, fb_Width, fb_Height);
    
    for (unsigned int i = 0; i < scene.size(); ++i)
    {
        const Vao &vao = scene[i];
        const glm::mat4& model_matrix =  vao.getModelMatrix();

        glm::mat4 vm = viewMat * model_matrix;
        glm::mat4 mvp = projMat * vm;
        
        glUniformMatrix4fv(
            glGetUniformLocation(p_Selection.getId(), "proj_view_model"), 1, GL_FALSE, glm::value_ptr(mvp));
        
        glUniform1i(
            glGetUniformLocation(p_Selection.getId(), "code"), i);
        
        glUniform1i(
            glGetUniformLocation(p_Selection.getId(), "texture_enabled"), vao.isTextureEnabled());

        glBindVertexArray(vao.getId());
        glDrawArrays(GL_TRIANGLES, 0, vao.getVertexCount());
    }
    
    glBindVertexArray(0);
}

/**
 * Przetwarza elementy podświetlenia
*/
void Application::processSelection(int xx, int yy) 
{    
    unsigned char res[4];
    GLint viewport[4];
    
    renderSelection();
    
    float x_scale, y_scale;
    x_scale = (float) fb_Width / w_Width;
    y_scale = (float) fb_Height / w_Height;
    
    glGetIntegerv(GL_VIEWPORT, viewport);
    glReadPixels(xx*x_scale, viewport[3]-yy*y_scale, 1,1,GL_RGBA, GL_UNSIGNED_BYTE, &res);

    int selected = (int) res[0];
    
    if (selected < 100 && selected >= 0)
    {
        if (scene.selected() && ((game.getPlayerId() == 1 && selected > 16) || (game.getPlayerId() == 2 && selected < 17)))
            game.tryMovement(scene.getSelected() + 1, selected + 1);
        else if ((game.getPlayerId() == 1 && selected <= 16) || (game.getPlayerId() == 2 && selected > 16))
            scene.selectModel(selected);
    }
    else if (selected >= 100)
    {
        selected -= 100;
        int caseY = selected%8;
        int caseX = 7- (selected/8);

        if (scene.selected())
            game.tryMovement(scene.getSelected()+1 , caseX, caseY);
    }
}

/**
 * Renderuje skybox
*/
void Application::renderSkybox() 
{
    glDepthMask (GL_FALSE);

    scene.setView();
    program.use();

    glViewport(0, 0, fb_Width, fb_Height);

    glm::mat4 m = glm::mat4(1.0f);
    glm::mat4 v = scene.getViewMatrix();
    glm::mat4 p = scene.getProjectionMatrix();
    glm::mat4 mv = v * m;
    glm::mat4 mvp = p * mv;
    
    glUniformMatrix4fv(
        glGetUniformLocation(program.getId(), "proj_view_model"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform1i(
        glGetUniformLocation(program.getId(), "skybox_enabled"), isSkyBoxOn);

    glActiveTexture (GL_TEXTURE5);
    glBindTexture (GL_TEXTURE_CUBE_MAP, scene.getTexCube());
    
    glUniform1i(
        glGetUniformLocation(program.getId(), "cube_texture"), 5);
    
    glBindVertexArray (scene.getSkyBox());
    glDrawArrays (GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask (GL_TRUE);
}

/**
 * Wywoływane przy zmianie rozmiaru okna
*/
void Application::resizeCallback(GLFWwindow *window, int width, int height)
{
    glfwGetFramebufferSize(window, &fb_Width, &fb_Height);

    w_Width = width;
    w_Height = height;
    
    scene.setPerspective(width, height);
    glUniformMatrix4fv(
        glGetUniformLocation(program.getId(), "projection_matrix"), 
        1, 
        GL_FALSE, 
        scene.getProjectionMatrixArray()
    );
}

/**
 * Wywoływane gdy wsytąpi błąd
*/
void Application::errorCallback(int error, const char* description)
{
    fputs(description, stderr);
}

/**
 * Zapis tekstury
*/
void Application::saveTexture()
{
    std::cout << "Zapisz teksture" << std::endl;
    glBindTexture(GL_TEXTURE_CUBE_MAP, scene.getTexCube());
    
    GLint textureWidth, textureHeight;

    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &textureWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &textureHeight);
    
    std::cout << textureWidth << "x" << textureHeight << std::endl;
    
    GLubyte *data = new GLubyte[textureWidth*textureHeight*3];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    
    std::ofstream myfile;
    myfile.open("test.txt");
    for (int i = 0 ; i < textureHeight; ++i)
    {
        for (int j = 0; j < textureWidth; ++j)
        {
            int index = i*textureWidth+j;
            myfile << data[3*index] << ",";
            
        }
        myfile << "\n";
    }
    
    myfile.close();
}

/**
 * Wywoływane gdy wcisnięty jest klawisz klawiatury
*/
void Application::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (action == GLFW_PRESS) {
        switch(key) 
        {
        // Kontrolowanie kamery 
        case 'W':
            scene.setCamFW(true);
            break;
        case 'S':
            scene.setCamBW(true);
            break;
        case 'A':
            scene.setCamLS(true);
            break;
        case 'D':
            scene.setCamRS(true);
            break;
        case 'Q':
            scene.setCamZP(true);
            break;
        case 'E':
            scene.setCamZN(true);
            break;
        // Przełącznik światła
        case 'X':
            isLightOn[0] = isLightOn[0] ? false : true;
            break;
        // Przełącznik światła
        case 'C':
            isLightOn[1] = isLightOn[1] ? false : true;
            break;
        // Przełącznik skyboxa
        case 'B':
            isSkyBoxOn = isSkyBoxOn ? false : true;
            break;
        // Przełącznik cieni
        case 'V':
            isShadowOn = isShadowOn ? false : true;
            break;
        // Przełącznik odbicia
        case 'N':
            isReflectionOn = isReflectionOn ? false : true;
            break;
        default:
            break;
        }
    } else if (action == GLFW_RELEASE) {
        switch(key) {
        case 'W':
            scene.setCamFW(false);
            break;
        case 'S':
            scene.setCamBW(false);
            break;
        case 'A':
            scene.setCamLS(false);
            break;
        case 'D':
            scene.setCamRS(false);
            break;
        case 'Q':
            scene.setCamZP(false);
            break;
        case 'E':
            scene.setCamZN(false);
            break;
        default:
            break;
        }
    }
}

/**
 * Wywoływane gdy kursor zmienia pozycję
*/
void Application::cursorPosCallback(GLFWwindow* window, double mouseX, double mouseY) 
{
}

/**
 * Wywoływane gdy wciskany jest przycisk myszy
*/
void Application::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    double x;
    double y;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) 
    {
        glfwGetCursorPos(window, &x, &y);
        processSelection(x, y);
    }
}
