#ifndef PROGRAM_H
#define PROGRAM_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#define __gl_h_
#else
#include <GL/glew.h>
#endif

#include "Shader.hpp"

class Program 
{
public:
    void init();
    void initForShadowMap();
    void initForSelection();
    GLuint getId() const { return id; }
    void attach (Shader *);
    void link ();
    void detach(Shader *);
    void use();
    void stop();
    void reload();
private:
    GLuint id;
    std::string name;
    std::vector<Shader*> shaders;
};

#endif // PROGRAM_H
