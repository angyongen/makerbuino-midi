
#define _console gb.display
#define _debug Serial


struct NoteEvent
{
  uint32_t time;
  uint8_t type;
  uint8_t note;
};

uint8_t channelFreq[NUM_CHANNELS];

bool noteOnEvent(uint8_t note)
{
  for (int ch = 0; ch < NUM_CHANNELS; ch++)
  {
    if (channelFreq[ch] == 0)
    {
      sound.playNote(ch, VOLUME_CHANNEL_MAX / 2, note - 9);
      channelFreq[ch] = note;
      return true;
    }
  }
  return false;
}

void clearChannel(uint8_t channel)
{
  sound.stopNote(channel);
  channelFreq[channel] = 0;
}

void panic()
{
  for (int ch = 0; ch < NUM_CHANNELS; ch++)
  {
    clearChannel(ch);
  }
}

bool noteOffEvent(uint8_t note)
{
  for (int ch = 0; ch < NUM_CHANNELS; ch++)
  {
    int chb = NUM_CHANNELS - 1 - ch;
    if (channelFreq[chb] == note)
    {
      clearChannel(chb);
      channelFreq[chb] = 0;
      return true;
    }
  }
  return false;
}

uint32_t get_time()
{
  return micros();
  //return sound.micros() / 1000;
}


inline void printlnOkFailed(bool test)
{
  if (test)
  {
    _console.println(F("ok"));
  } else {
    _console.println(F("failed"));
  }
}
uint16_t currentEvent;
uint32_t t;
uint32_t eventTime;
uint32_t nextEventTime = 0;

//Pulses per quarter note (PPQ)
uint32_t ppq; //https://www.recordingblogs.com/wiki/time-division-of-a-midi-file
//microseconds per beat
uint32_t uspb = 500000;
//microseconds per tick
uint32_t uspt;

void eventPlayer()
{
  uspt = uspb / ppq;
  midiEvent currentEventData;

  t = get_time();
  while (midi_file.findNextNoteEvent(file, currentEventData))
  {
    nextEventTime = nextEventTime + (currentEventData.delta * uspt);
    //Serial.print(nextEventTime);
    //Serial.print(F("-"));
    //Serial.print(currentEventData.event_type);
    //Serial.print(F("-"));
    //Serial.println(currentEventData.data[0]);
    while ((eventTime = get_time() - t) < nextEventTime)
    {
      _console.clear();
      _console.println(sound.PWM_INTERRUPT_FREQUENCY);
      gb.buttons.update();
      if(gb.buttons.repeat(BTN_UP,2)) sound.PWM_INTERRUPT_FREQUENCY+=1;
      if(gb.buttons.repeat(BTN_DOWN,2)) sound.PWM_INTERRUPT_FREQUENCY-=1;
      
      //_console.println(eventTime);
      //_console.println(nextEventTime);
      for (int ch = 0; ch < NUM_CHANNELS; ch++)
      {
        _console.println(channelFreq[ch]);
      }
      _console.update();
    }
    uint8_t event_status_type = ((currentEventData.event_status >> 4) & 0xF);
    if (event_status_type == midiNoteOn && currentEventData.data[1] == 0) event_status_type == midiNoteOff;
    switch (event_status_type)
    {
      case midiNoteOff:
        Serial.print("NoteOff");
        Serial.println(currentEventData.data[0]);
        noteOffEvent(currentEventData.data[0]);
        break;
      case midiNoteOn:
        Serial.print("NoteOn");
        Serial.println(currentEventData.data[0]);
        noteOnEvent(currentEventData.data[0]);
        break;
      default:
        break;
    }

   // for (int ch = 0; ch < NUM_CHANNELS; ch++)
  //  {
   //   Serial.print("\t");
   //   Serial.print(channelFreq[ch]);
   // }
   // Serial.println();

  }

}

void executeEvents()
{
  _console.print(F("loading "));
  while (!gb.update()) {}
  //file.printName(&(_console));

  _console.println();
  _console.print(F("midi "));
  bool midi_ok = midi_file.loadFile(file);
  printlnOkFailed(midi_ok);
  _console.print(F("midi format "));
  _console.println(midi_file.format());
  _console.print(F("midi division "));
  _console.println(midi_file.time_division());
  ppq = midi_file.time_division();
  _console.print(midi_file.readable_tracks());
  _console.print(F("/"));
  _console.print(midi_file.tracks());
  _console.println(F(" tracks readable"));
  while (!gb.update()) {}
  do {
    delay(50);
    gb.buttons.update();
  } while (!gb.buttons.pressed(BTN_A));
  if (midi_ok)
  {
    //eventChecker();
    eventPlayer();
  }

  Serial.println(F("midi done"));
  _console.println(F("midi done"));
  while (!gb.update()) {}

  do {
    delay(50);
    gb.buttons.update();
  } while (!gb.buttons.pressed(BTN_A));
  //sound.test();

  /*
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(0,0);
    display.println("Hello, world!");
    display.setTextColor(WHITE, BLACK); // 'inverted' text
    display.println(3.141592);
    display.setTextSize(2);
    display.setTextColor(BLACK);

    currentEvent = 0;
    t = get_time();
    while (true)
    {
    eventTime = get_time() - t;
    //Serial.print(eventTime);
    //Serial.print("\t");
    //Serial.println(currentEvent);
    NoteEvent thisEvent;
    if (eventTime >= thisEvent.time)
    {
      switch (thisEvent.type)
      {
        case 0:
          Serial.print("NoteOff");
          noteOffEvent(thisEvent.note);
          break;
        case 1:
          Serial.print("NoteOn");
          noteOnEvent(thisEvent.note);
          break;
      }
      if (++currentEvent >= num_events)
      {
        return;
      }
        for (int ch = 0; ch < NUM_CHANNELS; ch++)
        {
        Serial.print("\t");
        Serial.print(channelFreq[ch]);
        }
        Serial.println();
    }
    }
  */
  panic();
}
