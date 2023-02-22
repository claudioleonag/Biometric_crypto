#Diretorios de Include

INC= $(HOME)/Biometric_crypto/include/
LIB= $(HOME)/Biometric_crypto/libs/
#b OLED_LIB = $(HOME)/Biometric_crypto/libs/liboled.so

SOURCES = main.c source/SSD1306_OLED.c source/example_app.c source/I2C.c source/OLED_CONTROLER.c
#SOURCERS_OLED = source/SSD1306_OLED.c source/example_app.c source/I2C.c source/OLED_CONTROLER.c
OBJS := $(SOURCES:.c=.o)
CFLAGS := -g

OBJS := $(SOURCES:.c=.o)
CFLAGS = -fPIC -Wall -Wextra -O2 -g
LDFLAGS = -shared   # linking flags
CC = gcc  # C compiler

ssd: $(OBJS)
	$(CC) -I $(INC) $^ -o $@ 

clean:
	rm -rf ssd $(OBJS)
