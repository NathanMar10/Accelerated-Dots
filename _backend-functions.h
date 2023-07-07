#include "dot-datastructure.h"

typedef enum {T, R, B, L, U} Axis;
typedef struct cross_tag {

    Axis axis;
    double placement;

} Cross;

void move_dots(
    Dot *dots, 
    int dot_count, 
    int screen_width, 
    int screen_height);

void draw_dots( 
    Dot *dots, 
    unsigned char* screen, 
    int dot_count, 
    int screen_width, 
    int screen_height);

void erase_screen(unsigned char* arr, int screen_width, int screen_height);
void blur_edges(unsigned char* arr, int screen_width, int screen_height);