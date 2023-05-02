#Diretorios de Include

INC= $(HOME)/Biometric_crypto/include/
LIB= $(HOME)/Biometric_crypto/libs/
#b OLED_LIB = $(HOME)/Biometric_crypto/libs/liboled.so

SOURCES = main.c source/SSD1306_OLED.c source/example_app.c source/I2C.c source/OLED_CONTROLER.c source/bio_reader.cpp
#SOURCERS_OLED = source/SSD1306_OLED.c source/example_app.c source/I2C.c source/OLED_CONTROLER.c
OBJS := $(SOURCES:.c=.o)

OBJS := $(SOURCES:.c=.o)
ccflags-y = -g
LDFLAGS = -shared   # linking flags
COMP = g++  # C compiler

ssd: $(OBJS)
	$(COMP) -I$(INC) -L$(LIB) $^ -o $@ -lsgfplib -lsgnfiq -lsgimage -lsgfpamx -lsgfdu05 -lusb -lpthread -lsodium -lsqlite3
