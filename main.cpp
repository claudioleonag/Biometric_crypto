/* Lib Includes */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <sqlite3.h>
#include <pthread.h>
#include <sys/inotify.h>
#include <fcntl.h>

/* Header Files */
#include "include/OLED_CONTROLER.h"
#include "include/bio_reader.h"
#include "include/sodium.h"

//file Path definition

#define keyPath "keys/key"
#define gpioPath "/sys/class/gpio/gpio60/value"

typedef unsigned char BYTE;
typedef unsigned long DWORD;

void *threadCrypto(void *key);

int main()
{
  bool matched;
  DWORD *score;
  BYTE *templateBuffer1;
  templateBuffer1 = (BYTE *)malloc(800); // area of the reader x * y
  BYTE *templateBuffer2;
  templateBuffer2 = (BYTE *)malloc(800);
  BYTE *imageBuffer;
  imageBuffer = (BYTE *)malloc(400 * 800);
  printf("\n-------------------------------------\n");

  ///////////////////////q
  printf("\n-------------------------------------\n");
  printf("TCC2 - Claudio Leon\n");
  printf("-------------------------------------\n");
  printf("Iniciando bloco criptografico...\n");

  ///// CRIPTO ////
  if (sodium_init() == -1)
  {
    printf("\nFalha na inicialização da biblioteca de criptografia");
  }
  else
  {
    printf("\nBiblioteca de criptografia inicializada");
  }

  unsigned char key[crypto_secretbox_KEYBYTES];
  unsigned char nonce[crypto_secretbox_NONCEBYTES];

  FILE *fp = fopen(keyPath, "rb");
  if (fp == NULL)
  {
    printf("\n--- VIOLATED READER - STOP ---\n");
    //printf("\nChave não criada, criando nova chave criptografica...\n");
    //crypto_secretbox_keygen(key); // cria nova chave
    //fp = fopen("keys/key", "wb");
    //fwrite(key, sizeof key[0], crypto_secretbox_KEYBYTES, fp);
    //fclose(fp);
    exit(1);
  }
  else
  {
    printf("\nChave criptografica encontrada.\n");
    fread(key, sizeof key[0], crypto_secretbox_KEYBYTES, fp);
    fclose(fp);
  }

  fp = fopen("keys/nonce", "rb");
  if (fp == NULL)
  {
    printf("\nNonce não criada, criando novo valor...");
    randombytes_buf(nonce, sizeof nonce); // cria nonce
    fp = fopen("keys/nonce", "wb");
    fwrite(nonce, sizeof nonce[0], crypto_secretbox_NONCEBYTES, fp);
    fclose(fp);
  }
  else
  {
    printf("\nNonce criptografica encontrada.");
    fgets((char *)nonce, sizeof(nonce), fp);
    fread(nonce, sizeof nonce[0], crypto_secretbox_NONCEBYTES, fp);
    fclose(fp);
  }

  // init_oled();
  // write_oled("TCC2",1,0,3);
  // write_oled("Claudio Leon",1,30,1); //(x, y, tamanho fonte)
  printf("\n-------------------------------------\n");
  printf("Iniciando leito biometrico...\n");

  if (errno = open_reader()) // inicia o leitor
  {
    printf("Failed to open biometric reader: %s.\n", strerror(errno));
    exit(1);
  }
  else
  {
    printf("\nSucesso em abrir o leitor\n");
    printf("\n-------------------------------------\n");
    // write_oled("Leitor iniciado",1,0,1);
  }
  // THREAD
  pthread_t thread_id;
  printf("Before Thread\n");
  pthread_create(&thread_id, NULL, threadCrypto, key);
  
  // menu implementation
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

    switch (command)
    {

    case 'r':

      errno = read_finger(imageBuffer);
      if (errno != 0)
      {
        printf("Error reading fingerprint: %s.\n", strerror(errno));
        return (-1);
      }
      errno =
          create_template(imageBuffer, templateBuffer1, key, nonce, true);
      if (errno != 0)
      {
        printf("Error creating template error: %s.\n",
               strerror(errno));
        return (-1);
      }
      // write_oled("Register Sucess",1,0,1);
      break;

    case 'm':
      read_finger(imageBuffer);
      create_template(imageBuffer, templateBuffer1, key, nonce, false); // so desejo o buffer para comparar
      matched = match_finger(templateBuffer1, score, key, nonce);
      break;

    case 'c':
      close_reader();
      free(imageBuffer);
      free(templateBuffer1);
      free(templateBuffer2);
      pthread_exit(NULL);
      printf("\nThanks!!");
      return 0;
    }
  }
  return 0;
}

void *threadCrypto(void *key)
{
  char GPIOStatus[10];
  printf("Watching : %s\n",gpioPath);
  while(1)
  {
    FILE *fp = fopen("/sys/class/gpio/gpio60/value", "rb");
    if (fp == NULL)
    {
      printf("\nFalha na leitura do GPIO...\n");
    }
    fread(GPIOStatus, sizeof(int), 1, fp);
    char *buf;
    long status = strtol(GPIOStatus,&buf, 10);
    if (status == 1)
    {
      printf("--- TAMPER-PROOF DETECTED TERMINATING CRYPTOGRAPH KEY AND CLOSING PROGRAM ---\n");
      system("rm -f /home/debian/Biometric_crypto/keys/key");
      exit(1);
    }
    fclose(fp);
    sleep(0.1);
  }
}