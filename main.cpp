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
#include "include/sodium.h"

typedef unsigned char BYTE;
typedef unsigned long DWORD;

#define MESSAGE ((const unsigned char *) "test")
#define MESSAGE_LEN 4
#define CIPHERTEXT_LEN (crypto_secretbox_MACBYTES + MESSAGE_LEN)

int printf_ByteArray2(const unsigned char *data, size_t len) {
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
    printf(  "Iniciando bloco criptografico...\n");

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
    unsigned char ciphertext[CIPHERTEXT_LEN];
    unsigned char decrypted[MESSAGE_LEN];

    FILE *fp = fopen("keys/key", "rb");
    if (fp == NULL){
        printf("\nChave não criada, criando nova chave criptografica...\n");
        crypto_secretbox_keygen(key); //cria nova chave
        printf_ByteArray2(key, crypto_secretbox_KEYBYTES);
        fp = fopen("keys/key", "wb");
        fwrite(key, sizeof key[0], crypto_secretbox_KEYBYTES, fp);
        fclose(fp); 
        }
    else{
        printf("\nChave criptografica encontrada.\n");
        fread(key, sizeof key[0], crypto_secretbox_KEYBYTES, fp);
        printf_ByteArray2(key, crypto_secretbox_KEYBYTES);
        fclose(fp);       
        }


    /*fp = fopen("keys/nonce.txt", "r+");
    if (fp == NULL){
        printf("\nNonce não criada, criando novo valor...");
        randombytes_buf(nonce, sizeof nonce);  //cria nonce
        fp = fopen("keys/nonce.txt", "w");
        fwrite (nonce , sizeof (nonce) , 1 , fp);

    }
       else{
        printf("\nNonce criptografica encontrada.");
        fgets((char*)nonce, sizeof(nonce), fp);
        fclose(fp);
        }*/
    printf("\n");
    //printf_ByteArray(MESSAGE, MESSAGE_LEN);
    crypto_secretbox_easy(ciphertext, MESSAGE, MESSAGE_LEN, nonce, key);
    printf("\n");
    //printf_ByteArray(ciphertext, MESSAGE_LEN);


    crypto_secretbox_open_easy(decrypted, ciphertext, CIPHERTEXT_LEN, nonce, key);
    printf("\n");
    //printf_ByteArray(decrypted, MESSAGE_LEN);

     //cria nounce

    /*
    fp = fopen("keys/key","wb");
    fwrite (key , 1 , sizeof(key) , fp);
    fclose(fp);
    fp = fopen("keys/nonce","wb");
    fwrite (nonce , 1 , sizeof(key) , fp);
    fclose(fp);




    randombytes_buf(nonce, sizeof nonce);
    crypto_secretbox_easy(ciphertext, MESSAGE, MESSAGE_LEN, nonce, key);
    printf("\n");
    printf_ByteArray(ciphertext, MESSAGE_LEN);

    unsigned char decrypted[MESSAGE_LEN];
    crypto_secretbox_open_easy(decrypted, ciphertext, CIPHERTEXT_LEN, nonce, key);
    int i;
    printf("\n");
    printf_ByteArray(decrypted, MESSAGE_LEN);

    crypto_secretbox_easy(ciphertext, MESSAGE, MESSAGE_LEN, nonce, key);
    printf("\n");
    printf_ByteArray(ciphertext, MESSAGE_LEN);

    crypto_secretbox_open_easy(decrypted, ciphertext, CIPHERTEXT_LEN, nonce, key);
    printf("\n");
    printf_ByteArray(decrypted, MESSAGE_LEN);
  */

   ///////// END CRIPTO /////////



    //init_oled();
    //write_oled("TCC2",1,0,3);
    //write_oled("Claudio Leon",1,30,1); //(x, y, tamanho fonte)
    printf(  "\n-------------------------------------\n");
    printf(  "Iniciando leito biometrico...\n");

    if (err = open_reader()) // inicia o leitor
    {
        printf("Falha em abrir o leitor, fechando o programa.\n");
        exit(1);
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
