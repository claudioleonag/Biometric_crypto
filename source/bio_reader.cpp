
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include "../include/bio_reader.h"
#include "../include/sgfplib.h"
#include "../include/sodium.h"

int printf_ByteArray(const unsigned char *data, size_t len) {
  size_t i;
  int result = 0;
  for (i = 0; i < len; i++) {
    int y;
    int ch = data[i];
    static char escapec[] = "\a\b\t\n\v\f\n\'\"\?\\";
    char *p = strchr(escapec, ch);
    if (p && ch) {
      static char escapev[] = "abtnvfn\'\"\?\\";
      y = printf("\\%c", escapev[p - escapec]);
    } else if (isprint(ch)) {
      y = printf("%c", ch);
    } else {
      // If at end of array, assume _next_ potential character is a '0'.
      int nch = i >= (len - 1) ? '0' : data[i + 1];
      if (ch < 8 && (nch < '0' || nch > '7')) {
        y = printf("\\%o", ch);
      } else if (!isxdigit(nch)) {
        y = printf("\\x%X", ch);
      } else {
        y = printf("\\o%03o", ch);
      }
    }
    if (y == EOF)
      return EOF;
    result += y;
  }
  return result;
}

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
//variaveis cripto

#define MESSAGE_LEN 800
#define CIPHERTEXT_LEN (crypto_secretbox_MACBYTES + MESSAGE_LEN)

unsigned char key[crypto_secretbox_KEYBYTES];
unsigned char nonce[crypto_secretbox_NONCEBYTES];
unsigned char ciphertext[CIPHERTEXT_LEN];
unsigned char decrypted[MESSAGE_LEN];


//variaveis leitor biometrico
long err;
DWORD templateSize, templateSizeMax;
DWORD quality;
char function[100];
char kbBuffer[100];
char kbWhichFinger[100];
int fingerLength = 0;
char *finger;
int fingerID = 0;
FILE *fp = NULL; 
SGDeviceInfoParam deviceInfo;

SGFingerInfo fingerInfo;
BOOL matched;
DWORD nfiq;
DWORD numOfMinutiae;

LPSGFPM sgfplib = NULL;

int open_reader()
{
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
    // Initiate device
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
        usleep(100000);
        err = sgfplib->SetLedOn(false);
        usleep(100000);
        err = sgfplib->SetLedOn(true);
        usleep(100000);
        err = sgfplib->SetLedOn(false);
        usleep(100000);
        err = sgfplib->SetLedOn(true);
        printf("%s returned: %ld\n",function,err);

        ///////////////////////////////////////////////
        // getDeviceInfo()
        deviceInfo.DeviceID = 0;
        strcpy(function,"GetDeviceInfo()");
        printf("\nCall %s\n",function);
        err = sgfplib->GetDeviceInfo(&deviceInfo);
        printf("%s returned: %ld\n",function,err);
        if (err == SGFDX_ERROR_NONE)
        {
            printf("\tdeviceInfo.DeviceID   : %ld\n", deviceInfo.DeviceID);
            printf("\tdeviceInfo.DeviceSN   : %s\n",  deviceInfo.DeviceSN);
            printf("\tdeviceInfo.ComPort    : %ld\n", deviceInfo.ComPort);
            printf("\tdeviceInfo.ComSpeed   : %ld\n", deviceInfo.ComSpeed);
            printf("\tdeviceInfo.ImageWidth : %ld\n", deviceInfo.ImageWidth);
            printf("\tdeviceInfo.ImageHeight: %ld\n", deviceInfo.ImageHeight);
            printf("\tdeviceInfo.Contrast   : %ld\n", deviceInfo.Contrast);
            printf("\tdeviceInfo.Brightness : %ld\n", deviceInfo.Brightness);
            printf("\tdeviceInfo.Gain       : %ld\n", deviceInfo.Gain);
            printf("\tdeviceInfo.ImageDPI   : %ld\n", deviceInfo.ImageDPI);
            printf("\tdeviceInfo.FWVersion  : %04X\n", (unsigned int) deviceInfo.FWVersion);  
        }
    }
    
    return 0;
}

void read_finger(BYTE *imageBuffer1)
{ 
    do
    {    
    ///////////////////////////////////////////////
    // getImage() - 1st Capture
    printf("\n Please place your finger on sensor and press <ENTER> ");
    //imageBuffer1 = (BYTE*) malloc(deviceInfo.ImageHeight*deviceInfo.ImageWidth); - removido pois quebra o ponteiro
    strcpy(function,"GetImage()");
    printf("\nCall %s\n",function);
    err = sgfplib->GetImage(imageBuffer1);
    printf("%s returned: %ld\n",function,err);
    /*if (err == SGFDX_ERROR_NONE)
    {
        sprintf(kbBuffer,"fingerData/digital_%i.raw",fingerID);
        fp = fopen(kbBuffer,"wb"); 
        fwrite (imageBuffer1 , sizeof (BYTE) , deviceInfo.ImageWidth*deviceInfo.ImageHeight , fp);
        fclose(fp);
    }*/

    ///////////////////////////////////////////////
    // getImageQuality()
    strcpy(function,"GetImageQuality()");
    printf("\nCall %s\n",function);
    err = sgfplib->GetImageQuality(deviceInfo.ImageWidth, deviceInfo.ImageHeight, imageBuffer1, &quality);
    printf("%s returned: %ld\n",function,err);
    printf("Image quality : [%ld]\n",quality);

    ///////////////////////////////////////////////
    // ComputeNFIQ()
    strcpy(function,"ComputeNFIQ()");
    printf("\nCall %s\n",function);
    nfiq = sgfplib->ComputeNFIQ(imageBuffer1, deviceInfo.ImageWidth, deviceInfo.ImageHeight);
    printf("NFIQ : [%ld]\n",nfiq);
    }while(nfiq < 80 && quality < 80);  
}

void close_reader()
{
    ///////////////////////////////////////////////
    // closeDevice()
    printf("\nCall CloseDevice()\n");
    err = sgfplib->CloseDevice();
    printf("CloseDevice returned : [%ld]\n",err);

    ///////////////////////////////////////////////
    // Destroy SGFPLib object
    strcpy(function,"DestroySGFPMObject()");
    printf("\nCall %s\n",function);
    err = DestroySGFPMObject(sgfplib);
    printf("%s returned: %ld\n",function,err);
}

bool match_finger(BYTE *templateBuffer1, BYTE *templateBuffer2, DWORD *score) //descriptografa as templates e compara
{
    fingerID = 0;

    //carrega a chave
    fp = fopen("keys/key.txt", "r+");
    if (fp == NULL)
    {
        printf("\nFalha na carga da chave");
    }
    fgets((char*)key, sizeof(key), fp);
    fclose(fp);

    printf("\n Print do key dentro do MATCH template");
    printf_ByteArray(key, sizeof(key));
    printf("\n Print do nonce dentro do MATCH template");
    printf_ByteArray(nonce, sizeof(nonce));


    // SetTemplateFormat(TEMPLATE_FORMAT_ISO19794)
    strcpy(function,"SetTemplateFormat(TEMPLATE_FORMAT_ISO19794)");
    printf("\nCall %s\n",function);
    err = sgfplib->SetTemplateFormat(TEMPLATE_FORMAT_ISO19794);
    printf("%s returned: %ld\n",function,err);

    

    ///////////////////////////////////////////////
    // MatchTemplate()
    while (matched != true)
    {
        //randombytes_buf(nonce, sizeof(nonce)); //cria nonce para transação
        printf("\nRealizando match na template: digital_%i.iso",fingerID);
        sprintf(kbBuffer,"fingerData/digital_%i.iso",fingerID);
        fp = fopen(kbBuffer, "r+");
        if (fp == NULL) 
            {   
                printf ("Sem mais templates, nenhum match realizado.\n");
                break;
            }
        
        err = fread(ciphertext, sizeof(ciphertext), 1, fp);
        printf("\n Print do ciphertext dentro do MATCH template:");
        printf_ByteArray(ciphertext, sizeof(ciphertext));
        printf("\nValor do fread = %d", (int) err);
        if (crypto_secretbox_open_easy(decrypted, ciphertext, CIPHERTEXT_LEN, nonce, key) == -1)
        {
            printf("\nErro na  remocao da criptografia\n");
        }

        fclose(fp);
        err = sgfplib->MatchIsoTemplate(templateBuffer1, 0, decrypted, 0, SL_NORMAL, &matched);
        printf("%s returned: %ld\n",function,err);
        if (matched == true)
        printf("<<MATCH>>\n");
        else
        printf("<<NO MATCH>>\n");
    }
    ///////////////////////////////////////////////
    // GetIsoMatchingScore()
    /*strcpy(function,"GetIsoMatchingScore(ISO1,ISO2)");
    printf("\nCall %s\n",function);
    err = sgfplib->GetIsoMatchingScore(templateBuffer1, 0, templateBuffer2, 0, score);
    printf("%s returned: %ld\n",function,err);
    printf("Score is : [%ld]\n",*score);*/

    return matched; 
}

void create_template(BYTE *imageBuffer1, BYTE *templateBuffer1)  //criptografa a template e armazena
{

    //carrega a chave
    fp = fopen("keys/key.txt", "r+");
    if (fp == NULL)
    {
        printf("\nFalha na carga da chave");
    }
    fgets((char*)key, sizeof(key), fp);
    fclose(fp);

    ///////////////////////////////////////////////
    // SetTemplateFormat(TEMPLATE_FORMAT_ISO19794)
    strcpy(function,"SetTemplateFormat(TEMPLATE_FORMAT_ISO19794)");
    printf("\nCall %s\n",function);
    err = sgfplib->SetTemplateFormat(TEMPLATE_FORMAT_ISO19794);
    printf("%s returned: %ld\n",function,err);  

    ///////////////////////////////////////////////
    // getMaxTemplateSize()
    strcpy(function,"GetMaxTemplateSize()");
    printf("\nCall %s\n",function);
    err = sgfplib->GetMaxTemplateSize(&templateSizeMax);
    printf("%s returned: %ld\n",function,err);
    printf("Max Template Size : [%ld]\n",templateSizeMax);

    ///////////////////////////////////////////////
    // getMinutiae()
    strcpy(function,"CreateTemplate()");
    printf("\nCall %s\n",function);
    //templateBuffer1 = (BYTE*) malloc(sizeof(templateSizeMax)); - removido pois quebra o ponteiro
    fingerInfo.FingerNumber = SG_FINGPOS_UK;
    fingerInfo.ViewNumber = 0;
    fingerInfo.ImpressionType = SG_IMPTYPE_LP;
    fingerInfo.ImageQuality = getPIVQuality(quality); //0 to 100
    printf("Valor quality %d\n", fingerInfo.ImageQuality);
    err = sgfplib->CreateTemplate(&fingerInfo, imageBuffer1, templateBuffer1);
    printf("CreateTemplate returned : [%ld]\n",err);
    randombytes_buf(nonce, sizeof(nonce)); //cria nonce para transação
    crypto_secretbox_easy(ciphertext, templateBuffer1, MESSAGE_LEN, nonce, key);//criptografa a template

    if (err == SGFDX_ERROR_NONE)
    {
        ///////////////////////////////////////////////
        // getTemplateSize()
        strcpy(function,"GetTemplateSize()");
        printf("\nCall %s\n",function);
        err = sgfplib->GetTemplateSize(templateBuffer1, &templateSize);
        printf("%s returned: %ld\n",function,err);
        printf("Template Size : [%ld]\n",templateSize);
        sprintf(kbBuffer,"fingerData/digital_%i.iso",fingerID);
        fp = fopen(kbBuffer,"wb");
        fwrite (ciphertext , sizeof (ciphertext) , 1 , fp); //armazena template criptografada
        fclose(fp);
        fingerID++;

        strcpy(function,"GetNumOfMinutiae()");
        printf("\nCall %s\n",function);
        err = sgfplib->GetNumOfMinutiae(TEMPLATE_FORMAT_ISO19794, templateBuffer1, &numOfMinutiae);
        printf("%s returned: %ld\n",function,err);
        printf("Minutiae Count : [%ld]\n",numOfMinutiae);
    }   
    printf("\n tamanho do ciphertext: %d", sizeof(ciphertext));
    printf("\n tamanho do templatebuffer: %d", (int) templateSize);
    printf("\n Print do templatebuffer dentro do create template:");
    printf_ByteArray(templateBuffer1, (int) templateSize);
    printf("\n Print do ciphertext dentro do create template:");
    printf_ByteArray(ciphertext, sizeof(ciphertext));
    printf("\n Print do key dentro do create template");
    printf_ByteArray(key, sizeof(key));
    printf("\n Print do nonce dentro do create template");
    printf_ByteArray(nonce, sizeof(nonce));
}

