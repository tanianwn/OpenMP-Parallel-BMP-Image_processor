#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define input_path "BMPIMAGES/"
#define OUTPUT_PATH "OUTIMAGES/"
#define NUM_THREADS 1300

//declaracion de funciones
extern double tiempo_inv_ing;
extern double tiempo_to_gray;
extern double tiempo_invert_vertical;
extern double tiempo_color_blur;


// Declaraciones de funciones
void inv_ing(char mask[10], char path[80]);
void to_gray(char path[80]);
void invert_vertical(char path[80]);
//void inv_color(char mask[10], char path[80]);
void color_blur(char mask[10], char path[80]);

#endif

