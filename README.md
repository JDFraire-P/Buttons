# JDFraire Buttons Library

![Version](https://img.shields.io/badge/version-1.0-brightgreen.svg)
![Arduino Nano Every](https://img.shields.io/badge/Arduino-compatible-blue.svg)

## Description

The JDFraire Buttons Library is a versatile Arduino library designed to simplify button control and interaction. It provides an easy way to manage button states, detect clicks and long clicks, and handle simultaneous button presses. This library is especially useful for projects that involve user input through physical buttons.

## Features

- Supports both PULLUP and PULLDOWN button modes.
- Provides a straightforward API for managing button states, clicks, and long clicks.
- Allows the Arduino to wake from sleep mode when a button is pressed.
- Enables the execution of custom functions on button events.
- Handles simultaneous button presses between multiple buttons.
- Efficiently stores button events in a circular buffer.

## Installation

1. Download the JDFraire Buttons Library from the [GitHub repository](https://github.com/JDFraire-P/Buttons).
2. Open the Arduino IDE.
3. Go to Sketch > Include Library > Add .ZIP Library.
4. Select the downloaded library ZIP file.
5. The library is now installed and ready to use.

## Usage

Here's a simple example of how to use the JDFraire Buttons Library:

```cpp
#include <JDFraire_Buttons.h>

// Define your button pins and modes
Button button1(2, BUTTON_PULLUP);
Button button2(3, BUTTON_PULLDOWN);

void setup() {
  // Initialize buttons
  button1.begin();
  button2.begin();

  // Attach functions to button events
  button1.attachClickAction(onButtonClick);
  button2.attachLongClickAction(onButtonLongClick);
}

void loop() {
  // Handle button events
  button1.executeActions();
  button2.executeActions();
}

void onButtonClick() {
  // Handle button click event
  // Your custom code here
}

void onButtonLongClick() {
  // Handle button long click event
  // Your custom code here
}
```


## Contributing

Contributions to this library are welcome! If you have suggestions, bug reports, or want to contribute code, please open an issue or submit a pull request on the [GitHub repository](https://github.com/JDFraire-P/Buttons).

## License

This library is open-source and available under the [MIT License](LICENSE).
