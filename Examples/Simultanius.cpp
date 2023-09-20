#include <JDFraire_Buttons.h>

// Define the pins for your buttons and their modes
Button button1(2, BUTTON_PULLUP);
Button button2(3, BUTTON_PULLUP);

void setup() {
  // Initialize buttons
  button1.begin();
  button2.begin();

  // Attach functions to button click events
  button1.attachClickAction(onButtonClick1);
  button2.attachClickAction(onButtonClick2);

  // Attach function to simultaneous click event
  button1.attachSimultaneousClickAction(onSimultaneousClick, &button2);
}

void loop() {
  // Handle button events
  button1.executeActions();
  button2.executeActions();
}

void onButtonClick1() {
  // Handle button 1 click event
  // Your custom code here
}

void onButtonClick2() {
  // Handle button 2 click event
  // Your custom code here
}

void onSimultaneousClick() {
  // Handle simultaneous button click event
  // This function will be called when both buttons are clicked simultaneously
  // Your custom code here
}
