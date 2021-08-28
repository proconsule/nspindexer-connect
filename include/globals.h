#ifndef GLOBALS_H
#define GLOBALS_H

#include <glad/glad.h>

//#define DEBUG_NXLINK

typedef struct {
    GLuint id = 0;
    int width = 0;
    int height = 0;
} Tex;


extern Tex dummyNSP;
extern Tex dummyNSZ;
extern Tex dummyXCI;
extern Tex dummyXCZ;



#endif