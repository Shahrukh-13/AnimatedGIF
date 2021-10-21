// TFT_eSPI_memory
//
// Example sketch which shows how to display an
// animated GIF image stored in FLASH memory
//
// written by Larry Bank
// bitbank@pobox.com
//
// Adapted by Bodmer for the TFT_eSPI Arduino library:
// https://github.com/Bodmer/TFT_eSPI
//
// To display a GIF from memory, a single callback function
// must be provided - GIFDRAW
// This function is called after each scan line is decoded
// and is passed the 8-bit pixels, RGB565 palette and info
// about how and where to display the line. The palette entries
// can be in little-endian or big-endian order; this is specified
// in the begin() method.
//
// The AnimatedGIF class doesn't allocate or free any memory, but the
// instance data occupies about 22.5K of RAM.

//#define USE_DMA       // ESP32 ~1.25x single frame rendering performance boost for badgers.h
                        // Note: Do not use SPI DMA if reading GIF images from SPI SD card on same bus as TFT
  #define NORMAL_SPEED  // Comment out for rame rate for render speed test

// Load GIF library
#include <AnimatedGIF.h>
AnimatedGIF gif;

// Example AnimatedGIF library images

// For ESP32 TTGO 135 X 240
//#include "firewatch.h"
//#include "family.h"
//#include "DragonFire.h"
//#include "fox.h"
//#include "fire.h"
//#include "skullBoom.h"
//#include "darksouls.h"
//#include "gryffindor.h"
//#include "cozyfireplace.h"
//#include "campfire.h"
//#include "redDragon.h"
//#include "bonfire.h"
//#include "sakuraTree.h"
//#include "redSunset.h"
//#include "planet.h"
                                // ESP32 40MHz SPI single frame rendering performance
                                // Note: no DMA performance gain on smaller images or transparent pixel GIFs
//#define GIF_IMAGE firewatch   //  No DMA  63 fps, DMA:  71fps
//#define GIF_IMAGE family
//#define GIF_IMAGE DragonFire    
//#define GIF_IMAGE fox
//#define GIF_IMAGE fire
//#define GIF_IMAGE skullBoom
//#define GIF_IMAGE darksouls
//#define GIF_IMAGE gryffindor
//#define GIF_IMAGE cozyfireplace
//#define GIF_IMAGE campfire
//#define GIF_IMAGE redDragon
//#define GIF_IMAGE bonfire
//#define GIF_IMAGE sakuraTree
//#define GIF_IMAGE redSunset
//#define GIF_IMAGE planet
 

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////


// For ESP32 ST7789V 320 X 240
//#include "family2.h"
//#include "planet2.h"
#include "deer.h"
//#include "bonfire2.h"     
//#include "gryffindor2.h"   
//#include "darksouls2.h"          

//#define GIF_IMAGE family2
//#define GIF_IMAGE planet2
#define GIF_IMAGE deer
//#define GIF_IMAGE bonfire2
//#define GIF_IMAGE gryffindor2
//#define GIF_IMAGE darksouls2


/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);

  tft.begin();
#ifdef USE_DMA
  tft.initDMA();
#endif
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  gif.begin(BIG_ENDIAN_PIXELS);
}

#ifdef NORMAL_SPEED // Render at rate that is GIF controlled
void loop()
{
  // Put your main code here, to run repeatedly:
  if (gif.open((uint8_t *)GIF_IMAGE, sizeof(GIF_IMAGE), GIFDraw))
  {
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    tft.startWrite(); // The TFT chip slect is locked low
    while (gif.playFrame(true, NULL))
    {
      yield();
    }
    gif.close();
    tft.endWrite(); // Release TFT chip select for other SPI devices
  }
}
#else // Test maximum rendering speed
void loop()
{
  long lTime = micros();
  int iFrames = 0;

  if (gif.open((uint8_t *)GIF_IMAGE, sizeof(GIF_IMAGE), GIFDraw))
  {
    tft.startWrite(); // For DMA the TFT chip slect is locked low
    while (gif.playFrame(false, NULL))
    {
      // Each loop renders one frame
      iFrames++;
      yield();
    }
    gif.close();
    tft.endWrite(); // Release TFT chip select for other SPI devices
    lTime = micros() - lTime;
    Serial.print(iFrames / (lTime / 1000000.0));
    Serial.println(" fps");
  }
}
#endif
