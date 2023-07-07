
#include <Python.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <arrayobject.h>
#include "dot-datastructure.h"

/* Macro to get a random number between the given MAX and MIN value */
#define GET_RANDOM(MIN,MAX) (((double)rand() / RAND_MAX) * ((MAX) - (MIN)) + (MIN))



/* Debug Define Symbols */
// #define PRINT_ARGS
// #define PRINT_ASSIGNMENTS



Dot *dot_arr;
static PyArrayObject* screen;
static unsigned char *screen_buffer;
static unsigned int screen_width, screen_height, dots_count;

/* Numpy array here too -> ill be working on this real soon */



/* Function to initialize the dots.
Python will pass in the following parameters:

numpy_array
screen_width
screen_height
dots_count
min_size
max_size
min_velocity
max_velocity
seed

These are then parsed to  initialize the dots structure and create
a numpy array to store the next image in.
Later on, this will be updated to create other configurations like
collision type, COR, G, K, and others (Including optional Mass)*/

static PyObject *create_dots(PyObject *self, PyObject *args) {

  unsigned int seed, i;
  double min_velocity, max_velocity, min_size, max_size;

  /* Loads the numpy array into the array variable so that I can
   * play with it */
  
  if (!PyArg_ParseTuple(args, "OIIIIdddd", &screen, &screen_width,
			&screen_height, &dots_count, &seed,
			&min_velocity, &max_velocity, &min_size,
			&max_size)) {
            return NULL;
  }



  #if defined(PRINT_ARGS)
  printf("Printing the Dots Arguments...\n");
  printf("Screen Size: %d x %d\n", screen_width, screen_height);
  printf("There are %d dots\n", dots_count);
  printf("Velocity Range: %.2f : %.2f\n", min_velocity, max_velocity);
  printf("Size Range: %.2f : %.2f\n", min_size, max_size);
  #endif


  /* If a seed is given, we will set it. Otherwise, use random seed */
  if (seed != 0) {
      srand(seed);
  } else {
    srand(time(NULL));
  }

  /* Array that will store all of the dots */  
  dot_arr = malloc(dots_count * sizeof(Dot));

  /* For loop initializes each dot in the array */
  for (i = 0; i < dots_count; i++) {
    dot_arr[i].velx = GET_RANDOM(min_velocity, max_velocity);
    dot_arr[i].vely = GET_RANDOM(min_velocity, max_velocity);
    dot_arr[i].posy = GET_RANDOM(0, screen_height);
    dot_arr[i].posx = GET_RANDOM(0, screen_width);
    dot_arr[i].size = GET_RANDOM(min_size, max_size);
    
    dot_arr[i].color[0] = GET_RANDOM(0, 255);
    dot_arr[i].color[1] = GET_RANDOM(0, 255);
    dot_arr[i].color[2] = GET_RANDOM(0, 255);

    //dot_arr[i].color[0] = dot_arr[i].color[1] = dot_arr[i].color[2];
    #if defined(PRINT_ASSIGNMENTS)

    printf("Data For Dot #%d:\n", i);
    printf("\t Posx = %f\n", dot_arr[i].posx);
    printf("\t Vely = %f\n", dot_arr[i].posy);
    printf("\t Velx = %f\n", dot_arr[i].velx);
    printf("\t Vely = %f\n", dot_arr[i].vely);

    #endif

  }

  screen_buffer = malloc( 1 * screen_width * screen_height * 3 );

  Py_RETURN_NONE;
}

/* Function for Python to request the next from from C. This will
 * update the numpy array and hopefully allow python to use the
 * changes for something else somewhere */
static PyObject *request_image(PyObject *self) {

  move_dots(dot_arr, dots_count, screen_width, screen_height);
  erase_screen(PyArray_DATA(screen), screen_width, screen_height);
  draw_dots(dot_arr, PyArray_DATA(screen), dots_count, screen_width, screen_height);

  Py_RETURN_NONE;
}



/* Function to deinitialize all of the data that I have created. The
 * Python program must call this when it is done with its dots to
 * ensure that we aren't leaking any memory */
static PyObject *uncreate_dots(PyObject *self) {
  free(dot_arr);
  free(screen_buffer);
  Py_RETURN_NONE;
}


static PyMethodDef _accelerated_dots_methods[] = {
    {"create_dots", (PyCFunction)create_dots, METH_VARARGS},
    {"uncreate_dots", (PyCFunction)uncreate_dots, METH_NOARGS},
    {"request_image", (PyCFunction)request_image, METH_NOARGS},
    { NULL, NULL, 0, NULL}
};

static struct PyModuleDef FirstModule = {
    PyModuleDef_HEAD_INIT,
    "FirstModule",
    "Not sure what im doing here",
    -1,
    _accelerated_dots_methods
};

PyMODINIT_FUNC PyInit_AcceleratedDots(void) {
  
  PyObject* obj = PyModule_Create(&FirstModule);
  import_array();
  return obj;
   
}