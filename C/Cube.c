#include "gaspardos_user_x86.h"


typedef enum {
    COLOR_BLACK       = 0x000000FF,
    COLOR_WHITE       = 0xFFFFFFFF,
    COLOR_RED         = 0xFF0000FF,
    COLOR_GREEN       = 0x008000FF,
    COLOR_BLUE        = 0x0000FFFF,
    COLOR_YELLOW      = 0xFFFF00FF,
    COLOR_CYAN        = 0x00FFFFFF,
    COLOR_MAGENTA     = 0xFF00FFFF,
    COLOR_GRAY        = 0x808080FF,
    COLOR_TRANSPARENT = 0x00000000
} BasicRGBAColor;
#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768

typedef struct Vec3 {
    float x, y, z;
} Vec3;

Vec3 vertices[8] = {
    // Avant
    {-1.0f, -1.0f,  1.0f},  // Point 0 : en bas à gauche de la face avant
    {1.0f, -1.0f,  1.0f},   // Point 1 : en bas à droite de la face avant
    {-1.0f,  1.0f,  1.0f},  // Point 2 : en haut à gauche de la face avant
    {1.0f,  1.0f,  1.0f},   // Point 3 : en haut à droite de la face avant
    // Arrière
    {-1.0f, -1.0f, -1.0f},  // Point 4 : en bas à gauche de la face arrière
    {1.0f, -1.0f, -1.0f},   // Point 5 : en bas à droite de la face arrière
    {-1.0f,  1.0f, -1.0f},  // Point 6 : en haut à gauche de la face arrière
    {1.0f,  1.0f, -1.0f}    // Point 7 : en haut à droite de la face arrière
};
void gmain() {



    gaspardapi_init_video();




}