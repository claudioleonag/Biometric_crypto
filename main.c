/* Lib Includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/* Header Files */
#include "OLED_CONTROLER/OLED_CONTROLER.h"


int main()
{
    printf("olá do %s!\n", "Biometric_crypto");
    init_oled();
    write_oled("TCC2",1,0,3);
    write_oled("Claudio Leon",1,30,1); //(x, y, tamanho fonte)
    return 0;
}