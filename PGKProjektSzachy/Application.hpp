#ifndef __application_h_
#define __application_h_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Program.hpp"
#include "Scene.hpp"
#include "Game.hpp"
#include "Camera.hpp"

class Application
{
public:
    static void start();

private:
    static void display();
    static void initOpenGL();
    static void initGame();

    // Callbacki
    static void errorCallback(int error, const char* description);
    static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void resizeCallback(GLFWwindow *window, int width, int height);
    static void cursorPosCallback(GLFWwindow* window, double mouseX, double mouseY);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    // Wyliczanie fps
    static double lastTime;
    static int nbFrames;
    static int nbFramesLastSecond;

    static GLFWwindow *window;

    // Programy
    static Program program;
    static Program p_Shadows;
    static Program p_Selection;
    static Program p_Skybox;

    static Scene scene;
    static Camera *camera;
    static Game game;
    
    static void renderShadow(int);
    static void renderScene();

    // Rozmiary okna
    static int w_Width, w_Height;
    // Rozmiary framebuffera
    static int fb_Width, fb_Height;
    // Œrodkowe wspó³rzêdne okna
    static int w_MidX, w_MidY;

    static void processSelection(int, int);
    static void renderSelection();

    static void renderSkybox();
    
    static void saveTexture();
    
    static bool isSkyBoxOn, isShadowOn, isReflectionOn;
    static bool isLightOn[2];
};

#endif
