// -----
// AnalogButton.cpp - Library for detecting button clicks, doubleclicks and long press pattern on a single button.
// This class is implemented for use with the Arduino environment.
// Copyright (c) by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
// More information on: http://www.mathertel.de/Arduino
// -----
// Changelog: see AnalogButton.h
// -----

#include "AnalogButton.h"

// ----- Initialization and Default Values -----

AnalogButton::AnalogButton(int pin, int activeValue)
{
	pinMode(pin, INPUT);    // sets the MenuPin as input
  _pin = pin;

  _clickTicks = 600;        // number of millisec that have to pass by before a click is detected.
  _pressTicks = 1000;       // number of millisec that have to pass by before a long button press is detected.
  _tolerance  = 10;			// +- to be mess tolerance
  _state = 0; // starting with state 0: waiting for button to be pressed
  _isLongPressed = false;  // Keep track of long press state

  ActiveValue = activeValue; 
  
  
  
    // button connects ground to the resistor and the pin when pressed.
    _buttonReleased = 1023; // notPressed
    _buttonPressed = activeValue;
    digitalWrite(pin, HIGH);   // turn on pullUp resistor

 


  _doubleClickFunc = NULL;
  _pressFunc = NULL;
  _longPressStartFunc = NULL;
  _longPressStopFunc = NULL;
  _duringLongPressFunc = NULL;
} // AnalogButton


// explicitly set the number of millisec that have to pass by before a click is detected.
void AnalogButton::setClickTicks(int ticks) { 
  _clickTicks = ticks;
} // setClickTicks


// explicitly set the number of millisec that have to pass by before a long button press is detected.
void AnalogButton::setPressTicks(int ticks) {
  _pressTicks = ticks;
} // setPressTicks

// explicitly set the Tolerance
void AnalogButton::setTolerance(int tolerance){
	_tolerance = tolerance;
}// setTolerance

// save function for click event
void AnalogButton::attachClick(callbackFunction newFunction)
{
  _clickFunc = newFunction;
} // attachClick


// save function for doubleClick event
void AnalogButton::attachDoubleClick(callbackFunction newFunction)
{
  _doubleClickFunc = newFunction;
} // attachDoubleClick


// save function for press event
// DEPRECATED, is replaced by attachLongPressStart, attachLongPressStop, attachDuringLongPress, 
void AnalogButton::attachPress(callbackFunction newFunction)
{
  _pressFunc = newFunction;
} // attachPress

// save function for longPressStart event
void AnalogButton::attachLongPressStart(callbackFunction newFunction)
{
  _longPressStartFunc = newFunction;
} // attachLongPressStart

// save function for longPressStop event
void AnalogButton::attachLongPressStop(callbackFunction newFunction)
{
  _longPressStopFunc = newFunction;
} // attachLongPressStop

// save function for during longPress event
void AnalogButton::attachDuringLongPress(callbackFunction newFunction)
{
  _duringLongPressFunc = newFunction;
} // attachDuringLongPress

// function to get the current long pressed state
bool AnalogButton::isLongPressed(){
  return _isLongPressed;
}

void AnalogButton::tick(void)
{
  // Detect the input information 
  int buttonLevel = analogRead(_pin); // current button signal.
  unsigned long now = millis(); // current (relative) time in msecs.

  // Implementation of the state machine
  if (_state == 0) { // waiting for menu pin being pressed.
    if (checkLevel(buttonLevel)) {
      _state = 1; // step to state 1
      _startTime = now; // remember starting time
    } // if

  } else if (_state == 1) { // waiting for menu pin being released.

    if ((buttonLevel >= _buttonReleased-_tolerance) && ((unsigned long)(now - _startTime) < _debounceTicks)) {
      // button was released to quickly so I assume some debouncing.
	  // go back to state 0 without calling a function.
      _state = 0;

    } else if (buttonLevel >= _buttonReleased-_tolerance) {
      _state = 2; // step to state 2

    } else if (checkLevel(buttonLevel) && ((unsigned long)(now - _startTime) > _pressTicks)) {
      _isLongPressed = true;  // Keep track of long press state
      if (_pressFunc) _pressFunc();
	  if (_longPressStartFunc) _longPressStartFunc();
	  if (_duringLongPressFunc) _duringLongPressFunc();
      _state = 6; // step to state 6
      
    } else {
      // wait. Stay in this state.
    } // if

  } else if (_state == 2) { // waiting for menu pin being pressed the second time or timeout.
    if ((unsigned long)(now - _startTime) > _clickTicks) {
      // this was only a single short click
      if (_clickFunc) _clickFunc();
      _state = 0; // restart.

    } else if (checkLevel(buttonLevel)) {
      _state = 3; // step to state 3
    } // if

  } else if (_state == 3) { // waiting for menu pin being released finally.
    if (buttonLevel >= _buttonReleased-_tolerance) {
      // this was a 2 click sequence.
      if (_doubleClickFunc) _doubleClickFunc();
      _state = 0; // restart.
    } // if

  } else if (_state == 6) { // waiting for menu pin being release after long press.
    if (buttonLevel >= _buttonReleased-_tolerance) {
	  _isLongPressed = false;  // Keep track of long press state
	  if(_longPressStopFunc) _longPressStopFunc();
      _state = 0; // restart.
    } else {
	  // button is being long pressed
	  _isLongPressed = true; // Keep track of long press state
	  if (_duringLongPressFunc) _duringLongPressFunc();
    } // if  

  } // if  
} // AnalogButton.tick()

bool AnalogButton::checkLevel(int buttonLevel){
	if (buttonLevel < ActiveValue + _tolerance && buttonLevel > ActiveValue - _tolerance) return true;
	else return false;
}
// end.

