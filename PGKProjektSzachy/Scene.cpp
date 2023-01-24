#include "Scene.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <map>
#include <iostream>
#include "stb_image.h"

static const glm::vec3 color1(1.0f, 1.0f, 1.0f);
static const glm::vec3 color2(0.2f, 0.2f, 0.2f);

void Scene::selectModel(int index) 
{
    selected_model = index;
    vao_selected = true;
}

void Scene::unselect() 
{
    vao_selected = false;
}

void Scene::initScene(int width, int height)
{
    selectTex = 0;

    initLights();
    initModels();

    for (unsigned int i = 0; i < lights.size(); ++i)
        initShadow(i);
    
    selectColor = glm::vec3(1.0, 1.0, 0.0);
    projMat = glm::mat4(1.0f);
    camera = Camera(width, height);

    initSkyBox();
}

void Scene::initLights()
{
    lights.push_back(
        Light(
            glm::vec3(400.f, 400.f, 400.f), 
            glm::vec3(0.2f, 0.2f, 0.2f), 
            glm::vec3(0.5f, 0.5f, 0.5f)
        )
    );

    lights.push_back(
        Light(
            glm::vec3(-400.f, 400.f, -400.f), 
            glm::vec3(0.2f, 0.2f, 0.2f), 
            glm::vec3(0.5f, 0.5f, 0.5f)
        )
    );
}

void Scene::initShadow(int light_index)
{
    shadowSize = 4096;
   
    Light &currLight = lights[light_index];
    
    glGenTextures(1, &currLight.getShadowTextureId());
    
    glBindTexture(GL_TEXTURE_2D , currLight.getShadowTextureId());
   
    glTexImage2D(GL_TEXTURE_2D , 0, GL_DEPTH_COMPONENT,  shadowSize, shadowSize, 0, GL_DEPTH_COMPONENT , GL_FLOAT , NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER , GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER , GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T , GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D , 0);

    // Framebuffer dla cieni
    glGenFramebuffers(1, &currLight.getShadowBufferId());
    glBindFramebuffer(GL_FRAMEBUFFER , currLight.getShadowBufferId());

    glFramebufferTexture2D(GL_FRAMEBUFFER , GL_DEPTH_ATTACHMENT , GL_TEXTURE_2D , currLight.getShadowTextureId(), 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (FBOstatus != GL_FRAMEBUFFER_COMPLETE)
        printf("Blad: GL_FRAMEBUFFER_COMPLETE_EXT: nie mozna uzyc FBO\n");
    
    glBindFramebuffer(GL_FRAMEBUFFER , 0);
    
    shadowProjMat = glm::perspective<float>(1.4f, 1.f, 20.f, 10000.f);
    
    biasMat = glm::mat4(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    );
    
}

void Scene::initModels()
{
    glm::mat4 rot180(1.f);
    rot180 = glm::rotate(rot180, (float)M_PI, glm::vec3(0.f, 1.f, 0.f));
    
    // Pionki
    loadedModeles["models/bishop.obj"] = 
        Vao::loadObj(PIECE_BISHOP_MODEL_PATH, glm::vec3(1.0));
    loadedModeles["models/king.obj"] = 
        Vao::loadObj(PIECE_KING_MODEL_PATH, glm::vec3(1.0));
    loadedModeles["models/knight.obj"] = 
        Vao::loadObj(PIECE_KNIGHT_MODEL_PATH, glm::vec3(1.0));
    loadedModeles["models/pawn.obj"] = 
        Vao::loadObj(PIECE_PAWN_MODEL_PATH, glm::vec3(1.0));
    loadedModeles["models/queen.obj"] = 
        Vao::loadObj(PIECE_QUEEN_MODEL_PATH, glm::vec3(1.0));
    loadedModeles["models/rook.obj"] = 
        Vao::loadObj(PIECE_ROOK_MODEL_PATH, glm::vec3(1.0));

    // Plansza
    Vao boardSurface = 
        Vao::loadObj("models/plane.obj", glm::vec3(0.f, 0.f, 0.f), "textures/szachownica1.tga");

    vao_list.push_back(boardSurface);
}

void Scene::setPerspective(int width, int height)
{
    projMat = glm::mat4(1.0f);
    projMat = glm::perspective(FOV, (float)width/height, ZNEAR, ZFAR);
}

void Scene::setView() {
    view_matrix = glm::mat4 (1.0f);
    view_matrix = glm::lookAt(camera.getPosition(), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
}

void Scene::move(int fps) {
    camera.move(fps);
}

GLfloat* Scene::getNormalMatrixArray(unsigned int vao_index)
{
    const glm::mat4 &model_matrix = vao_list[vao_index].getModelMatrix();
    //(1.0f);

    normal_matrix = glm::transpose(glm::inverse(view_matrix * model_matrix));

    return glm::value_ptr(normal_matrix);
}

void Scene::initSkyBox(){
    float points[] = {
        -4000.0f,  4000.0f, -4000.0f,
        -4000.0f, -4000.0f, -4000.0f,
        4000.0f, -4000.0f, -4000.0f,
        4000.0f, -4000.0f, -4000.0f,
        4000.0f,  4000.0f, -4000.0f,
        -4000.0f,  4000.0f, -4000.0f,

        -4000.0f, -4000.0f,  4000.0f,
        -4000.0f, -4000.0f, -4000.0f,
        -4000.0f,  4000.0f, -4000.0f,
        -4000.0f,  4000.0f, -4000.0f,
        -4000.0f,  4000.0f,  4000.0f,
        -4000.0f, -4000.0f,  4000.0f,

        4000.0f, -4000.0f, -4000.0f,
        4000.0f, -4000.0f,  4000.0f,
        4000.0f,  4000.0f,  4000.0f,
        4000.0f,  4000.0f,  4000.0f,
        4000.0f,  4000.0f, -4000.0f,
        4000.0f, -4000.0f, -4000.0f,

        -4000.0f, -4000.0f,  4000.0f,
        -4000.0f,  4000.0f,  4000.0f,
        4000.0f,  4000.0f,  4000.0f,
        4000.0f,  4000.0f,  4000.0f,
        4000.0f, -4000.0f,  4000.0f,
        -4000.0f, -4000.0f,  4000.0f,

        -4000.0f,  4000.0f, -4000.0f,
        4000.0f,  4000.0f, -4000.0f,
        4000.0f,  4000.0f,  4000.0f,
        4000.0f,  4000.0f,  4000.0f,
        -4000.0f,  4000.0f,  4000.0f,
        -4000.0f,  4000.0f, -4000.0f,

        -4000.0f, -4000.0f, -4000.0f,
        -4000.0f, -4000.0f,  4000.0f,
        4000.0f, -4000.0f, -4000.0f,
        4000.0f, -4000.0f, -4000.0f,
        -4000.0f, -4000.0f,  4000.0f,
        4000.0f, -4000.0f,  4000.0f
    };

    glGenVertexArrays (1, &skyBox);
    glBindVertexArray (skyBox);

    GLuint vbo;
    glGenBuffers (1, &vbo);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, 3 * 36 * sizeof (float), points, GL_STATIC_DRAW);

    glVertexAttribPointer ((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray (0);

    // Skybox i jego tekstury
    std::string prefix = "space";
    std::string path = "textures/" + prefix + "_";

    std::string front = path + "front.jpg";
    std::string back = path + "back.jpg";
    std::string right = path + "right.jpg";
    std::string left = path + "left.jpg";
    std::string top = path + "top.jpg";
    std::string bottom = path + "bottom.jpg";
    
    std::vector<const GLchar*> faces;
    faces.push_back(right.c_str());
    faces.push_back(left.c_str());
    faces.push_back(top.c_str());
    faces.push_back(bottom.c_str());
    faces.push_back(back.c_str());
    faces.push_back(front.c_str());

    texCube = loadCubemap(faces);
}

GLuint Scene::loadCubemap(std::vector<const GLchar*> faces)
{
    GLuint textureID;

    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE5);

    int width, height, n;
    unsigned char* image;

    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    image = stbi_load (faces[0], &width, &height, &n, 4);
    if (!image) 
        fprintf (stderr, "ERROR: could not load %s\n", faces[0]);

    glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    image = stbi_load (faces[1], &width, &height, &n, 4);
    if (!image) 
        fprintf (stderr, "ERROR: could not load %s\n", faces[1]);

    glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    image = stbi_load (faces[2], &width, &height, &n, 4);
    if (!image) 
        fprintf (stderr, "ERROR: could not load %s\n", faces[2]); 

    glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    image = stbi_load (faces[3], &width, &height, &n, 4);
    if (!image) 
        fprintf (stderr, "ERROR: could not load %s\n", faces[3]);

    glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    image = stbi_load (faces[4], &width, &height, &n, 4);
    if (!image) 
        fprintf (stderr, "ERROR: could not load %s\n", faces[4]);

    glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    image = stbi_load (faces[5], &width, &height, &n, 4);
    if (!image) 
        fprintf (stderr, "ERROR: could not load %s\n", faces[5]);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}

int Scene::addVaoPiece(std::string model, int team, glm::vec3 pos){
    Vao piece ;

    if (team == 1)
        piece = Vao::loadObj(model, color1);
    else
        piece = Vao::loadObj(model, color2);

    piece.translate(pos);

    vao_list.push_back(piece);

    return ((int)vao_list.size());
}

std::vector<int> Scene::addVaoPieces(
    std::vector<std::string> model, 
    std::vector<int> team, 
    std::vector<glm::vec3> pos
) {
    Vao boardSurf = vao_list[0];

    vao_list.clear();
    vao_list.push_back(boardSurf);
    
    std::map<std::string, int> loadedModeles;
    std::vector<int> indices;

    for (int i = 0 ; i < (int) model.size() ; i++)
    {
        Vao piece;
        std::map<std::string, int>::iterator it = loadedModeles.find(model[i]);
        
        if (it == loadedModeles.end())
        {
            if (team[i] == 1) 
                piece = Vao::loadObj(model[i],color1);
            else
                piece = Vao::loadObj(model[i],color2);
            
            loadedModeles[model[i]] = (int)vao_list.size();
        }
        else
        {
            if (team[i] == 1)
                piece = Vao(vao_list[it->second], color1);
            else
                piece = Vao(vao_list[it->second], color2);
        }
        
        piece.translate(pos[i]);
        vao_list.push_back(piece);
        indices.push_back((int) vao_list.size());
        
        if (team[i] == 2 
            && (model[i] == PIECE_KNIGHT_MODEL_PATH 
            || model[i] == PIECE_BISHOP_MODEL_PATH)
        )
            vao_list[i+1].rotate90();
    }

    return indices;
}


void Scene::slideVAOTo(int vao, glm::vec3 newPos)
{
    vao_list[vao].requestMovement(newPos);
}

void Scene::jumpVAOTo(int vao, glm::vec3 newPos)
{
    vao_list[vao].requestJumpMovement(newPos);
}

void Scene::ejectVAO(int vao) 
{
    vao_list[vao].requestEjectMovement();
}

void Scene::deleteVAO(int vao) 
{
    vao_list.erase(vao_list.begin() + vao);
}

const GLuint& Scene::getTexCube() 
{
     if (selectTex == 0)
          return texCube;
     else if (selectTex == 1)
          return texCubeYellow;
     else if (selectTex == 2)
          return texCubeRed;
}
