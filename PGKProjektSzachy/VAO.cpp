#include "VAO.hpp"
#include "TGA.hpp"
#include <vector>
#include <fstream>
#include <sstream>
#include <GLFW/glfw3.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>


static float const jumpHigh = 0.0f;
static float const speed = 5.0f;

void Vao::endMovement()
{
    movementReq = false;
}

void Vao::updateMovement()
{
    double elapsedTime = glfwGetTime() - movementStartTime;

    float movementLen = (float)elapsedTime* speed * getMovementLength();
    glm::vec3 translation = movementLen * movementDir;
    float y;

    if (jmpMovReq || ejectMovReq)
    {
        y = jmpA * pow(movementLen, 2.0f) + jmpB* movementLen;
        translation += glm::vec3(0.0f, y, 0.0f);
    }

    if (movementLen <= getMovementLength() || ejectMovReq)
    {
        modelMat = glm::translate(modelMatBeforeMovement, translation);
        
        if (rotated)
            rotate90();
    }
    
    if (elapsedTime >= 1.0f/speed && !ejectMovReq)
    {
        movementReq = false;
        jmpMovReq = false;
        
        modelMat = glm::mat4(1.f);
        modelMat = glm::translate(modelMat, posEnd);
        
        if (rotated)
            rotate90();
    }

    if (elapsedTime >= 10.0f/speed && ejectMovReq) 
    {
        ejectMovReq = false;
    }
}

void Vao::requestMovement(glm::vec3 pos_end)
{
    posStart = glm::vec3(modelMat * glm::vec4(0, 0, 0, 1));

    modelMatBeforeMovement = glm::mat4(1.f);
    modelMatBeforeMovement = glm::translate(modelMatBeforeMovement, posStart);

    movementReq = true;

    posEnd = pos_end;

    movLen = glm::length(posEnd-posStart);

    movementDir =posEnd-posStart;
    movementDir = glm::normalize(movementDir);

    movementStartTime = glfwGetTime();
}

void Vao::requestJumpMovement(glm::vec3 pos_end)
{
    posStart = glm::vec3(modelMat * glm::vec4(0, 0, 0, 1));
    
    modelMatBeforeMovement = glm::mat4(1.f);
    modelMatBeforeMovement = glm::translate(modelMatBeforeMovement, posStart);
    
    jmpMovReq = true;
    
    posEnd = pos_end;
    
    movLen = glm::length(posEnd-posStart);
    
    movementDir =posEnd-posStart;
    movementDir = glm::normalize(movementDir);
    
    movementStartTime = glfwGetTime();
    
    float alpha = getMovementLength();
    
    jmpB = 4 * jumpHigh / alpha; jmpA =  - 4 * jumpHigh / (float) powf(alpha, 2.0f);
}

void Vao::requestEjectMovement() 
{
    posStart = glm::vec3(modelMat * glm::vec4(0, 0, 0, 1));

    modelMatBeforeMovement = glm::mat4(1.f);
    modelMatBeforeMovement = glm::translate(modelMatBeforeMovement, posStart);

    ejectMovReq = true;
    
    posEnd = glm::vec3(500.0f, 0.0f, 0.0f);
    
    movLen = glm::length(posEnd-posStart);
    
    movementDir =posEnd-posStart;
    movementDir = glm::normalize(movementDir);
    
    movementStartTime = glfwGetTime();
    
    float alpha = getMovementLength();
    
    jmpB = 4 * jumpHigh / alpha; jmpA =  - 4 * jumpHigh / (float) powf(alpha, 2.0f);
}

Vao Vao::loadObj(std::string filename, glm::vec3 color)
{
    return Vao::loadObj(filename, color, "");
}

Vao Vao::loadObj(std::string filename, glm::vec3 color, std::string texture_filename)
{
	std::cout << "Ladowanie modelu: " << filename << std::endl;
    Vao vao;
    
    vao.diffuse_color = color;

    std::vector<glm::vec3> vertices, normals;
    std::vector<glm::vec2> textures;
    std::vector<unsigned int> vertex_indices, normal_indices, texture_indices;
    std::ifstream inputfile(filename);
    std::string str;

    while (std::getline(inputfile, str))
    {
        if (str.size() > 2)
        {
            std::istringstream iss(str);
            std::string token;
            float vec[3];

            if (str[0] == 'v' && str[1] == ' ')
            {
                getline(iss, token, ' ');
                for (int i = 0; i < 3; ++i) {
                    getline(iss, token, ' ');
                    vec[i] = std::stof(token);
                }
                vertices.push_back(glm::vec3(vec[0], vec[1], vec[2]));
            }
            else if (str[0] == 'v' && str[1] == 'n')
            {
                getline(iss, token, ' ');
                for (int i = 0; i < 3; ++i) {
                    getline(iss, token, ' ');
                    vec[i] = std::stof(token);
                }
                normals.push_back(glm::vec3(vec[0], vec[1], vec[2]));
            }
            else if (str[0] == 'v' && str[1] == 't')
            {
                getline(iss, token, ' ');
                
                for (int i = 0; i < 2; ++i) {
                    getline(iss, token, ' ');
                    vec[i] = std::stof(token);
                }
                
                textures.push_back(glm::vec2(vec[0], vec[1]));
            }
            else if (str[0] == 'f' && str[1] == ' ')
            {
                getline(iss, token, ' ');

                for (int i = 0; i < 3; ++i) {
                    getline(iss, token, ' ');
                    size_t index;

                    if ((index = token.find("//")) != std::string::npos)
                    {
                        std::string a = token.substr(0, index);
                        std::string b = token.substr(index + 2, token.size() - index);

                        vertex_indices.push_back(std::stoi(a) - 1);
                        normal_indices.push_back(std::stoi(b) - 1);
                    }
                    else
                    {
                        index = token.find("/");
                        size_t index2 = token.find("/", index + 1);
                        std::string a = token.substr(0, index);
                        std::string b = token.substr(index + 1, index2 - index - 1);
                        std::string c = token.substr(index2 + 1, token.size() - index2 + 1);
                        
                        vertex_indices.push_back(std::stoi(a) - 1);
                        texture_indices.push_back(std::stoi(b) - 1);
                        normal_indices.push_back(std::stoi(c) - 1);
                    }
                }

            }
        }
    }

    std::vector<glm::vec3> vertices_new, normals_new;
    std::vector<glm::vec2> textures_new;
    
    vao.texture_enabled = textures.size() > 0 && texture_indices.size() > 0 && texture_filename.size() > 0;


    for (unsigned int i = 0; i < vertex_indices.size(); ++i)
    {
        vertices_new.push_back(vertices[vertex_indices[i]]);
        normals_new.push_back(normals[normal_indices[i]]);
        
        if (vao.texture_enabled)
            textures_new.push_back(textures[texture_indices[i]]);
    }

    vao.vertex_count = (GLuint)vertices_new.size();

    std::cout << "\tLiczba wierzcholkow: " << vao.vertex_count << std::endl;

    vao.modelMat = glm::mat4(1.f);

    glGenVertexArrays(1, &vao.id);
    glBindVertexArray(vao.id);

    GLuint vbo_positions;
    glGenBuffers(1, &vbo_positions);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_positions);
    glBufferData(
        GL_ARRAY_BUFFER, 
        vao.vertex_count * sizeof(glm::vec3), 
        &vertices_new[0], 
        GL_STATIC_DRAW
    );
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    GLuint vbo_normals;
    glGenBuffers(1, &vbo_normals);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
    glBufferData(
        GL_ARRAY_BUFFER, vao.vertex_count * sizeof(glm::vec3), 
        &normals_new[0], 
        GL_STATIC_DRAW
    );
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    
    if (vao.texture_enabled)
    {
        glGenTextures(1, &vao.texture_id);
        glBindTexture(GL_TEXTURE_2D, vao.texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        Tga tga = Tga::loadFileTGA(texture_filename.c_str());
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tga.imageWidth, tga.imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, tga.imageData);

        GLuint vbo_textures;
        glGenBuffers(1, &vbo_textures);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_textures);
        glBufferData(GL_ARRAY_BUFFER, vao.vertex_count * sizeof(glm::vec2), &textures_new[0], GL_STATIC_DRAW);
        glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(2);        
    }

    glBindVertexArray(0);
    return vao;
}

void Vao::translate(const glm::vec3 &vector)
{
    modelMat = glm::translate(modelMat, vector);
}

void Vao::rotate(float angle, const glm::vec3 &vector)
{
    modelMat = glm::rotate(modelMat, angle, vector);
}

void Vao::scale(const glm::vec3 &vector)
{
    modelMat = glm::scale(modelMat, vector);
}

Vao Vao::getSkyBoxCube(){
    float points[] = {
        -1000.0f,  1000.0f, -1000.0f,
        -1000.0f, -1000.0f, -1000.0f,
        1000.0f, -1000.0f, -1000.0f,
        1000.0f, -1000.0f, -1000.0f,
        1000.0f,  1000.0f, -1000.0f,
        -1000.0f,  1000.0f, -1000.0f,

        -1000.0f, -1000.0f,  1000.0f,
        -1000.0f, -1000.0f, -1000.0f,
        -1000.0f,  1000.0f, -1000.0f,
        -1000.0f,  1000.0f, -1000.0f,
        -1000.0f,  1000.0f,  1000.0f,
        -1000.0f, -1000.0f,  1000.0f,

        1000.0f, -1000.0f, -1000.0f,
        1000.0f, -1000.0f,  1000.0f,
        1000.0f,  1000.0f,  1000.0f,
        1000.0f,  1000.0f,  1000.0f,
        1000.0f,  1000.0f, -1000.0f,
        1000.0f, -1000.0f, -1000.0f,

        -1000.0f, -1000.0f,  1000.0f,
        -1000.0f,  1000.0f,  1000.0f,
        1000.0f,  1000.0f,  1000.0f,
        1000.0f,  1000.0f,  1000.0f,
        1000.0f, -1000.0f,  1000.0f,
        -1000.0f, -1000.0f,  1000.0f,

        -1000.0f,  1000.0f, -1000.0f,
        1000.0f,  1000.0f, -1000.0f,
        1000.0f,  1000.0f,  1000.0f,
        1000.0f,  1000.0f,  1000.0f,
        -1000.0f,  1000.0f,  1000.0f,
        -1000.0f,  1000.0f, -1000.0f,

        -1000.0f, -1000.0f, -1000.0f,
        -1000.0f, -1000.0f,  1000.0f,
        1000.0f, -1000.0f, -1000.0f,
        1000.0f, -1000.0f, -1000.0f,
        -1000.0f, -1000.0f,  1000.0f,
        1000.0f, -1000.0f,  1000.0f
    };
    GLuint vbo;
    glGenBuffers (1, &vbo);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, 3 * 36 * sizeof (float), &points, GL_STATIC_DRAW);

    GLuint vaoID = 0;
    Vao vao;
    vao.id = vaoID;
    vao.vertex_count = 6*2*3;
    vao.modelMat = glm::mat4(1.0f);
    vao.diffuse_color = glm::vec3(0.0f,0.0f,1.0f);

    glGenVertexArrays (1, &vaoID);
    glBindVertexArray (vaoID);
    glEnableVertexAttribArray (0);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    return vao;
}

Vao Vao::getCube()
{
    Vao vao;
    vao.vertex_count = 6*2*3;
    vao.modelMat = glm::mat4(1.0f);

    float vertexPositions[] = {
        -0.5f,  0.5f,  -0.5f,
        0.5f, -0.5f,  -0.5f,
        -0.5f, -0.5f,  -0.5f,
        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,

        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,

        -0.5f,  0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,

        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f
    };

    float vertexColors[] = {
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,

        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,

        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,

        0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f
    };

    glGenVertexArrays(1, &vao.id);
    glBindVertexArray(vao.id);

    GLuint vbo_positions;
    glGenBuffers(1, &vbo_positions);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_positions);
    glBufferData(GL_ARRAY_BUFFER, 12*9*sizeof(float), vertexPositions, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    GLuint vbo_colors;
    glGenBuffers(1, &vbo_colors);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
    glBufferData(GL_ARRAY_BUFFER, 12*9*sizeof(float), vertexColors, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    return vao;
}



