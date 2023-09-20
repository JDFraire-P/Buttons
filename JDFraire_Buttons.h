// JDFraire_Buttons.h

//  Creator: JDFraire

//  Description: This library is used to control Arduino with buttons.

//  Date: 2023-08-31

//  Version: 1.0

#ifndef JD_FRAIRE_BUTTONS_H
#define JD_FRAIRE_BUTTONS_H

#include <Arduino.h>
#include <avr/sleep.h>

// Button modes, PULLUP = 0, PULLDOWN = 1, 
enum ButtonMode{
  BUTTON_PULLUP = INPUT_PULLUP,
  BUTTON_PULLDOWN = INPUT_PULLDOWN
};

// Button states, RELEASED = 0, PRESSED = 1, UNKNOWN = 2
enum ButtonState{
  BUTTON_RELEASED = 0, // digitalRead() returns 0 when the button is released
  BUTTON_PRESSED = 1, // digitalRead() returns 1 when the button is pressed
  BUTTON_UNKNOWN = 2, // used for initialization
  BUTTON_LOST = 3 // used when the event is lost
};


struct ButtonClick {
  unsigned long timestamp = 0; // time of event start in milliseconds
  unsigned long duration = 0; // duration of the click in milliseconds
  bool simultaneous = false; // simultaneous click event
};

const byte BufferClicksSize = 10; // size of the buffer. 2 events per click, 3 clicks per button
const unsigned long clickTime = 50; // time in milliseconds to consider a click
const unsigned long longClickTime = 1500; // time in milliseconds to consider a long click
const unsigned long simultaneousClickTime = 1000; // time in milliseconds to consider a simultaneous click

class Button{
public:
  Button(byte pin, ButtonMode mode); // Constructor
  byte getPin(); // Get the pin number
  ButtonMode getMode(); // Get the mode of the button
  void begin(); // Initialize the button, attach the interrupt

  void sleep(); // set the button to awake the microcontroller from sleep mode
  void attachAwakeRoutine(void (*function)()); // Attach a function to the awake routine

  void setState(ButtonState state); // Set the state of the button
  ButtonState getState(); // Get the state of the button
  void setLastState(ButtonState state); // Set the last state of the button
  ButtonState getLastState(); // Get the last state of the button

  bool isPressed(); // Check if the button is pushed
  bool isReleased(); // Check if the button is released

  volatile void addClick(); // Add the current click to the buffer
  int availableClicks(); // Get the number of clicks in the buffer
  ButtonClick getClick(); // Get the next click available from the buffer 
  void wipeClicks(); // Wipe the buffer of clicks

  unsigned long getCurrentClickTimestamp(); // Get the timestamp of the current click (timestamp of the last push) returns 0 if there is no current click
  unsigned long getCurrentClickDuration(); // Get the duration of the current click (time between the last push and the last release) returns 0 if there is no current click
  unsigned long getLastClickTimestamp(); // Get the timestamp of the last click (timestamp of the last push) returns 0 if there is no last click
  unsigned long getLastClickDuration(); // Get the duration of the last click (time between the last push and the last release) returns 0 if there is no last click
  
  void setCurrentClickTimestamp(unsigned long timestamp); // Set the timestamp of the current click (timestamp of the last push) returns 0 if there is no current click
  void setCurrentClickDuration(unsigned long duration); // Set the duration of the current click (time between the last push and the last release) returns 0 if there is no current click

  unsigned long getReleaseDuration(); // Get the duration of the last release (time between the last push and the last release)


  void attachClickAction(void (*function)()); // Attach a function to the click event
  void attachLongClickAction(void (*function)()); // Attach a function to the click event

  void attachSimultaneousClickAction(void (*function)(), Button *button); // Attach a function to the simultaneous click event
  void checkSimultaneousClick(); // Check if the simultaneous click event is triggered
  void confirmSimultaneousClick(); // Confirm the simultaneous click event
  void executeActions(); // Execute the actions of the events in the buffer

  bool getSimultaneousResponse(); // Get the simultaneous response

  int getClicksCount(); // Get the number of clicks

  void (*awakeRoutine)() = NULL; // Function to execute when the button is pressed to awake the microcontroller from sleep mode

private:
  static void Click_isr(void *arg); // Interrupt service routine with pointer to the object, add the timestamp to the current click when change from LOW to HIGH and add the current click to the buffer When change from HIGH to LOW
  static void Awake_isr(void *arg); // Interrupt service routine to awake the microcontroller from sleep mode
  byte pin; // Pin number
  ButtonMode mode; // Mode of the button (INPUT_PULLUP or INPUT_PULLDOWN)
  volatile ButtonState state = BUTTON_UNKNOWN; // Current state of the button
  volatile ButtonState lastState = BUTTON_UNKNOWN; // Last state of the button
  volatile ButtonClick currentClick; // Current click
  volatile ButtonClick lastClick; // Last click
  volatile ButtonClick BufferClicks[BufferClicksSize]; // Buffer of clicks

  volatile byte BufferClicksIndex_new = 0; // Index of the last click added to the buffer
  volatile byte BufferClicksIndex_old = 0; // Index of the oldest click in the buffer
  volatile byte BufferClicksIndex_write = 0; // Index of the buffer for the next click to be added
  volatile byte BufferClicksIndex_read = 0; // Index of the buffer for the next click to be read

  volatile int ClicksCount = 0; // Number of clicks

  void (*clickAction)() = NULL; // Function to execute when the button is clicked
  void (*longClickAction)() = NULL; // Function to execute when the button is long clicked

  void (*simultaneousClickAction)() = NULL; // Function to execute when the button is clicked
  Button *simultaneousClickButton = NULL; // Button to check if the simultaneous click event is triggered

  bool simultaneousResponse = false; // what button executes the simultaneous click event (true = this button, false = the other button)
  volatile bool simultaneousClick = false; // simultaneous click event triggered

};

#endif