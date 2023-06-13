#Diretorios de Include

INC= $(HOME)/Biometric_crypto/include/

SOURCES = main.cpp source/SSD1306_OLED.c source/example_app.c source/I2C.c source/OLED_CONTROLER.c source/bio_reader.cpp
#SOURCERS_OLED = source/SSD1306_OLED.c source/example_app.c source/I2C.c source/OLED_CONTROLER.c
OBJS := $(SOURCES:.c=.o)

OBJS := $(SOURCES:.c=.o)
ccflags-y = -g
LDFLAGS = -shared   # linking flags
COMP = g++  # C compiler

ssd: 
	$(COMP) -I$(INC) $(SOURCES) $^ -o $@ -lsgfplib -lsgnfiq -lsgimage -lsgfpamx -lsgfdu05 -lusb -lpthread -lsodium -lsqlite3
