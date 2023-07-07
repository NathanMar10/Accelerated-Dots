
#include <Python.h>
#include <stdlib.h>
#define PY_ARRAY_UNIQUE_SYMBOL
#include <arrayobject.h>
#include "dot-datastructure.h"

/* Macro to get a random number between the given MAX and MIN value */
#define GET_RANDOM(MIN, MAX) ( ( rand() / RAND_MAX ) * (MAX + MIN) - MIN )


Dot *const dot_arr;
PyArrayObject* screen;
unsigned int const screen_width, const screen_height;

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
min_size
max_size
seed

These are then parsed to  initialize the dots structure and create
a numpy array to store the next image in.
Later on, this will be updated to create other configurations like
collision type, COR, G, K, and others (Including optional Mass)*/

static PyObject *create_dots(PyObject *self, PyObject *args) {

    unsigned int dots_count, seed;
    double min_velocity, max_velocity, min_size, max_size;

  /* Loads the numpy array into the array variable so that I can
   * play with it */
  
  if (!PyArg_ParseTuple("O!IIIIdddd", &PyArray_Type, &screen, &screen_width,
			&screen_height, &dots_count, &seed,
			&min_velocity, &max_velocity, &min_size,
			&max_size)) {
            return NULL;
    }

    /* If a seed is given, we will set it */
    if (seed != 0) {
        srand(seed);
    }

    /* Array that will store all of the dots */  
    dot_arr = malloc(dots_count * sizeof(Dot));

    /* For loop initializes each dot in the array */
    for (int i = 0; i < dots_count; i++) {
        dot_arr[i].velx = GET_RANDOM(min_velocity, max_velocity);
	dot_arr[i].vely = GET_RANDOM(min_velocity, max_velocity);
	dot_arr[i].posx = GET_RANDOM(0, screen_width);
	dot_arr[i].posy = GET_RANDOM(0, screen_height);
	dot_arr[i].size = GET_RANDOM(min_size, max_size);
	dot_arr[i].color.r = GET_RANDOM(0, 255);
	dot_arr[i].color.g = GET_RANDOM(0, 255);
	dot_arr[i].color.b = GET_RANDOM(0, 255);
    }

    /* Initializing the Array. When I ask for a new image,
     * I'll ask for a numpy array argument and copy in colors
     * as I go */

    screen = malloc(sizeof(Color) * screen_width * screen_height);

    set_screen_white();
    
    Py_RETURN_NONE;
}

/* Function for Python to request the next from from C. Args will
 * contain a pointer to a numpy array that we can use
 * to run my image into to display the circles */
static PyObject *request_image(PyObject *self) {





  
  Py_RETURN_NONE;
}



/* Function to deinitialize all of the data that I have created. The
 * Python program must call this when it is done with its dots to
 * ensure that we aren't leaking any memory */
static PyObject *uncreate_dots(PyObject *self) {
  free(dot_arr);
  free(screen);

  Py_RETURN_NONE;
}


static PyMethodDef first_module_methods[] = {
    {"create_dots", (PyCFunction)create_dots, METH_VARARGS},
    {"uncreate_dots", (PyCFunction)uncreate_Dots, METH_NOARGS},
    { NULL, NULL, 0, NULL}
};

static struct PyModuleDef FirstModule = {
    PyModuleDef_HEAD_INIT,
    "FirstModule",
    "Not sure what im doing here",
    -1,
    first_module_methods
};

PyMODINIT_FUNC PyInit_FirstModule(void) {
    /* This is the initialization function, this should be called by things */

    PyModule_Create(&FirstModule);
}


static void set_screen_gray(void) {
  for (int i = 0; i < screen_width; i++) {
    for (int j = 0; j < screen_width; j++) {
      for (int k = 0; k < screen_width; k++) {
	screen -> data + i * screen -> strides[0] +
	  j * screen -> strides[1] + k * screen -> strides[2] = 100;
      }
    }
  }
}

