/* Lib Includes */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>


/* Header Files */
#include "include/OLED_CONTROLER.h"
#include "include/bio_reader.h"

typedef unsigned char BYTE;
typedef unsigned long DWORD;

int main()
{
    bool matched;
    DWORD *score;
    BYTE *templateBuffer1;
    templateBuffer1 = (BYTE*) malloc(800);
    BYTE *templateBuffer2;
    templateBuffer2 = (BYTE*) malloc(800);    
    BYTE *imageBuffer;
    imageBuffer = (BYTE*) malloc(400*800);
    int err;
    printf("\n-------------------------------------\n");
    printf(  "TCC2 - Claudio Leon\n");
    printf(  "-------------------------------------\n");

    printf("olá do %s!\n", "Biometric_crypto");
    //init_oled();
    //write_oled("TCC2",1,0,3);
    //write_oled("Claudio Leon",1,30,1); //(x, y, tamanho fonte)
 
    if (err = open_reader()) // inicia o leitor
    {
        printf("Falha em abrir o leitor");
    }
    else
    {
        printf("\nSucesso em abrir o leitor\n");
        //write_oled("Leitor iniciado",1,0,1);
    }  

    //menu implementation

    int command;
    printf("\n\n\n-- MENU --\n");
    printf("r - Register new fingerprint\n");
    printf("m - Match a fingerprint\n");
    printf("d - Delete a fingerprint\n");
    printf("c - Close program\n");

    while ((command = getchar()) != EOF)
    {
        printf("\n\n\n-- MENU --\n");
        printf("r - Register new fingerprint\n");
        printf("m - Match a fingerprint\n");
        printf("d - Delete a fingerprint\n");
        printf("c - Close program\n");

        switch(command)
        {

        case 'r':   

            read_finger(imageBuffer);
            create_template(imageBuffer, templateBuffer1);
            //write_oled("Register Sucess",1,0,1);
            break;

        case 'm':
            read_finger(imageBuffer);
            create_template(imageBuffer, templateBuffer2);
            matched = match_finger(templateBuffer1, templateBuffer2, score);
            if (matched)
            {
                //write_oled("Finger Matched",1,0,1);
                printf("<<MATCH>> realizado com sucesso.");    
            }
            else
            {
                printf("<< NO MATCH >>. %d");        
            }
            break;

        case 'c':
            close_reader();
            free(imageBuffer);
            free(templateBuffer1);
            free(templateBuffer2);
            printf("\nThanks!!");
            return 0;
    
        }
    }
    return 0;
}
