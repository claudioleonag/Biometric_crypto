#ifndef BIO_READER_H_
#define BIO_READER_H_

typedef unsigned char BYTE;

int open_reader();
void read_finger(BYTE *imageBuffer1);
void create_template(BYTE *imageBuffer1, BYTE *templateBuffer1);
void match_finger(BYTE *templateBuffer1, BYTE *templateBuffer2);
void close_reader();

#endif 