//definir .h 
#include "funciones.h"


//includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//abre 
#define input_path "BMPIMAGES/"
//guarda imagenes
#define OUTPUT_PATH "OUTIMAGES/"
//lib para paralelizar
#include <omp.h>
//#define NUM_THREADS 30

//para toamr el tiempo
double tiempo_inv_ing = 0.0;
double tiempo_to_gray = 0.0;
double tiempo_invert_vertical = 0.0;
double tiempo_color_blur = 0.0;


// pasar a gris + invertir //tomar el tiempo
void inv_ing(char mask[10], char path[80]) {
    //double t0 = omp_get_wtime(); //tomar el tiempo
    FILE *image, *outputImage;
    unsigned char r, g, b, pixel;
    unsigned char *arr_in;
    unsigned char header[54];
    long ancho, alto, tam, bpp;
    int j = 0;
    
    //sacar y leer la imagen
    char outputPath[100];
    char inputPath[200];
    snprintf(inputPath, sizeof(inputPath), "%s%s", input_path, path); 
    snprintf(outputPath, sizeof(outputPath), "%s%s_grey_inv_%s", OUTPUT_PATH, mask, path);


    image = fopen(inputPath, "rb");
    if (!image) {
        printf("Error al abrir la imagen %s\n", path);
        return;
    }

    outputImage = fopen(outputPath, "wb");
    if (!outputImage) {
        printf("Error al crear la imagen %s\n", outputPath);
        fclose(image);
        return;
    }

    for (int i = 0; i < 54; i++) {
        header[i] = fgetc(image);
        fputc(header[i], outputImage);
    }

    tam    = (long)header[2] + ((long)header[3] << 8) + ((long)header[4] << 16) + ((long)header[5] << 24);
    ancho  = (long)header[18] + ((long)header[19] << 8) + ((long)header[20] << 16) + ((long)header[21] << 24);
    alto   = (long)header[22] + ((long)header[23] << 8) + ((long)header[24] << 16) + ((long)header[25] << 24);
    bpp    = (long)header[28] + ((long)header[29] << 8);

    printf("Tamaño archivo: %ld bytes\n", tam);
    printf("Ancho: %ld px\n", ancho);
    printf("Alto: %ld px\n", alto);
    printf("Bits por pixel: %ld\n", bpp);

    int bytesPerPixel = bpp / 8;
    int rowSize = (ancho * bytesPerPixel + 3) & (~3);
    int padding = rowSize - (ancho * bytesPerPixel);

    printf("Padding por fila: %d bytes\n", padding);

    arr_in = (unsigned char*) malloc(ancho * alto * sizeof(unsigned char));
    if (!arr_in) {
        printf("No se pudo reservar memoria\n");
        fclose(image);
        fclose(outputImage);
        return;
    }
    
    for (long row = 0; row < alto; row++) {
        for (long col = 0; col < ancho; col++) {
            b = fgetc(image);
            g = fgetc(image);
            r = fgetc(image);
            if (feof(image)) break;
            pixel = (unsigned char)(0.21*r + 0.72*g + 0.07*b);
            arr_in[row * ancho + col] = pixel;
        }
        for (int k = 0; k < padding; k++) fgetc(image);
    }

    printf("Lectura de datos completa.\n");

    //Invertir horizontalmente
    //paralelizado
    #pragma omp parallel for
    for (long row = 0; row < alto; row++) {
        for (long col = 0; col < ancho / 2; col++) {
            unsigned char tmp = arr_in[row * ancho + col];
            arr_in[row * ancho + col] = arr_in[row * ancho + (ancho - col - 1)];
            arr_in[row * ancho + (ancho - col - 1)] = tmp;
        }
    }

    for (long row = alto - 1; row >= 0; row--) {
        for (long col = 0; col < ancho; col++) {
            unsigned char px = arr_in[row * ancho + col];
            fputc(px, outputImage);
            fputc(px, outputImage);
            fputc(px, outputImage);
        }
        for (int k = 0; k < padding; k++) fputc(0x00, outputImage);
    }

    free(arr_in);
    fclose(image);
    fclose(outputImage);
    printf("Imagen en gris e invertida horizontal guardada como: %s\n",outputPath);
    
   // tiempo_inv_ing += omp_get_wtime() - t0; //tiempo final
    //printf("Ejecutando gris e invertida en thread %d\n", omp_get_thread_num());

}
    



// SOLO ESCALA DE GRISES (gris.bmp)

void to_gray(char path[80]) {
    //double t0 = omp_get_wtime(); //tiempo
    FILE *image, *outputImage;
    unsigned char r, g, b, pixel;
    unsigned char header[54];
    long ancho, alto, tam, bpp;

    char inputPath[200], outputPath[200];
    snprintf(inputPath, sizeof(inputPath), "%s%s", input_path, path);
    snprintf(outputPath, sizeof(outputPath), "%sgray_%s", OUTPUT_PATH, path);

    image = fopen(inputPath, "rb");
    if (!image) {
        printf("Error al abrir %s\n", path);
        return;
    }

    outputImage = fopen(outputPath, "wb");
    if (!outputImage) {
        printf("Error al crear gris.bmp\n");
        fclose(image);
        return;
    }

    for (int i = 0; i < 54; i++) {
        header[i] = fgetc(image);
        fputc(header[i], outputImage);
    }

    tam    = (long)header[2] + ((long)header[3] << 8) + ((long)header[4] << 16) + ((long)header[5] << 24);
    ancho  = (long)header[18] + ((long)header[19] << 8) + ((long)header[20] << 16) + ((long)header[21] << 24);
    alto   = (long)header[22] + ((long)header[23] << 8) + ((long)header[24] << 16) + ((long)header[25] << 24);
    bpp    = (long)header[28] + ((long)header[29] << 8);

    int bytesPerPixel = bpp / 8;
    int rowSize = (ancho * bytesPerPixel + 3) & (~3);
    int padding = rowSize - (ancho * bytesPerPixel);

    
    //reservar memoria para toda la imagen (primero se lee toda la imagen)
    unsigned char *data = (unsigned char*) malloc(rowSize * alto); //buffer
    if (!data) {
        printf("No se pudo reservar memoria\n");
        fclose(image);
        fclose(outputImage);
        return;
    }

    //leer toda la imagen
    fread(data, 1, rowSize * alto, image);
    
    //procesamiento
    //paralelizado
    #pragma omp parallel for collapse(2) //paraleliza el mismo tiempo 
    for (long row = 0; row < alto; row++) {
        for (long col = 0; col < ancho; col++) {
            long idx = row * rowSize + col * 3; //new
            unsigned char b = data[idx]; // b = fgetc(image);
            unsigned char g = data[idx + 1]; //  g = fgetc(image);
            unsigned char r = data[idx + 2]; // r = fgetc(image);
            unsigned char gray = (unsigned char)(0.21*r + 0.72*g + 0.07*b);
            data[idx] = data[idx + 1] = data[idx + 2] = gray;
        }
    }
    //escribir imagen
    fwrite(data, 1, rowSize * alto, outputImage);
    
    free(data);
    fclose(image);
    fclose(outputImage);
    
    printf("Imagen en escala de grises guardada como: %s\n", outputPath);
    //printf("Ejecutando to_gray en thread %d\n", omp_get_thread_num());
  
//  tiempo_to_gray += omp_get_wtime() - t0; //tiempo final
}



// INVERTIR VERTICALMENTE (gris_vertical.bmp)

void invert_vertical(char path[80]) {
    //double t0 = omp_get_wtime(); //tiempo
    FILE *image, *outputImage;
    unsigned char header[54];
    unsigned char *arr_in;
    unsigned char r, g, b, pixel;
    long ancho, alto, tam, bpp;

    char inputPath[200], outputPath[200];
    snprintf(inputPath, sizeof(inputPath), "%s%s", input_path, path);
    snprintf(outputPath, sizeof(outputPath), "%sgray_vertical_%s", OUTPUT_PATH, path);

    image = fopen(inputPath, "rb");
    if (!image) {
        printf("Error al abrir %s\n", path);
        return;
    }

    outputImage = fopen(outputPath, "wb");
    if (!outputImage) {
        printf("Error al crear gris_vertical.bmp\n");
        fclose(image);
        return;
    }

    // Leer y copiar cabecera
    for (int i = 0; i < 54; i++) {
        header[i] = fgetc(image);
        fputc(header[i], outputImage);
    }

    // Extraer datos de la cabecera
    tam   = (long)header[2] + ((long)header[3] << 8) + ((long)header[4] << 16) + ((long)header[5] << 24);
    ancho = (long)header[18] + ((long)header[19] << 8) + ((long)header[20] << 16) + ((long)header[21] << 24);
    alto  = (long)header[22] + ((long)header[23] << 8) + ((long)header[24] << 16) + ((long)header[25] << 24);
    bpp   = (long)header[28] + ((long)header[29] << 8);

    int bytesPerPixel = bpp / 8;
    int rowSize = (int)((ancho * bytesPerPixel + 3) & (~3));
    int padding = rowSize - (ancho * bytesPerPixel);

    // Reservar memoria
    arr_in = (unsigned char*) malloc((size_t)ancho * (size_t)alto);
    if (!arr_in) {
        printf("No se pudo reservar memoria\n");
        fclose(image);
        fclose(outputImage);
        return;
    }

    // Leer imagen y convertir a grisLA
    for (long row = 0; row < alto; row++) {
        for (long col = 0; col < ancho; col++) {
            b = fgetc(image);
            g = fgetc(image);
            r = fgetc(image);
            if (feof(image)) break;
            pixel = (unsigned char)(0.21*r + 0.72*g + 0.07*b);
            arr_in[row * ancho + col] = pixel;
        }
        for (int k = 0; k < padding; k++) fgetc(image);
    }
    

    // Reflejar horizontalmente (espejo)
    //paralelizado
    #pragma omp parallel for
    for (long row = 0; row < alto; row++) {
        for (long col = 0; col < ancho / 2; col++) {
            unsigned char tmp = arr_in[row * ancho + col];
            arr_in[row * ancho + col] = arr_in[row * ancho + (ancho - 1 - col)];
            arr_in[row * ancho + (ancho - 1 - col)] = tmp;
        }
    }

    // Escribir imagen sin invertir verticalmente (mantiene arriba/abajo igual)
    for (long row = 0; row < alto; row++) {
        for (long col = 0; col < ancho; col++) {
            unsigned char px = arr_in[row * ancho + col];
            fputc(px, outputImage);
            fputc(px, outputImage);
            fputc(px, outputImage);
        }
        for (int k = 0; k < padding; k++) fputc(0x00, outputImage);
    }

    free(arr_in);
    fclose(image);
    fclose(outputImage);
    printf("Imagen gris invertida verticalmente guardada como: %s\n", outputPath);
    //printf("Ejecutando gris invertida en thread %d\n", omp_get_thread_num());
    //tiempo_invert_vertical += omp_get_wtime() - t0; //tiempo final

}
    
    
    
//invierte a color 
void inv_color(char mask[10], char path[80]){
    FILE *image, *outputImage;
    //char add_char[80] = "./img/";
    //strcat(add_char, mask);
    //strcat(add_char, ".bmp");
    //printf("%s\n", add_char);
    char inputPath[200], outputPath[200];
    snprintf(inputPath, sizeof(inputPath), "%s%s", input_path, path);
    snprintf(outputPath, sizeof(outputPath), "%simg_color_inv_%s", OUTPUT_PATH, path);

    image = fopen(inputPath,"rb");
    if (!image) {
        printf("Error al abrir %s\n", path);
        return;
    }
    outputImage = fopen(outputPath,"wb");
    if (!outputImage) {
        printf("Error al crear la imagen de salida\n");
        fclose(image);
        return;
    }

    // Lectura de cabecera BMP
    unsigned char xx[54];
    for (int i = 0; i < 54; i++) {
        xx[i] = fgetc(image);
        fputc(xx[i], outputImage);
    }

    long tam = (long)xx[4]*65536+(long)xx[3]*256+(long)xx[2];
    long ancho = (long)xx[20]*65536+(long)xx[19]*256+(long)xx[18];
    long alto  = (long)xx[24]*65536+(long)xx[23]*256+(long)xx[22];

    printf("Invertir color: ancho = %li, alto = %li\n", ancho, alto);

    int rowSize = (ancho * 3 + 3) & (~3);
    unsigned char *pixelData = (unsigned char*)malloc(rowSize * alto);

    for (long i = 0; i < alto; i++) {
        fread(&pixelData[i * rowSize], 1, rowSize, image);
    }

    // Invertir de cabeza (verticalmente)
    //paralelizado
    #pragma omp parallel for
    for (long i = alto - 1; i >= 0; i--) {
        fwrite(&pixelData[i * rowSize], 1, rowSize, outputImage);
    }

    free(pixelData);
    fclose(image);
    fclose(outputImage);
    printf("Imagen invertida a color guardada como: %s\n", outputPath);
    //printf("Ejecutando imagen invertida a color en thread %d\n", omp_get_thread_num());

}


// BLUR A IMAGEN A COLOR 

void color_blur(char mask[10], char path[80]){
    //double t0 = omp_get_wtime(); //tiempo
    FILE *image, *outputImage;
    
    //char add_char[80] = "./img/";
    //strcat(add_char, mask);
    //strcat(add_char, ".bmp");
    //printf("%s\n", add_char);
    
    char inputPath[200], outputPath[200];
    snprintf(inputPath, sizeof(inputPath), "%s%s", input_path, path);
    snprintf(outputPath, sizeof(outputPath), "%simg_color_blur_%s", OUTPUT_PATH, path);

    image = fopen(inputPath, "rb");
    if (!image) {
        printf("Error al abrir %s\n", path);
        return;
    }
    outputImage = fopen(outputPath, "wb");
    if (!outputImage) {
        printf("Error al crear img_color_blur.bmp\n");
        fclose(image);
        return;
    }

    unsigned char header[54];
    for (int i = 0; i < 54; i++) {
        header[i] = fgetc(image);
        fputc(header[i], outputImage);
    }

    long ancho = (long)header[18] + ((long)header[19] << 8) + ((long)header[20] << 16) + ((long)header[21] << 24);
    long alto  = (long)header[22] + ((long)header[23] << 8) + ((long)header[24] << 16) + ((long)header[25] << 24);
    int rowSize = (ancho * 3 + 3) & (~3);

    unsigned char *pixelData = (unsigned char*)malloc(rowSize * alto);
    unsigned char *outputData = (unsigned char*)malloc(rowSize * alto);
    if (!pixelData || !outputData) {
        printf("Error de memoria\n");
        fclose(image);
        fclose(outputImage);
        return;
    }

    for (long i = 0; i < alto; i++) {
        fread(&pixelData[i * rowSize], 1, rowSize, image);
    }

    int kernelRadius = 57;
    //paralelizado
    #pragma omp parallel for
    for (long y = 0; y < alto; y++) {
        for (long x = 0; x < ancho; x++) {
            unsigned int rSum = 0, gSum = 0, bSum = 0, count = 0;
            for (int ky = -kernelRadius; ky <= kernelRadius; ky++) {
                for (int kx = -kernelRadius; kx <= kernelRadius; kx++) {
                    long ny = y + ky, nx = x + kx;
                    if (ny < 0 || ny >= alto || nx < 0 || nx >= ancho) continue;
                    int idx = ny * rowSize + nx * 3;
                    bSum += pixelData[idx];
                    gSum += pixelData[idx + 1];
                    rSum += pixelData[idx + 2];
                    count++;
                }
            }
            int outIdx = y * rowSize + x * 3;
            outputData[outIdx] = bSum / count;
            outputData[outIdx + 1] = gSum / count;
            outputData[outIdx + 2] = rSum / count;
        }
    }

    for (long i = 0; i < alto; i++) {
        fwrite(&outputData[i * rowSize], 1, rowSize, outputImage);
    }

    free(pixelData);
    free(outputData);
    fclose(image);
    fclose(outputImage);
    printf("Imagen a color con blur guardada como: %s\n", outputPath);
    //printf("Ejecutando blur a color en thread %d\n", omp_get_thread_num());
    //tiempo_color_blur += omp_get_wtime() - t0; //tiempo final 

}


//Main
/*
int main() {
    omp_set_num_threads(NUM_THREADS);  //paralelizado
    char mask[10] = "10";
    char path[80] = "10.bmp";

    inv_ing(mask, path);         // gris + horizontal
    to_gray(path);               // solo gris
    invert_vertical(path);       // gris + vertical
    //inv_color(mask, path);       // color + horizontal 
    color_blur(mask, path);     // blur + color

    return 0;
}
*/
