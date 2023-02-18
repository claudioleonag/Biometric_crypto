#Diretorios de Include

INC_DIR= ../I2C_Library"
INC_DIR= ../SSD1306_OLED_Library"
INC_DIR= ../OLED_API"
INC_DIR= ../OLED_CONTROLER"

SOURCES = main.c SSD1306_OLED_Library/SSD1306_OLED.c OLED_API/example_app.c I2C_Library/I2C.c OLED_CONTROLER/OLED_CONTROLER.c
OBJS := $(SOURCES:.c=.o)
CPPFLAGS := -I SSD1306_OLED_Library -I I2C_Library
CFLAGS := -g

ssd: $(OBJS)
	$(CC) $^ -o $@

clean:
	rm -rf ssd $(OBJS)
