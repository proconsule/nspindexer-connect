#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>

#include <switch.h>
#include <glad/glad.h>

//#define DEBUG_NXLINK

typedef struct {
    GLuint id = 0;
    int width = 0;
    int height = 0;
} Tex;

extern char* serverUrl;

extern Tex dummyNSP;
extern Tex dummyNSZ;
extern Tex dummyXCI;
extern Tex dummyXCZ;

extern s64 sdcard_total_space_size;
extern s64 sdcard_free_space_size;

extern std::string sdcard_total_space_human;
extern std::string sdcard_free_space_human;

extern s64 nand_total_space_size;
extern s64 nand_free_space_size;

extern std::string nand_total_space_human;
extern std::string nand_free_space_human;



#endif