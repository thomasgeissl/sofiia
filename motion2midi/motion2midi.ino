#include "./config.h"

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#include <Adafruit_MPU6050.h>
#include <Wire.h>


Adafruit_USBD_MIDI usb_midi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);
Adafruit_MPU6050 mpu;


void setup() {
  Serial.begin(115200);
  Serial.println("setting up midi device");
  usb_midi.setStringDescriptor("motion2midi");
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // If already enumerated, additional class driverr begin() e.g msc, hid, midi won't take effect until re-enumeration
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  Serial.println("setting up motion sensor");
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void loop() {
#ifdef TINYUSB_NEED_POLLING_TASK
  // Manual call tud_task since it isn't called by Core's background
  TinyUSBDevice.task();
#endif

  // MIDI.sendNoteOn(note_sequence[position], 127, 1);
  // MIDI.sendNoteOff(note_sequence[previous], 0, 1);

  MIDI.read();
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  MIDI.sendControlChange(ACCEL_X_CC, map(a.acceleration.x, -10, 10, 0, 127), 1);  //TODO: in range
  MIDI.sendControlChange(ACCEL_Y_CC, map(a.acceleration.y, -10, 10, 0, 127), 1);  //TODO: in range
  MIDI.sendControlChange(ACCEL_Z_CC, map(a.acceleration.z, -10, 10, 0, 127), 1);  //TODO: in range
    MIDI.sendControlChange(GYRO_X_CC, map(g.orientation.x, -10, 10, 0, 127), 1);  //TODO: in range
  MIDI.sendControlChange(GYRO_Y_CC, map(g.orientation.y, -10, 10, 0, 127), 1);  //TODO: in range
  MIDI.sendControlChange(GYRO_Z_CC, map(g.orientation.z, -10, 10, 0, 127), 1);  //TODO: in range

  //TODO: if run to fast then dalay for some ms
}

void handleNoteOn(byte channel, byte pitch, byte velocity) {}
void handleNoteOff(byte channel, byte pitch, byte velocity) {}
