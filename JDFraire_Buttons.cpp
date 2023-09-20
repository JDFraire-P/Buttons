// JDFraire_Buttons.cpp

//  Creator: JDFraire

//  Description: Buttons library.

//  Date: 2023-08-31

//  Version: 1.0

#include "JDFraire_Buttons.h"

// Constructor
Button::Button(byte pin, ButtonMode mode): pin(pin), mode(mode) {}

// Get the pin number
byte Button::getPin() {
  return pin;
}

// Get the mode of the button
ButtonMode Button::getMode() {
  return mode;
}

// Initialize the button, attach the interrupt
void Button::begin() {
  pinMode(getPin(), mode);
    // Read the pin
  int read = digitalRead(getPin());
  setState(read == HIGH ? BUTTON_PRESSED : BUTTON_RELEASED);
  if(read == HIGH) currentClick.timestamp = millis();
  attachInterruptParam(digitalPinToInterrupt(pin), Click_isr, CHANGE, this);
}

// Set the button to awake the microcontroller from sleep mode
void Button::sleep() {
  noInterrupts();
  detachInterrupt(digitalPinToInterrupt(getPin()));
  attachInterruptParam(digitalPinToInterrupt(getPin()), Awake_isr, CHANGE, this);
  interrupts();
}

// Attach a function to the awake routine
void Button::attachAwakeRoutine(void (*function)()) {
  awakeRoutine = function;
}

// Set the state of the button
void Button::setState(ButtonState state) {
  this->state = state;
}

// Get the state of the button
ButtonState Button::getState() {
  return state;
}

// Set the last state of the button
void Button::setLastState(ButtonState state) {
  lastState = state;
}

// Get the last state of the button
ButtonState Button::getLastState() {
  return lastState;
}

// Check if the button is pushed
bool Button::isPressed() {
  return state == BUTTON_PRESSED;
}

// Check if the button is released
bool Button::isReleased() {
  return state == BUTTON_RELEASED;
}

// Add the current click to the buffer
volatile void Button::addClick() {

  BufferClicks[BufferClicksIndex_write].timestamp = currentClick.timestamp;
  BufferClicks[BufferClicksIndex_write].duration = millis() - currentClick.timestamp;
  BufferClicks[BufferClicksIndex_write].simultaneous = simultaneousClick;


  lastClick.timestamp = BufferClicks[BufferClicksIndex_write].timestamp;
  lastClick.duration = BufferClicks[BufferClicksIndex_write].duration;
  lastClick.simultaneous = BufferClicks[BufferClicksIndex_write].simultaneous;

  currentClick.timestamp = 0;
  currentClick.duration = 0;
  currentClick.simultaneous = false;

  simultaneousClick = false;


  BufferClicksIndex_new = BufferClicksIndex_write;
  BufferClicksIndex_write = (BufferClicksIndex_write + 1) % BufferClicksSize; // increment the index

  // if the buffer is full, the most old event is BufferEventIndex
  if(BufferClicksIndex_write == BufferClicksIndex_old){
    BufferClicksIndex_old = BufferClicksIndex_write; 
  }


  ClicksCount++;

}

// Get the number of clicks in the buffer
int Button::availableClicks() {
  if(BufferClicksIndex_write >= BufferClicksIndex_read){
    return BufferClicksIndex_write - BufferClicksIndex_read;
  }
  else{
    return BufferClicksIndex_write + BufferClicksSize - BufferClicksIndex_read;
  }
}

// Get the next click available from the buffer
ButtonClick Button::getClick() {
  ButtonClick click;
  if(availableClicks() > 0){
    click.timestamp = BufferClicks[BufferClicksIndex_read].timestamp;
    click.duration = BufferClicks[BufferClicksIndex_read].duration;
    click.simultaneous = BufferClicks[BufferClicksIndex_read].simultaneous;
    BufferClicksIndex_read = (BufferClicksIndex_read + 1) % BufferClicksSize; // increment the index
  }
  else{
    click.timestamp = 0;
    click.duration = 0;
    click.simultaneous = false;
  }
  return click;
}

// Wipe the buffer of clicks
void Button::wipeClicks() {
  BufferClicksIndex_new = 0;
  BufferClicksIndex_old = 0;
  BufferClicksIndex_write = 0;
  BufferClicksIndex_read = 0;
}

// Get the timestamp of the current click (timestamp of the last push) returns 0 if there is no current click
unsigned long Button::getCurrentClickTimestamp() {
  return currentClick.timestamp;
}

// Get the duration of the current click (time between the last push and the last release) returns 0 if there is no current click
unsigned long Button::getCurrentClickDuration() {
  if(isPressed()) {
    currentClick.duration = millis() - currentClick.timestamp;
    return currentClick.duration;
  }
  else{
    return 0;
  }
}

// Set the timestamp of the current click (timestamp of the last push) returns 0 if there is no current click
void Button::setCurrentClickTimestamp(unsigned long timestamp) {
  currentClick.timestamp = timestamp;
}

// Set the duration of the current click (time between the last push and the last release) returns 0 if there is no current click
void Button::setCurrentClickDuration(unsigned long duration) {
  currentClick.duration = duration;
}

// Get the timestamp of the last click (timestamp of the last push) returns 0 if there is no last click
unsigned long Button::getLastClickTimestamp() {
  return lastClick.timestamp;
}

// Get the duration of the last click (time between the last push and the last release) returns 0 if there is no last click
unsigned long Button::getLastClickDuration() {
  return lastClick.duration;
}

// Get the duration of the last release (time between the last push and the last release)
unsigned long Button::getReleaseDuration() {
  if(isReleased()){
    return millis() - (lastClick.timestamp + lastClick.duration);
  }
  else{
    return 0; 
  }
}

// Attach a function to the click event
void Button::attachClickAction(void (*function)()) {
  clickAction = function;
}

// Attach a function to the long click event
void Button::attachLongClickAction(void (*function)()) {
  longClickAction = function;
}

// Attach a function to the simultaneous click event
void Button::attachSimultaneousClickAction(void (*function)(), Button *button) {
  simultaneousClickAction = function;
  simultaneousClickButton = button;
  if(getPin() > button->getPin()){
    simultaneousResponse = true;
    button->attachSimultaneousClickAction(function, this);
  }
  else{
    simultaneousResponse = false;
  }
}

// Check if the simultaneous click event is triggered
void Button::checkSimultaneousClick() {
 if(!simultaneousClick && simultaneousClickButton->isPressed() && simultaneousClickButton != NULL && simultaneousClickAction != NULL){
    if(abs(getCurrentClickTimestamp() - simultaneousClickButton->getCurrentClickTimestamp()) < simultaneousClickTime){
      confirmSimultaneousClick();
      simultaneousClickButton->confirmSimultaneousClick();
    }
  }
}

// Confirm the simultaneous click event
void Button::confirmSimultaneousClick() {
  simultaneousClick = true;
}

// Execute the actions of the events in the buffer, verify the simultaneous click event
void Button::executeActions() {
  while(availableClicks() > 0 && getCurrentClickDuration() < longClickTime){
    Serial.println(String(ClicksCount) + " Button: "+ String(getPin()) + " availableClicks: " + String(availableClicks()));
    ButtonClick click = getClick();
    if(getSimultaneousResponse() && click.simultaneous && simultaneousClickAction != NULL && click.duration < longClickTime){
      // Serial.println("simultaneous");
      simultaneousClickButton->wipeClicks();
      simultaneousClickAction();
    }
    else if(clickAction != NULL && !click.simultaneous && click.duration < longClickTime){
      // Serial.println("click");
      clickAction();
    }

  }
  while(getCurrentClickDuration() >= longClickTime && isPressed()){
    // Serial.println("long click " + String(getCurrentClickDuration()));
    // if(availableClicks() > 0) wipeClicks();
    if(longClickAction != NULL){
      longClickAction();
    }
  }
}

// Get the simultaneous response
bool Button::getSimultaneousResponse() {
  return simultaneousResponse;
}

// Interrupt service routine with pointer to the object, add the timestamp to the current click when change from LOW to HIGH and add the current click to the buffer When change from HIGH to LOW
void Button::Click_isr(void *arg) {
  unsigned long timestamp = millis();
  Button *button = (Button *)arg;
  // Read the pin
  int read = digitalRead(button->getPin());
  button->setLastState(button->getState());
  button->setState(read == HIGH ? BUTTON_PRESSED : BUTTON_RELEASED);
  // Add event to the buffer
  if(button->isPressed()){
    Serial.println("pressed");
    button->setCurrentClickTimestamp(timestamp);
    button->checkSimultaneousClick();
  }
  else if(button->isReleased()){
    Serial.println("released");
    button->addClick();
  }
}

// Interrupt service routine to awake the microcontroller from sleep mode
void Button::Awake_isr(void *arg) {
  // awake the microcontroller from sleep mode
  sleep_disable();
  unsigned long timestamp = millis();
  Button *button = (Button *)arg;
  // Read the pin
  int read = digitalRead(button->getPin());
  button->setLastState(button->getState());
  button->setState(read == HIGH ? BUTTON_PRESSED : BUTTON_RELEASED);
  // Add event to the buffer
  if(button->isPressed()){
    Serial.println("pressed");
    button->setCurrentClickTimestamp(timestamp);
    button->checkSimultaneousClick();
  }
  else if(button->isReleased()){
    Serial.println("released");
    button->addClick();
  }

  // detach the interrupt
  noInterrupts();
  detachInterrupt(digitalPinToInterrupt(button->getPin()));
  // attach the interrupt
  pinMode(button->getPin(), button->mode);
  attachInterruptParam(digitalPinToInterrupt(button->pin), Click_isr, CHANGE, button);
  interrupts();
  // awake routine if exist
  if(button->awakeRoutine != NULL){
    Serial.println("awake routine");
    button->awakeRoutine();
  }
}
