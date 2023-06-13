
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <sqlite3.h>
#include "../include/bio_reader.h"
#include "../include/sgfplib.h"
#include "../include/sodium.h"

int getPIVQuality(int quality)
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

// variaveis cripto

#define MESSAGE_LEN 800
#define CIPHERTEXT_LEN (crypto_secretbox_MACBYTES + MESSAGE_LEN)

unsigned char ciphertext[CIPHERTEXT_LEN];
unsigned char decrypted[MESSAGE_LEN];

// variaveis leitor biometrico
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

// sqlite database variables
sqlite3 *db;
char *zErrMsg = 0;
char sql[800]; // size of a template
int open_reader()
{
  system ("export CRYPTO_KEY_PATH=/gome/debian/Biometric_crypto/keys");
  // initiate database
  printf("\n-------------------------------------\n");
  printf("Starting database...\n");

  errno = sqlite3_open("template_storage.db", &db);
  if (errno)
  {
    printf("Can't open database: %s\n", sqlite3_errmsg(db));
    return (0);
  }
  else
  {
    printf("Opened database successfully. \n");
  }
  char sql[800];
  sprintf(sql,
          "CREATE TABLE TEMPLATE([ID] INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, [TEMPLATE] BLOB);");
  errno = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
  if (errno != SQLITE_OK)
  {
    printf("\nDatabase already exists ...");
  }
  else
  {
    printf("Database created: %s ", strerror(errno));
  }

  ///////////////////////////////////////////////
  // Instantiate SGFPLib object

  strcpy(function, "CreateSGFPMObject()");
  printf("\nCall %s\n", function);
  errno = CreateSGFPMObject(&sgfplib);
  if (!sgfplib)
  {
    printf("ERROR - Unable to instantiate FPM object.\n\n");
    return -1;
  }
  printf("%s returned: %d\n", function, errno);

  ///////////////////////////////////////////////
  // Initiate device
  strcpy(function, "Init(SG_DEV_AUTO)");
  printf("\nCall %s\n", function);
  errno = sgfplib->Init(SG_DEV_AUTO);
  printf("%s returned: %d\n", function, errno);

  if (errno != SGFDX_ERROR_NONE)
  {
    printf("ERROR - Unable to initialize device.\n\n");
    return -1;
  }

  ///////////////////////////////////////////////
  // OpenDevice()
  strcpy(function, "OpenDevice(0)");
  printf("\nCall %s\n", function);
  errno = sgfplib->OpenDevice(0);
  printf("%s returned: %d\n", function, errno);

  if (errno == SGFDX_ERROR_NONE)
  {

    ///////////////////////////////////////////////
    // setBrightness()
    strcpy(function, "SetBrightness()");
    printf("\nCall %s\n", function);
    errno = sgfplib->SetBrightness(30);
    printf("%s returned: %d\n", function, errno);

    ///////////////////////////////////////////////
    // setLedOn(true) - Startup sequence
    strcpy(function, "SetLedOn(true)");
    printf("\nCall %s\n", function);
    errno = sgfplib->SetLedOn(true);

    ///////////////////////////////////////////////
    // getDeviceInfo()
    deviceInfo.DeviceID = 0;
    strcpy(function, "GetDeviceInfo()");
    printf("\nCall %s\n", function);
    errno = sgfplib->GetDeviceInfo(&deviceInfo);
    printf("%s returned: %d\n", function, errno);
    if (errno == SGFDX_ERROR_NONE)
    {
      printf("\tdeviceInfo.DeviceID   : %ld\n", deviceInfo.DeviceID);
      printf("\tdeviceInfo.DeviceSN   : %s\n", deviceInfo.DeviceSN);
      printf("\tdeviceInfo.ComPort    : %ld\n", deviceInfo.ComPort);
      printf("\tdeviceInfo.ComSpeed   : %ld\n", deviceInfo.ComSpeed);
      printf("\tdeviceInfo.ImageWidth : %ld\n", deviceInfo.ImageWidth);
      printf("\tdeviceInfo.ImageHeight: %ld\n", deviceInfo.ImageHeight);
      printf("\tdeviceInfo.Contrast   : %ld\n", deviceInfo.Contrast);
      printf("\tdeviceInfo.Brightness : %ld\n", deviceInfo.Brightness);
      printf("\tdeviceInfo.Gain       : %ld\n", deviceInfo.Gain);
      printf("\tdeviceInfo.ImageDPI   : %ld\n", deviceInfo.ImageDPI);
      printf("\tdeviceInfo.FWVersion  : %04X\n",
             (unsigned int)deviceInfo.FWVersion);
    }
    // SetTemplateFormat(TEMPLATE_FORMAT_ISO19794)
    strcpy(function, "SetTemplateFormat(TEMPLATE_FORMAT_ISO19794)");
    printf("\nCall %s\n", function);
    errno = sgfplib->SetTemplateFormat(TEMPLATE_FORMAT_ISO19794);
    printf("%s returned: %d\n", function, errno);
  }

  return 0;
}

int read_finger(BYTE *imageBuffer1)
{

  do
  { ///////////////////////////////////////////////
    // getImage() - 1st Capture
    strcpy(function, "GetImage()");
    printf("\nCall %s\n", function);
    errno = sgfplib->GetImage(imageBuffer1);
    printf("%s returned: %d\n", function, errno);
    if (errno != 0)
    {
      printf("Failed to capture image: %s.\n", strerror(errno));
      //exit(1);
    }

    ///////////////////////////////////////////////
    // getImageQuality()
    strcpy(function, "GetImageQuality()");
    printf("\nCall %s\n", function);
    errno =
        sgfplib->GetImageQuality(deviceInfo.ImageWidth,
                                 deviceInfo.ImageHeight, imageBuffer1,
                                 &quality);
    printf("%s returned: %d\n", function, errno);
    printf("Image quality : [%ld]\n", quality);

    ///////////////////////////////////////////////
    // ComputeNFIQ()
    strcpy(function, "ComputeNFIQ()");
    printf("\nCall %s\n", function);
    nfiq =
        sgfplib->ComputeNFIQ(imageBuffer1, deviceInfo.ImageWidth,
                             deviceInfo.ImageHeight);
    printf("NFIQ : [%ld]\n", nfiq);
  } while (quality < 60);
  return 0;
}

int create_template(BYTE *imageBuffer1, BYTE *templateBuffer1, unsigned char *key, unsigned char *nonce, bool storage) // criptografa a template e armazena
{
  ///////////////////////////////////////////////
  // SetTemplateFormat(TEMPLATE_FORMAT_ISO19794)
  strcpy(function, "SetTemplateFormat(TEMPLATE_FORMAT_ISO19794)");
  printf("\nCall %s\n", function);
  errno = sgfplib->SetTemplateFormat(TEMPLATE_FORMAT_ISO19794);
  printf("%s returned: %d\n", function, errno);

  ///////////////////////////////////////////////
  // getMaxTemplateSize()
  strcpy(function, "GetMaxTemplateSize()");
  printf("\nCall %s\n", function);
  errno = sgfplib->GetMaxTemplateSize(&templateSizeMax);
  printf("%s returned: %d\n", function, errno);
  printf("Max Template Size : [%ld]\n", templateSizeMax);

  ///////////////////////////////////////////////
  // getMinutiae()
  strcpy(function, "CreateTemplate()");
  printf("\nCall %s\n", function);
  // templateBuffer1 = (BYTE*) malloc(sizeof(templateSizeMax)); - removido pois quebra o ponteiro
  fingerInfo.FingerNumber = SG_FINGPOS_UK;
  fingerInfo.ViewNumber = 0;
  fingerInfo.ImpressionType = SG_IMPTYPE_LP;
  fingerInfo.ImageQuality = getPIVQuality(quality); // 0 to 100
  printf("Valor quality %d\n", fingerInfo.ImageQuality);
  errno = sgfplib->CreateTemplate(&fingerInfo, imageBuffer1, templateBuffer1); // cria a template
  printf("CreateTemplate returned : [%d]\n", errno);
  if (errno != 0)
  {
    printf("Error creating template error: %s.\n", strerror(errno));
    return (-1);
  }

  if (errno == SGFDX_ERROR_NONE)
  {
    ///////////////////////////////////////////////
    // getTemplateSize()
    strcpy(function, "GetTemplateSize()");
    printf("\nCall %s\n", function);
    errno = sgfplib->GetTemplateSize(templateBuffer1, &templateSize);
    printf("%s returned: %d\n", function, errno);
    printf("Template Size : [%ld]\n", templateSize);

    // contagem das minucias
    strcpy(function, "GetNumOfMinutiae()");
    printf("\nCall %s\n", function);
    errno =
        sgfplib->GetNumOfMinutiae(TEMPLATE_FORMAT_ISO19794, templateBuffer1,
                                  &numOfMinutiae);
    printf("%s returned: %d\n", function, errno);
    printf("Minutiae Count : [%ld]\n", numOfMinutiae);
  }
  if (storage == true)
  {
    // criptografa e armazena a template
    strcpy(function, "crypto()");
    printf("\nCall %s\n", function);
    if (crypto_secretbox_easy(ciphertext, templateBuffer1, MESSAGE_LEN, nonce, key) == -1) // criptografa a template
    {
      printf("\nErro na criptografia da template .iso");
    }
    else
    {
      printf("\nCriptografado template .iso");
    }
    // store on inside databa se
    sqlite3_stmt *pStmt;
    sprintf(sql, "INSERT INTO TEMPLATE (TEMPLATE) VALUES(?);");
    errno = sqlite3_prepare(db, sql, -1, &pStmt, 0);
    if (errno != SQLITE_OK)
    {
      printf("\nFailed to prepare storage: %s ", strerror(errno));
      printf("\n errno: %d", errno);
    }
    sqlite3_bind_blob(pStmt, 1, ciphertext, CIPHERTEXT_LEN, SQLITE_STATIC);
    errno = sqlite3_step(pStmt);
    if (errno != SQLITE_DONE)
    {
      printf("Template storage failed: %s", sqlite3_errmsg(db));
    }
    else
    {
      printf("\nTemplate stored sucessfully on database.");
    }
    sqlite3_finalize(pStmt);

    sprintf(kbBuffer, "fingerData/digital_%i.iso", fingerID);
    fp = fopen(kbBuffer, "wb");
    if (fp == NULL)
    {
      printf("\nErro no armazenamento da template criptografada");
    }
    fwrite(ciphertext, sizeof ciphertext[0], CIPHERTEXT_LEN, fp);
    printf("\nArmazenado com sucesso template ISO. Path:fingerData/digital_%i.iso",
           fingerID);
    fclose(fp);
    fingerID++;
  }
  *imageBuffer1 = 0;
  return 0;
}
bool match_finger(BYTE *templateBuffer1, DWORD *score, unsigned char *key, unsigned char *nonce) // descriptografa as templates e compara
{
  
  matched = false;
  fingerID = 1;
  ///////////////////////////////////////////////
  // MatchTemplate()
  while (matched != true)
  {
    // database block
    sprintf(sql, "SELECT TEMPLATE FROM TEMPLATE WHERE ID = %d;", fingerID);
    sqlite3_stmt *pStmt;
    errno = sqlite3_prepare_v2(db, sql, -1, &pStmt, 0);
    if (errno != SQLITE_OK)
    {
      printf("Failed to prepare statement: %s \n", sqlite3_errmsg(db));
      return 1;
    }
    errno = sqlite3_step(pStmt); // realizo a busca na base de dados
    if (errno == SQLITE_DONE)
    {
      printf("\nNo more templates.: %s", sqlite3_errmsg(db));
      break;
    }

     // copia a qtade de bytes na coluna

    // randombytes_buf(nonce, sizeof(nonce)); //cria nonce para transação
    /*
    sprintf(kbBuffer, "fingerData/digital_%i.iso", fingerID);
    fp = fopen(kbBuffer, "rb");
    if (fp == NULL)
    {
      printf("\nSem mais templates, nenhum match realizado.\n");
      break;
    }

    errno = fread(ciphertext, sizeof ciphertext[0], CIPHERTEXT_LEN, fp);
    // printf("\n Print do ciphertext dentro do MATCH template:");
    // printf_ByteArray(ciphertext, CIPHERTEXT_LEN);
    // printf("\nValor do fread = %d", (int) errno);
    fclose(fp);*/
    unsigned char* dbMessage = (unsigned char*)sqlite3_column_blob(pStmt, 0);
    
    // remove criptografia
    if (crypto_secretbox_open_easy(decrypted, dbMessage, CIPHERTEXT_LEN, nonce, key) == -1)
    {
      printf("\nErro na  remocao da criptografia\n");
    }

    errno =
        sgfplib->MatchIsoTemplate(templateBuffer1, 0, decrypted, 0,
                                  SL_NORMAL, &matched);
    printf("\nRealizando match na template: digital_%i.iso: %s", fingerID,
           matched ? "MATCH" : "NO MATCH");
    fingerID++;
  }
  ///////////////////////////////////////////////
  // GetIsoMatchingScore()
  /*strcpy(function,"GetIsoMatchingScore(ISO1,ISO2)");
     printf("\nCall %s\n",function);
     errno = sgfplib->GetIsoMatchingScore(templateBuffer1, 0, templateBuffer2, 0, score);
     printf("%s returned: %d\n",function,errno);
     printf("Score is : [%d]\n",*score); */

  return matched;
}

void close_reader()
{
  ///////////////////////////////////////////////
  // closeDevice()
  printf("\nCall CloseDevice()\n");
  errno = sgfplib->CloseDevice();
  printf("CloseDevice returned : [%d]\n", errno);

  ///////////////////////////////////////////////
  // Destroy SGFPLib object
  strcpy(function, "DestroySGFPMObject()");
  printf("\nCall %s\n", function);
  errno = DestroySGFPMObject(sgfplib);
  printf("%s returned: %d\n", function, errno);

  sqlite3_close(db);
}

