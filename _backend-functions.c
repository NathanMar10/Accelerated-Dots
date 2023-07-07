#include "_backend-functions.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include <stdio.h>


static double get_percent_in(double bound_value, double *values);
#define TRIANGLE_SAME(FIRST, SECOND) (.5 * (FIRST) * (SECOND))
#define TRIANGLE_DIFF(FIRST, SECOND) (1. - .5 * (FIRST) * (SECOND))
#define TRAPEZOID(FIRST, SECOND) (.5 * ((FIRST) + (SECOND)))

//#define ANTI_ALIAS
//#define BLUR_EDGES
#define BACKGROUND_STRIPES
//#define MOTION_BLUR
#define MOTION_BLUR_CONST .7
#define VERTICAL_GRAVITY .02

#define DIST(A, B) (sqrt(pow((A.posx) - (B.posx), 2) + pow((A.posy) - (B.posy), 2)))

void draw_dots(Dot *dots, unsigned char* arr, int dot_count, int screen_width, int screen_height) {

    int dot_num, j, k, c;
    Dot curr;

    /* Pseudocode:
    For each dot, i want to isolate an area, then im gonna draw a square all the way around it to start */

  
    for (dot_num = 0 ; dot_num < dot_count; dot_num++) {
        /* Iterates once for each of my dots */
        Dot curr = dots[dot_num];
        for (j = (int)curr.posy - (int)curr.size - 2; j < (int)curr.posy + (int)curr.size + 2; j++) {
            for (k = (int)curr.posx - (int)curr.size - 2; k < (int)curr.posx + (int)curr.size + 2; k++) {


                if ( j < screen_height && j > 0 && k < screen_width && k > 0) {
                    /* Loop executes for each square within the bounds of the circle */
                    /* Basic Solution: Run distance formula on every single dot */
                    
                    #if defined ANTI_ALIAS
                        
                        double corners[4];
                        corners[0] = sqrt( pow( curr.posx - k, 2) + pow( curr.posy - j, 2));
                        corners[1] = sqrt( pow( curr.posx - k, 2) + pow( curr.posy - j - 1, 2)); 
                        corners[2] = sqrt( pow( curr.posx - k - 1, 2) + pow( curr.posy - j, 2)); 
                        corners[3] = sqrt( pow( curr.posx - k - 1, 2) + pow( curr.posy - j - 1, 2)); 
                        
                        double factor = get_percent_in(curr.size, corners);
                        if (abs(factor - 1) > .01 && abs(factor - 0) > .01) {
                            printf("%f\n", factor);
                        }

                        if (factor > .01) {
                            arr[j * screen_width * 3 + k * 3 ] = (double)curr.color[0] * factor * factor;
                            arr[j * screen_width * 3 + k * 3 + 1] = (double)curr.color[1] * factor * factor;
                            arr[j * screen_width * 3 + k * 3 + 2] = (double)curr.color[2] * factor * factor;  
                        }

                    #else

                        if ( sqrt( pow( curr.posx - k, 2) + pow( curr.posy - j, 2) ) < curr.size) {
                            arr[j * screen_width * 3 + k * 3 ] = curr.color[0];
                            arr[j * screen_width * 3 + k * 3 + 1] = curr.color[1];
                            arr[j * screen_width * 3 + k * 3 + 2] = curr.color[2]; 
                        }



                    #endif
                }
            }
        }
    }
    #if defined(BLUR_EDGES)
        blur_edges(arr, screen_width, screen_height);
    #endif
}


void move_dots(Dot *dots, int dot_count, int screen_width, int screen_height) {

    int i, j;
    double dist, net_vel_x, net_vel_y, delta_p_x, delta_p_y, mass_ratio, angle;
    double Azi, Bzi, Awi, Bwi, Bzf, Azf;
    Dot *A, *B, *temp;
    for (i = 0; i < dot_count; i++) {

        /* Setting Basic Positions */
        dots[i].posx += dots[i].velx;
        dots[i].posy += dots[i].vely;

        /* Checking Collisions with the Wall */
        if ( ( dots[i].posx > screen_width  - dots[i].size && dots[i].velx > 0 ) || 
            (dots[i].posx < 0 + dots[i].size && dots[i].velx < 0) ) {
            dots[i].velx *= -1;
        }
        if ( ( dots[i].posy > screen_height - dots[i].size && dots[i].vely > 0 ) || 
            ( dots[i].posy < 0 + dots[i].size && dots[i].vely < 0 ) ) {
            dots[i].vely *= -.75;
        } else {
            dots[i].vely += VERTICAL_GRAVITY;
        }

        /* Checking Collisions with eachother */   

        for (j = i + 1; j < dot_count; j++) {

            dist = DIST(dots[i], dots[j]);
            if ( dist <= dots[i].size + dots[j].size) {
                // we are now too close, they must collide.


                A = &dots[i];
                B = &dots[j];

                if ( B -> size > A -> size ) {
                    temp = A;
                    A = B;
                    B = temp;
                }
                    
                net_vel_x = A -> velx - B -> velx;
                net_vel_y = A -> vely - B -> vely;
                delta_p_x = A -> posx - B -> posx;
                delta_p_y = A -> posy - B -> posy;

                angle = acos( (B -> posx - A -> posx) / dist);
                if ( (B -> posy - A -> posy) < 0) {
                    angle = (2 * M_PI) - angle;
                }

                if (net_vel_x * delta_p_x + net_vel_y * delta_p_y < 0) {



                    mass_ratio = B -> size / A -> size;

                    Azi = A -> velx * cos(angle) + A -> vely * sin(angle);
                    Awi = A -> vely * cos(angle) - A -> velx * sin(angle);
                    Bzi = B -> velx * cos(angle) + B -> vely * sin(angle);
                    Bwi = B -> vely * cos(angle) - B -> velx * sin(angle);
                            

                    /* This line below should have coefficient of restitution times the send term */
                    Bzf = ((mass_ratio * Bzi + Azi) + 1 * (Azi - Bzi)) / (1 + mass_ratio);
        
                    Azf = mass_ratio * (Bzi - Bzf) + Azi;

                    A -> velx = Azf * cos(-angle) + Awi * sin(-angle);
                    A -> vely = Awi * cos(-angle) - Azf * sin(-angle);
                    B -> velx = Bzf * cos(-angle) + Bwi * sin(-angle);
                    B -> vely = Bwi * cos(-angle) - Bzf * sin(-angle);
                        


                 

                } 
/*
                    printf("Did this look stupid?\n");


                    B -> posx = A -> posx + cos(angle)*(A -> size + B -> size + 1);
                    B -> posy = A -> posy + sin(angle)*(A -> size + B -> size + 1);
*/
            }

        }

        
    }

    
}

void erase_screen(unsigned char* arr, int screen_width, int screen_height) {
  
  unsigned int i, j, k;

  for (i = 0; i < screen_height; i++) { // Iterates Which row
    for (j = 0; j < screen_width; j++) { // Iterates which column
      for (k = 0; k < 3; k++) {
        

        #if defined(MOTION_BLUR)
            arr[i*screen_width*3 + j*3 + k] *= MOTION_BLUR_CONST;
        #elif defined(BACKGROUND_STRIPES)
            arr[i * screen_width * 3 + j * 3 + k] = (i + j) % 150;
        #else
            arr[i*screen_width*3 + j*3 + k] = 0;
        #endif

        
        
        
      }
    }
  }
}

//static double get_distance(double x1, double y1, double x2, double y2)

/* Below functions are deprecated, theyre called by a few different modes but I dont like the look of em all that much */

/* This function implements the marching squares algorithm (to an extent). It will receive the values
 * of a function evaluated at each corner of a pixel and then determine what
 * percentage of a pixel is encompassed by the function.
 * 
 * Why????
 * This will be called once on the pixels on the outside of a circle, meaning that I will know what
 * percentage of the pixel is inside of the circle. This will let me color the outer boundary all
 * anti-aliased like. 
 * It will then be called for the pixels on the inner radius of the circle (I should have
 * some sort of heuristic to know which pixels will be inside -> I'll return a crossing point on 
 * all sides to some integers if i care about them). I can then anti-alias the insides of the circles
 * too. 
 */


static double get_percent_in(double bound_value, double *values) {

    /* I will define a set of coordinates, where the top left is 0,0 and bottom right
     * is 1,1. I will then calculate the positions of the intercepts of a linear
     * function and depending on tose c */

    /* If i rotate around the square starting tl and going clockwise, whenever we
     * change sign, i can know that there was a crossing. I can then instantly
     * calculate the crossing point (cross structure with axis and placement) and then
     * calculate my area with a formula. */


    /* These are where I will store the places that the funtion crosses
     * the bounds */
    Cross first, second;
    first.axis = U;
    second.axis = U;
    int found_first = 0, found_second = 0;
    /* Sets the Crossing Points */
    if ( ( values[0] > bound_value ) != ( values[1] > bound_value) ) {
        /* Crosses on Top */
        if (!found_first) {
            first.axis = T;
            first.placement = (values[0] - bound_value) / (values[0] - values[1]);
            found_first = 1;
        } else {
            second.axis = T;
            second.placement = (values[0] - bound_value) / (values[0] - values[1]);
            found_second = 1;
        }
    }
    if ( ( values[1] > bound_value ) != ( values[2] > bound_value) ) {
        /* Crosses on Right */
        if (!found_first) {
            first.axis = R;
            first.placement = (values[1] - bound_value) / (values[1] - values[2]);
            found_first = 1;
        } else {
            second.axis = R;
            second.placement = (values[1] - bound_value) / (values[1] - values[2]);
            found_second = 1;
        }
    }
    if ( ( values[2] > bound_value ) != ( values[3] > bound_value) ) {
        /* Crosses on Bottom */
        if (!found_first) {
            first.axis = B;
            first.placement = (values[3] - bound_value) / (values[3] - values[2]);
            found_first = 1;
        } else {
            second.axis = B;
            second.placement = (values[3] - bound_value) / (values[3] - values[2]);
            found_second = 1;
        }
    }
    if ( ( values[3] > bound_value ) != ( values[0] > bound_value) ) {
        /* Crosses on Left */
        if (!found_first) {
            first.axis = L;
            first.placement = (values[0] - bound_value) / (values[0] - values[3]);
            found_first = 1;
        } else {
            second.axis = L;
            second.placement = (values[0] - bound_value) / (values[0] - values[3]);
            found_second = 1;
        }
    }


    if (found_first && !found_second) {
        printf("Shits wack");
    }

    /* I now have the crossing points and can determine how much area is inside of the boy */
    /* Area will hold the area to the side of the top left corner */
    double area = 1;


    //printf("MAde it to the switch\n");
    switch(first.axis) {
        case(T):
        //puts("Case T\n");
        if (second.axis == L) {
         area = TRIANGLE_SAME(first.placement, second.placement);
        } else if (second.axis == R) {
            area = TRIANGLE_DIFF(first.placement, second.placement);
        } else if (second.axis == B) {
            /* Trapezoid Case */
            area = TRAPEZOID(first.placement, second.placement);
        } else {
            puts("Only one intersection found in get_percent_in, Case T");
        }
        break;

        case(R):
            //puts("Case R\n");
            if (second.axis == T) {
                //puts("Case RT\n");
                area = TRIANGLE_DIFF(first.placement, second.placement);
            } else if (second.axis == B) {
                //puts("Case RB\n");
                area = TRIANGLE_DIFF(first.placement, second.placement);
            } else if (second.axis == L) {
                //puts("Case RL\n");
                /* Trapezoid Case */
                area = TRAPEZOID(first.placement, second.placement);
        } else {
            puts("Only one intersection found in get_percent_in, Case R");
        }
        break;

        case(B):
        //puts("Case B\n");
        if (second.axis == L) {
            area = TRIANGLE_DIFF(first.placement, second.placement);
        } else if (second.axis == R) {
            area = TRIANGLE_DIFF(first.placement, second.placement);
        } else if (second.axis == T) {
            /* Trapezoid Case */
            area = TRAPEZOID(first.placement, second.placement);
        } else {
            printf("Only one intersection found in get_percent_in, Case B");
        }
        break;

        case(L):
        //puts("Case L\n");
        if (second.axis == T) {
            area = TRIANGLE_SAME(first.placement, second.placement);
        } else if (second.axis == B) {
            area = TRIANGLE_DIFF(first.placement, second.placement);
        } else if (second.axis == R) {
            /* Trapezoid Case */
            area = TRAPEZOID(first.placement, second.placement);
        } else {
            puts("Only one intersection found in get_percent_in, Case L");
        }
        break;
    }


    

    if (values[0] > bound_value) {
        area = 1 - area;
    }


    //printf("Made it past the switch\n");
    
    return area;

}


void blur_edges(unsigned char* arr, int screen_width, int screen_height) {

    int i = 0, j = 0, k = 0;

    double count, total;

    //unsigned char *copied_arr = malloc(screen_width * screen_height * 3);
    unsigned char copied_arr[400][800][3];
    /*
    if (copied_arr == NULL) {
        printf("Allocation Failed... \n");
    }
*/
    for (i = 0; i < screen_height; i++) {
        for (j = 0; j < screen_width; j++) {
            for (k = 0; k < 3; k++) {
                total = 0;
                count = 0;
                for (int xoff = -4; xoff <= 4; xoff++) {
                    for (int yoff = -4; yoff <= 4; yoff++) {

                        if (i + yoff < screen_height && i + yoff >= 0 && j + xoff < screen_width && j + xoff >= 0) {
                            count ++;;
                            total += arr[(i + yoff) * screen_width * 3 + (j + xoff) * 3 + k] * 
                                        arr[(i + yoff) * screen_width * 3 + (j + xoff) * 3 + k];;
                            
                        }

                    }
                }
                //printf("Copy Count\n");
                //printf("%d, %d\n", count, total);
                //printf("%d %d %d\n", i, j, k);
                //copied_arr[0][0][0] = 1;
                //printf("DID THIS WORK");
                copied_arr[i][j][k] = sqrt((double)total / count);
                //printf("THis bit wrked now too\n");
            }
           
            

        }



        
    }

    //printf("Abt to copy");
    for (i = 0; i < screen_height; i++) {
        for (j = 0; j < screen_width; j++) {
            for (k = 0; k < 3; k++) {
                arr[i * screen_width * 3 + j * 3 + k] = copied_arr[i][j][k];
            }
        }
    }  

    //free(copied_arr); 
}

