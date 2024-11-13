#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#include <Adafruit_VL53L0X.h>

#define TOUCH_PIN 4

Adafruit_USBD_MIDI usb_midi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);
Adafruit_VL53L0X lox = Adafruit_VL53L0X();


void setup() {
  Serial.begin(115200);
  Serial.println("setting up midi device");
  usb_midi.setStringDescriptor("distance2midi");
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // If already enumerated, additional class driverr begin() e.g msc, hid, midi won't take effect until re-enumeration
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  Serial.println("setting up distance sensor");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while (1){}
  }
}

void loop() {
#ifdef TINYUSB_NEED_POLLING_TASK
  // Manual call tud_task since it isn't called by Core's background
  TinyUSBDevice.task();
#endif


  // MIDI.sendNoteOn(note_sequence[position], 127, 1);
  // MIDI.sendNoteOff(note_sequence[previous], 0, 1);

  MIDI.read();
  VL53L0X_RangingMeasurementData_t measure;

  // if (lox.isRangeComplete()) {
  //   Serial.print("Distance in mm: ");
  //   Serial.println(lox.readRange());
  //   auto range = lox.readRange();
  //   MIDI.sendControlChange(1, round(range / 10), 1);
  // }
  lox.rangingTest(&measure, false);  // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    MIDI.sendControlChange(1, map(measure.RangeMilliMeter, 0, 1000, 0, 127), 1);
    Serial.println(measure.RangeMilliMeter);
  } else {
    Serial.println(" out of range ");
  }

  int touchValue = touchRead(TOUCH_PIN);
  // Serial.println(touchValue);
  // MIDI.sendControlChange(1, round(range / 10), 1);
}

void handleNoteOn(byte channel, byte pitch, byte velocity) {}
void handleNoteOff(byte channel, byte pitch, byte velocity) {}
