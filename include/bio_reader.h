#ifndef BIO_READER_H_
#define BIO_READER_H_

typedef unsigned char BYTE;
typedef unsigned long DWORD;

int open_reader();
void read_finger(BYTE *imageBuffer1);
int create_template(BYTE *imageBuffer1, BYTE *templateBuffer1, unsigned char *key, unsigned char *nonce, bool storage);
bool match_finger(BYTE *templateBuffer1, DWORD *score,  unsigned char *key, unsigned char *nonce);
void close_reader();

#endif 