/*************************************************** 
  This is based extensively on the Adafruit VS1053 Codec Breakout sample
  provided by Adafruit Industries.  They're awesome!
  rlankenau@workshop88.com

  Designed specifically to work with the Adafruit VS1053 Codec Breakout 
  ----> https://www.adafruit.com/products/1381

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

#define BUTTON 5
#define OPTICAL A0
#define VOLUME A1
#define SENSE_THRESHOLD 200

// These can be any pins:
#define RESET 9      // VS1053 reset pin (output)
#define CS 7       // VS1053 chip select pin (output)
#define DCS 6        // VS1053 Data/command select pin (output)
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

char *names[7]={"Illinois Chorus Frog", "Coyote", "Eastern Gray Squirrel", "Eastern Fox Squirrel", "Great Horned Owl", "Sandhill Crane", "Wood Frog"};
char *files[7]={"chorfrog.ogg", "coyote.ogg", "esquir.ogg", "fsquir.ogg", "owl.ogg", "sandhill.ogg", "wfrog.ogg"};

Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(RESET, CS, DCS, DREQ, CARDCS);

void setup() {
  
  pinMode(BUTTON, INPUT_PULLUP); /* Added in case I want to add support for the button later. */
  randomSeed(analogRead(A5));
  
  Serial.begin(9600);
  Serial.println("Coindrop Initialized");

  musicPlayer.begin(); // initialise the music player
  SD.begin(CARDCS);    // initialise the SD card
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(20,20);

  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
  // audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
  
  // Play Owl effect to indicate that we're up and running.
  if(!musicPlayer.playFullFile(files[4]))
  {
    Serial.println("Could read data.  Make sure the SD card is inserted.");
  }
}

/* Test to see if there is a coin there, and if so, kick of a sound */
void checkCoin()
{
  int sense = analogRead(OPTICAL);
  if(sense < SENSE_THRESHOLD)
  {
    /* Pick a file to play */
    int idx = random(7);
    Serial.print("Playing ");
    Serial.println(names[idx]);
    musicPlayer.startPlayingFile(files[idx]);
  }
}

int current_volume = 20;
/* Adjust the volume */
void checkVolume()
{
  int volume = analogRead(VOLUME);
  /* Map the 0-1023 range to something more like 0-50 */
  int outvol = (1023-volume)/14;
  if(outvol!=current_volume) {
    Serial.print("Changed volume to ");
    Serial.println(outvol);
  }
  current_volume=outvol;
  musicPlayer.setVolume(outvol, outvol);
}

void loop() {
  checkVolume();
  if(musicPlayer.stopped())
  {
    checkCoin();
  }
}
