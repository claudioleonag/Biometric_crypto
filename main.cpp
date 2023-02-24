/* Lib Includes */
#include "include/sgfplib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/* Header Files */
#include "include/OLED_CONTROLER.h"

LPSGFPM  sgfplib = NULL;

int  getPIVQuality(int quality)
{
    if (quality <= 20)
        return 20;
    if (quality <= 40)
        return 40;
    if (quality <= 60)
        return 60;
    if (quality <= 80)
        return 80;

    return 100;
}


int main()
{
    printf("olá do %s!\n", "Biometric_crypto");
    init_oled();
    write_oled("TCC2",1,0,3);
    write_oled("Claudio Leon",1,30,1); //(x, y, tamanho fonte)
 

    //-------------------------------//
    //variáveis leitor

    long err;
    DWORD templateSize, templateSizeMax;
    DWORD quality;
    char function[100];
    char kbBuffer[100];
    char kbWhichFinger[100];
    int fingerLength = 0;
    char *finger;
    BYTE *imageBuffer1;
    BYTE *imageBuffer2;
    BYTE *imageBuffer3;
    BYTE *minutiaeBuffer1;
    BYTE *minutiaeBuffer2;
    BYTE *ansiMinutiaeBuffer1;
    BYTE *ansiMinutiaeBuffer2;
    BYTE *isoMinutiaeBuffer1;
    BYTE *isoMinutiaeBuffer2;
    BYTE *isoCompactMinutiaeBuffer1;
    BYTE *isoCompactMinutiaeBuffer2;
    FILE *fp = NULL;
    SGDeviceInfoParam deviceInfo;
    DWORD score;
    SGFingerInfo fingerInfo;
    BOOL matched;
    DWORD nfiq;
    DWORD numOfMinutiae;

    for (int i=0; i < 100; ++i)
        kbWhichFinger[i] = 0x00; //zera vetor de digitais

    printf("\n-------------------------------------\n");
    printf(  "TCC2 - Claudio Leon\n");
    printf(  "-------------------------------------\n");

    ///////////////////////////////////////////////
    // Instantiate SGFPLib object
    strcpy(function,"CreateSGFPMObject()");
    printf("\nCall %s\n",function);
    err = CreateSGFPMObject(&sgfplib);
    if (!sgfplib)
    {
        printf("ERROR - Unable to instantiate FPM object.\n\n");
        return -1;
    }
    printf("%s returned: %ld\n",function,err);

    ///////////////////////////////////////////////
    // Init()
    strcpy(function,"Init(SG_DEV_AUTO)");
    printf("\nCall %s\n",function);
    err = sgfplib->Init(SG_DEV_AUTO);
    printf("%s returned: %ld\n",function,err);

    if (err != SGFDX_ERROR_NONE)
    {
        printf("ERROR - Unable to initialize device.\n\n");
        return -1;
    }

    ///////////////////////////////////////////////
    // OpenDevice()
    strcpy(function,"OpenDevice(0)");
    printf("\nCall %s\n",function);
    err = sgfplib->OpenDevice(0);
    printf("%s returned: %ld\n",function,err);

    if (err == SGFDX_ERROR_NONE)
    {

        ///////////////////////////////////////////////
        // setBrightness()
        strcpy(function,"SetBrightness()");
        printf("\nCall %s\n",function);
        err = sgfplib->SetBrightness(100);
        printf("%s returned: %ld\n",function,err);
        
        ///////////////////////////////////////////////
        // setLedOn(true) - Startup sequence
        strcpy(function,"SetLedOn(true)");
        printf("\nCall %s\n",function);
        err = sgfplib->SetLedOn(true);
        usleep(1000000);
        err = sgfplib->SetLedOn(false);
        usleep(1000000);
        err = sgfplib->SetLedOn(true);
        usleep(1000000);
        err = sgfplib->SetLedOn(false);
        usleep(1000000);
        err = sgfplib->SetLedOn(true);
        printf("%s returned: %ld\n",function,err);
        getc(stdin);
    }

    ///////////////////////////////////////////////
    // closeDevice()TEMPLATE_FORMAT_ISO19794
    printf("\nCall CloseDevice()\n");
    err = sgfplib->CloseDevice();
    printf("CloseDevice returned : [%ld]\n",err);

    ///////////////////////////////////////////////
    // Destroy SGFPLib object
    strcpy(function,"DestroySGFPMObject()");
    printf("\nCall %s\n",function);
    err = DestroySGFPMObject(sgfplib);
    printf("%s returned: %ld\n",function,err);

    free(imageBuffer1);
    free(imageBuffer2);
    free(minutiaeBuffer1);
    free(minutiaeBuffer2);
    free(ansiMinutiaeBuffer1);
    free(ansiMinutiaeBuffer2);
    free(isoMinutiaeBuffer1);
    free(isoMinutiaeBuffer2);
    free(finger);
    imageBuffer1 = NULL;
    imageBuffer2 = NULL;
    minutiaeBuffer1 = NULL;
    minutiaeBuffer2 = NULL;
    ansiMinutiaeBuffer1 = NULL;
    ansiMinutiaeBuffer2 = NULL;
    isoMinutiaeBuffer1 = NULL;
    isoMinutiaeBuffer2 = NULL;
    finger = NULL;


    return 0;

}