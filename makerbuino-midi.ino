
#include <MidiSdFatBase.h>
#include <midi2.h>

#include <SPI.h>
#include "gamebuino_main_alt.h"
#include <Sound4.h>

#define _console gb.display
#define _debug Serial

MIDIFileTrack file;
midiFileSdFat midi_file;

Sound4 sound;
Gamebuino gb;

#define NUM_CHANNELS channels
#define VOLUME_CHANNEL_MAX 255

SdFat SD;
//Adafruit_PCD8544 display = Adafruit_PCD8544(SCR_CLK, SCR_DIN, SCR_DC, SCR_CS, SCR_RST);


#define lfnSize 25
char lfn[lfnSize];
//char * lfn;


void waitForUpdate()
{
  while (!gb.update()) {}
}


inline bool checkExtension(char *filename, uint8_t namelen, char *ext, uint8_t extlen)
{
  return (strcmp(strlwr(filename + (namelen - extlen)), strlwr(ext)) == 0);
}


void setup() {
  Serial.begin(115200);
  gb.begin();
  sound.begin();

  Serial.println(((CHAR_BIT * sizeof(phase_accumulator_t)) - (CHAR_BIT * sizeof(amplitude_t))));

  _console.println(F("reading SD card..."));
  waitForUpdate();

  if (!SD.begin(SD_CS/*, SPI_HALF_SPEED*/)) {
    _console.println(F("SD card not found"));
    _debug.println(F("SD failed"));
  } else {
    _console.println(F("SD card OK"));
    _debug.println(F("SD ok"));
  }
  waitForUpdate();
  delay(200);
  gb.display.clear();
}


void loop() {
  sd_explore_simple();
  gb.display.clear();
  gb.display.persistence = true;

  executeEvents();
  file.close();
}

void sd_explore_simple()
{
  //gb.display.persistence = false;
  while (true)
  {
    SD.vwd()->rewind();
    //File dir = SD.open("/");
    //dir.rewindDirectory();
    while (file.openNext(SD.vwd(), O_READ)) {
    //while (file = dir.openNextFile()) {
      //lfn = file.name();
      //file.getName(lfn, lfnSize);
file.getSFN (lfn);
      
      gb.display.clear();
      uint8_t len = strlen(lfn);
      if (checkExtension(lfn, len, ".MID", 4) || checkExtension(lfn, len, ".MIDI", 5))
      {
        //file.printName(&(gb.display));
      file.getName(lfn, lfnSize);
        gb.display.print(lfn);
        waitForUpdate();
        boolean exit2 = false;
        while (!exit2) {
          gb.buttons.update();
          delay(50);
          if (gb.buttons.repeat(BTN_DOWN, 4)) {
            exit2 = true;
          }
          if (gb.buttons.pressed(BTN_A))
          {
            return;
          }
        }
      }
      file.close();
    }
    gb.display.print(F("rewinding"));
    waitForUpdate();
    delay(500);
  }
}
