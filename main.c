#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "funciones.h"

//Paraleliza con sections
void images(const char *mask, const char *path) {
    printf("Procesando imagen %s en thread %d\n", path, omp_get_thread_num());
    inv_ing((char *)mask, (char *)path);
    to_gray((char *)path);
    invert_vertical((char *)path);
    color_blur((char *)mask, (char *)path);
}

int main() {
    omp_set_num_threads(1300); 
    
    double t0 = omp_get_wtime();   //tiempo

    #pragma omp parallel
    {
        #pragma omp sections
        {
            #pragma omp section
            {
                images("01", "01.bmp");
            }

            #pragma omp section
            {
                images("02", "02.bmp");
            }

            #pragma omp section
            {
                images("03", "03.bmp");
            }

            #pragma omp section
            {
                images("04", "04.bmp");
            }

            #pragma omp section
            {
                images("05", "05.bmp");
            }
            
            #pragma omp section
            {
                images("06", "06.bmp");
            }
            
            #pragma omp section
            {
                images("07", "07.bmp");
            }

            #pragma omp section
            {
                images("08", "08.bmp");
            }

            #pragma omp section
            {
                images("09", "09.bmp");
            }

            #pragma omp section
            {
                images("10", "10.bmp");
            }

            #pragma omp section
            {
                images("11", "11.bmp");
            }
            
            #pragma omp section
            {
                images("12", "12.bmp");
            }
            #pragma omp section
            {
                images("13", "13.bmp");
            }

            #pragma omp section
            {
                images("14", "14.bmp");
            }

            #pragma omp section
            {
                images("15", "15.bmp");
            }

            #pragma omp section
            {
                images("16", "16.bmp");
            }

            #pragma omp section
            {
                images("17", "17.bmp");
            }
            
            #pragma omp section
            {
                images("18", "18.bmp");
            }
            
            #pragma omp section
            {
                images("19", "19.bmp");
            }
            
            #pragma omp section
            {
                images("20", "20.bmp");
            }
            #pragma omp section
            {
                images("21", "21.bmp");
            }

            #pragma omp section
            {
                images("22", "22.bmp");
            }

            #pragma omp section
            {
                images("23", "23.bmp");
            }

            #pragma omp section
            {
                images("24", "24.bmp");
            }

            #pragma omp section
            {
                images("25", "25.bmp");
            }
            
            #pragma omp section
            {
                images("26", "26.bmp");
            }
            #pragma omp section
            {
                images("27", "27.bmp");
            }

            #pragma omp section
            {
                images("28", "28.bmp");
            }
            
            #pragma omp section
            {
                images("29", "29.bmp");
            }
            
            #pragma omp section
            {
                images("30", "30.bmp");
            }
  
            
        }
    }
    
    double t1 = omp_get_wtime();   //fin tiempo total
    printf("\n °☆•° TIEMPOS TOTALES °•☆° \n");
    printf("Tiempo total de procesamiento: %f segundos\n", t1 - t0);
    //printf("inv_ing total:           %f segundos\n", tiempo_inv_ing);
    //printf("to_gray total:           %f segundos\n", tiempo_to_gray);
    //printf("invert_vertical total:   %f segundos\n", tiempo_invert_vertical);
    //printf("color_blur total:        %f segundos\n", tiempo_color_blur);

    return 0;
}




//paraleliza sin sections
/*
int main() {
    omp_set_num_threads(NUM_THREADS);  //paralelizado
    char mask[10] = "25";
    char path[80] = "25.bmp";

    inv_ing(mask, path);         // gris + horizontal
    to_gray(path);               // solo gris
    invert_vertical(path);       // gris + vertical
    //inv_color(mask, path);       // color + horizontal 
    color_blur(mask, path);     // blur + color

    return 0;
}
*/
