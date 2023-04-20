/* Lib Includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/* Header Files */
#include "../include/I2C.h"
#include "../include/SSD1306_OLED.h"
#include "../include/example_app.h"

/* Oh Compiler-Please leave me as is */
volatile unsigned char flag = 0;

/* Alarm Signal Handler */
void
ALARMhandler (int sig)
{
  /* Set flag */
  flag = 5;
}

void
init_oled (void)
{
  //enable ports 21,22 as I2C
  if (system ("config-pin p9.21 i2c") == 0)
    {
      printf ("Pin 21 configured as I2C sucessfully\r\n");
    }
  else
    {
      printf ("Failed to configure pin 21\r\n");
      exit (1);
    }

  if (system ("config-pin p9.22 i2c") == 0)
    {
      printf ("Pin 21 configured as I2C sucessfully\r\n");
    }
  else
    {
      printf ("Failed to configure pin 22\r\n");
    }


  /* Initialize I2C bus and connect to the I2C Device */
  if (init_i2c_dev (I2C_DEV2_PATH, SSD1306_OLED_ADDR) == 0)
    {
      printf ("(Main)i2c-2: Bus Connected to SSD1306\r\n");
    }
  else
    {
      printf ("(Main)i2c-2: Something Went Wrong\r\n");
      exit (1);
    }

  /* Register the Alarm Handler */
  signal (SIGALRM, ALARMhandler);

  /* Run SDD1306 Initialization Sequence */
  display_Init_seq ();

  /* Clear display */
  clearDisplay ();
  usleep (1000);

  printf ("OLED initialized\n");

  // Fill the round rectangle
  testfillroundrect ();
  usleep (1000000);
  clearDisplay ();
}

void
write_oled (char *message, int x, int y, int size)
{
  clearDisplay ();
  setTextSize (size);
  setTextColor (WHITE);
  setCursor (x, y);
  print_strln (message);
  println ();
  Display ();
}
