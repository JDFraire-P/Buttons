#include <JDFraire_Buttons.h>

// Define the pin for your wake-up button and its mode
Button wakeButton(2, BUTTON_PULLUP);

void setup() {
  // Initialize the wake-up button
  wakeButton.begin();

  // Attach a function to the wake-up button's click event
  wakeButton.attachClickAction(onWakeButtonClick);

  // Attach a function to the awake routine
  wakeButton.attachAwakeRoutine(onAwake);

  // Put the Arduino to sleep initially
  sleepNow();
}

void loop() {
  // Handle button events
  wakeButton.executeActions();
}

void onWakeButtonClick() {
  // Handle wake-up button click event
  // Your custom code here
}

void onAwake() {
  // Handle the awake routine
  // Your custom code here
}

void sleepNow() {
  // Disable interrupts
  noInterrupts();

  // Attach the interrupt to wake up from sleep mode
  wakeButton.sleep();

  // Put the Arduino to sleep
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  interrupts();
  sleep_cpu();

  // Arduino wakes up here when the button is pressed
}

void loop() {
  // Your main loop code goes here
}
