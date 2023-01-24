#ifndef __vao_h_
#define __vao_h_

#define GLM_FORCE_RADIANS
#define _USE_MATH_DEFINES

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#define __gl_h_
#else
#include <GL/glew.h>
#endif // __APPLE__

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <math.h>

class Vao
{
public:
    Vao() {}
    Vao(const Vao& vao, const glm::vec3& color) { id = vao.id; vertex_count = vao.vertex_count; diffuse_color = color; texture_enabled = vao.texture_enabled;}
    
    GLuint getId() const { return id; }
    GLsizei getVertexCount() const { return vertex_count; }
    
    const GLfloat* getModelMatrixArray() const { return glm::value_ptr(modelMat); }
    const GLfloat* getDiffuseColorArray() const { return glm::value_ptr(diffuse_color); }
    const glm::mat4& getModelMatrix() const { return modelMat; }
    
    static Vao getCube();
    static Vao getSkyBoxCube();
    static Vao loadObj(std::string, glm::vec3);
    static Vao loadObj(std::string, glm::vec3, std::string);

    void translate(const glm::vec3& vector);
    void rotate(float angle, const glm::vec3 &vector);
    void scale(const glm::vec3 &vector);
    
    bool isTextureEnabled() const { return texture_enabled; }
    GLuint getTextureId() const { return texture_id; }
    
    void requestMovement(glm::vec3 pos_end);
    void requestJumpMovement(glm::vec3 pos_end);
    void requestEjectMovement();
    inline bool isMovementRequested() const { return movementReq; }
    inline bool isJumpMovementRequested() const { return jmpMovReq; }
    inline bool isEjectMovementRequested() const { return ejectMovReq; }
    inline const glm::vec3& getMovementDirection() const { return movementDir; }
    inline double getMovementStartTime() const { return movementStartTime; }
    inline float getMovementLength() const { return movLen; }
    void updateMovement();
    void endMovement();
    
    inline bool isRotated90() { return rotated; }
    
    void rotate90() { rotate(M_PI, glm::vec3(0.f, 1.f, 0.f));  rotated = true;}

private:
    GLuint id;
    GLuint texture_id = 0;
    GLsizei vertex_count;
    glm::mat4 modelMat;
    glm::vec3 diffuse_color;
    GLboolean texture_enabled;
    
    bool movementReq = false;
    bool jmpMovReq = false;
    bool ejectMovReq = false;

    // Zmienne do obliczania skoku
    float jmpA, jmpB;
    float movLen;

    glm::vec3 posStart, posEnd, movementDir;
    
    double movementStartTime;
    
    glm::mat4 modelMatBeforeMovement;

    bool rotated = false;
};

#endif
