#ifndef BIO_READER_H_
#define BIO_READER_H_

typedef unsigned char BYTE;
typedef unsigned long DWORD;

int open_reader();
void read_finger(BYTE *imageBuffer1);
void create_template(BYTE *imageBuffer1, BYTE *templateBuffer1);
bool match_finger(BYTE *templateBuffer1, BYTE *templateBuffer2, DWORD *score);
void close_reader();

#endif 