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




int main()
{
    int err;
    printf("\n-------------------------------------\n");
    printf(  "TCC2 - Claudio Leon\n");
    printf(  "-------------------------------------\n");

    printf("olá do %s!\n", "Biometric_crypto");
    init_oled();
    write_oled("TCC2",1,0,3);
    write_oled("Claudio Leon",1,30,1); //(x, y, tamanho fonte)
 
    if (err != open_reader()) // inicia o leitor
    {
        printf("Falha em abrir o leitor");
    }
    else
    {
        printf("\nSucesso em abrir o leitor\n");
        write_oled("Leitor iniciado",1,0,1);
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

        switch(command)
        {

        case 'r':
            read_finger();
            create_template();
            write_oled("Register Sucess",1,0,1);
            break;

        case 'm':
            break;

        case 'c':
            close_reader();
            printf("\nThanks!!");
            return 0;
    
        }
        printf("r - Register new fingerprint\n");
        printf("m - Match a fingerprint\n");
        printf("d - Delete a fingerprint ID\n");
    }
    return 0;
}
