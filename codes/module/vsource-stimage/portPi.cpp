#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include "portPi.h"

#define PI_GPIO_MASK  (0xFFFFFFC0)
#define	PAGE_SIZE		(4*1024)
#define	BLOCK_SIZE		(4*1024)

static volatile uint32_t *gpio ;
static int piModel2 = false ;
static int *pinToGpio ;
static int *physToGpio ;

static volatile unsigned int RASPBERRY_PI_PERI_BASE ;
static volatile unsigned int GPIO_PADS ;
static volatile unsigned int GPIO_CLOCK_BASE ;
static volatile unsigned int GPIO_BASE ;
static volatile unsigned int GPIO_TIMER ;
static volatile unsigned int GPIO_PWM ;
static int wiringPiMode = WPI_MODE_UNINITIALISED ;



int PiSetup (void)
{
  int   fd ;
  int   boardRev ;
  int   model, rev, mem, maker, overVolted ;
  static int alreadyCalled = false ;

  boardRev = 2 ;
  piModel2 = true;

  if (boardRev == 1) // A, B, Rev 1, 1.1
  {
     /*pinToGpio =  pinToGpioR1 ;
    physToGpio = physToGpioR1 ;*/
  }
  else        // A, B, Rev 2, B+, CM, Pi2, Zero
  {
     pinToGpio =  pinToGpioR2 ;
    physToGpio = physToGpioR2 ;
  }


  if (piModel2)
    RASPBERRY_PI_PERI_BASE = 0x3F000000 ;
  else
    RASPBERRY_PI_PERI_BASE = 0x20000000 ;


  GPIO_PADS     = RASPBERRY_PI_PERI_BASE + 0x00100000 ;
  GPIO_CLOCK_BASE = RASPBERRY_PI_PERI_BASE + 0x00101000 ;
  GPIO_BASE   = RASPBERRY_PI_PERI_BASE + 0x00200000 ;
  GPIO_TIMER    = RASPBERRY_PI_PERI_BASE + 0x0000B000 ;
  GPIO_PWM    = RASPBERRY_PI_PERI_BASE + 0x0020C000 ;


  if (false)
  {
    if ((fd = open ("/dev/gpiomem", O_RDWR | O_SYNC | O_CLOEXEC) ) < 0)
      printf("wiringPiSetup: Unable to open /dev/gpiomem\n");
    RASPBERRY_PI_PERI_BASE = 0 ;
  }
  else
  {


    if (geteuid () != 0)
      printf("UID Error, did you forget sudo?\n");

    if ((fd = open ("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC) ) < 0)
      printf("Unable to open /dev/mem \n");
  }

//  GPIO:
  gpio = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, GPIO_BASE) ;
  if ((int32_t)gpio == -1)
    printf("wiringPiSetup: mmap (GPIO) failed\n");

  /*piBoardId (&model, &rev, &mem, &maker, &overVolted) ;
  if (model == PI_MODEL_CM)
    wiringPiMode = WPI_MODE_GPIO ;
  else*/
  wiringPiMode = WPI_MODE_PINS ;

  return 0 ;
}

void pinMode (int pin, int mode)
{
  int    fSel, shift, alt ;
  int origPin = pin ;

  if ((pin & PI_GPIO_MASK) == 0)    // On-board pin
  {
    if (wiringPiMode == WPI_MODE_PINS)
      pin = pinToGpio [pin] ;
    else if (wiringPiMode == WPI_MODE_PHYS)
      pin = physToGpio [pin] ;
    else if (wiringPiMode != WPI_MODE_GPIO)
      return ;

   // softPwmStop  (origPin) ;
   // softToneStop (origPin) ;

    fSel    = gpioToGPFSEL [pin] ;
    shift   = gpioToShift  [pin] ;

    if (mode == INPUT)
      *(gpio + fSel) = (*(gpio + fSel) & ~(7 << shift)) ; // Sets bits to zero = input
    else if (mode == OUTPUT)
      *(gpio + fSel) = (*(gpio + fSel) & ~(7 << shift)) | (1 << shift) ;
  }
  else
  {
    printf("Error on-board pin\n");
    return ;

  }
}
void digitalWrite (int pin, int value)
{

  if ((pin & PI_GPIO_MASK) == 0)    // On-Board Pin
  {
    if (wiringPiMode == WPI_MODE_GPIO_SYS) // Sys mode
    {
      if (sysFds [pin] != -1)
      {
        if (value == LOW)
          write (sysFds [pin], "0\n", 2) ;
        else
          write (sysFds [pin], "1\n", 2) ;
      }
      return ;
    }
    else if (wiringPiMode == WPI_MODE_PINS)
      pin = pinToGpio [pin] ;
    else if (wiringPiMode == WPI_MODE_PHYS)
      pin = physToGpio [pin] ;
    else if (wiringPiMode != WPI_MODE_GPIO)
      return ;

    if (value == LOW)
      *(gpio + gpioToGPCLR [pin]) = 1 << (pin & 31) ;
    else
      *(gpio + gpioToGPSET [pin]) = 1 << (pin & 31) ;
  }
  else
  {
    /*if ((node = wiringPiFindNode (pin)) != NULL)
      node->digitalWrite (node, pin, value) ;*/
    printf("Error on-board pin\n");
  }
}
