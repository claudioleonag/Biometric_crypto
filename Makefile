#Diretorios de Include

INC= $(HOME)/Biometric_crypto/include/
LIB= $(HOME)/Biometric_crypto/libs/
#b OLED_LIB = $(HOME)/Biometric_crypto/libs/liboled.so

SOURCES = main.c source/SSD1306_OLED.c source/example_app.c source/I2C.c source/OLED_CONTROLER.c source/bio_reader.cpp
#SOURCERS_OLED = source/SSD1306_OLED.c source/example_app.c source/I2C.c source/OLED_CONTROLER.c
OBJS := $(SOURCES:.c=.o)
CFLAGS := -g

OBJS := $(SOURCES:.c=.o)
CFLAGS = -fPIC -Wall -Wextra -O2 -g
LDFLAGS = -shared   # linking flags
CC = g++  # C compiler

ssd: $(OBJS)
	$(CC) -I$(INC) -L$(LIB) $^ -o $@ -lsgfplib -lsgnfiq -lsgimage -lsgfpamx -lsgfdu05 -lusb -lpthread -lsodium -lsqlite3 -Wall -Werror

clean:
	rm -rf ssd $(OBJS)
