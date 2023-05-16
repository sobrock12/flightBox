/*

Arduino Micro Custom Flight Simulator Controller Sketch written by Stan Obrock, 15 May 2023
Uses Encoder library written by John Lluch https://github.com/John-Lluch/Encoder
Uses Joystick library written by Matthew Heironimus https://github.com/MHeironimus/ArduinoJoystickLibrary

Basic concept: Electronic components that have been connected to Arduino Micro board each have 
an emulated joystick button assigned to them. On operation of component, the emulated button is pressed and 
can be set in a flight sim program to trigger an action.

This set up uses 5 encoders for increasing and decreasing values in flight sim, 2 of which have multiple outputs 
assigned to them based on bat switch position, 2 bat switches for modifying certain encoder outputs, 
2 push buttons which also have different outputs assigned based on switch position, and one locking 
single throw single pole switch acting as a landing gear switch.

*/

#include <Encoder.h>
#include <Joystick.h>

//define joystick object parameters
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
  36, 0,                  // Button Count, Hat Switch Count
  false, false, false,     // X and Y, but no Z Axis
  false, false, false,   // No Rx, Ry, or Rz
  false, false,          // No rudder or throttle
  false, false, false);  // No accelerator, brake, or steering

//define encoder pins
#define enc1A 7
#define enc1B 6
#define enc2A 0
#define enc2B 1
#define enc3A 2
#define enc3B 3
#define enc4A 4
#define enc4B 5
#define enc5A 11
#define enc5B 12
#define pb 13

int testDelay = 50;

//define gear switch pins
int gearUp = A4;
int gearDown = A5;

//instantiate logic variables for gear switching
int prevGearPos = 0;
int currentGearPos;

//define mod switch pins
int switchL = A2;
int switchR = A3;

//define push button pins
int pushButton1 = A0;
int pushButton2 = A1;

//define pin for radio frequency whole/decimal switch
int radioSwitch = 13;

//counter for right push button to switch between 4 modes when mod switch is in right position
int counter = 0;

//create encoder objects. 
//encoder push buttons aren't being used, so an unconnected pin is passed in for those parameters
Encoder enc1(enc1A, enc1B, pb);
Encoder enc2(enc2A, enc2B, pb);
Encoder enc3(enc3A, enc3B, pb);
Encoder enc4(enc4A, enc4B, pb);
Encoder enc5(enc5A, enc5B, pb);

//create array for handling all encoder inputs
Encoder *encoderArray[] = {&enc1, &enc2, &enc3, &enc4, &enc5};

//instantaite encoder position/direction tracking variables
int enc1Val = 0;
int prevEnc1Val = -999;
int enc2Val = 0;
int prevEnc2Val = -999;
int enc3Val = 0;
int prevEnc3Val = -999;
int enc4Val = 0;
int prevEnc4Val = -999;
int enc5Val = 0;
int prevEnc5Val = -999;

void setup() {

  //set up pins for gear switch
  pinMode(gearUp, INPUT_PULLUP);
  pinMode(gearDown, INPUT_PULLUP);
  
  //set up pins for mod switch
  pinMode(switchL, INPUT_PULLUP);
  pinMode(switchR, INPUT_PULLUP);

  //set up pins for push buttons
  pinMode(pushButton1, INPUT_PULLUP);
  pinMode(pushButton2, INPUT_PULLUP);

  //set up pin for radio switch
  pinMode(radioSwitch, INPUT_PULLUP);

  //start encoder monitoring
  EncoderInterrupt.begin(encoderArray, 5);

  //start joystick emulation
  Joystick.begin();

}

void loop() {

  //instantiate variables that read gear switch position
  int gearUpState = digitalRead(gearUp);
  int gearDownState = digitalRead(gearDown);

  //instantiate variables that read mod switch position
  int switchLState = digitalRead(switchL);
  int switchRState = digitalRead(switchR);

  //instantiate variables that read push button presses
  int button1State = digitalRead(pushButton1);
  int button2State = digitalRead(pushButton2);

  //instantiate variable for radio switch position
  int radioSwitchState = digitalRead(radioSwitch);

  //encoder data handling
  int delta1 = enc1.delta();
  int delta2 = enc2.delta();
  int delta3 = enc3.delta();
  int delta4 = enc4.delta();
  int delta5 = enc5.delta();

  enc1Val += delta1;
  enc2Val += delta2;
  enc3Val += delta3;
  enc4Val += delta4;
  enc5Val += delta5;

  //read encoder 1 activity
  if (enc1Val != prevEnc1Val)
  {

    if (enc1Val < prevEnc1Val)
    {

      //Keyboard.println("1 R");
      //delay(75);
      Joystick.pressButton(0);
      delay(testDelay);
      Joystick.releaseButton(0);

    }

    if (enc1Val > prevEnc1Val)
    {

      //Keyboard.println("1 L");
      //delay(75);
      Joystick.pressButton(1);
      delay(testDelay);
      Joystick.releaseButton(1);

      
    }

    prevEnc1Val = enc1Val;

  }

  //read encoder 2 activity, button pressed changes based on mod switch position
  if (enc2Val != prevEnc2Val)
  {

    if (switchLState == LOW)
    {

      if (enc2Val < prevEnc2Val)
      {

        Joystick.pressButton(2);
        delay(testDelay);
        Joystick.releaseButton(2);
        delay(25);
        Joystick.pressButton(2);
        delay(testDelay);
        Joystick.releaseButton(2);
        delay(25);
        Joystick.pressButton(2);
        delay(testDelay);
        Joystick.releaseButton(2);

      }

      if (enc2Val > prevEnc2Val)
      {

        Joystick.pressButton(3);
        delay(testDelay);    
        Joystick.releaseButton(3);
        delay(25);
        Joystick.pressButton(3);
        delay(testDelay);    
        Joystick.releaseButton(3);
        delay(25);
        Joystick.pressButton(3);
        delay(testDelay);    
        Joystick.releaseButton(3);

      }

    }

    if (switchRState == LOW)
    {

      if (enc2Val < prevEnc2Val)
      {

        Joystick.pressButton(14);
        delay(testDelay);
        Joystick.releaseButton(14);

      }

      if (enc2Val > prevEnc2Val)
      {

        Joystick.pressButton(15);
        delay(testDelay);    
        Joystick.releaseButton(15);
        
      }

    }

    if (radioSwitchState == HIGH && switchLState == HIGH && switchRState == HIGH)
    {

      if (enc2Val < prevEnc2Val)
      {

        Joystick.pressButton(16);
        delay(testDelay);
        Joystick.releaseButton(16);

      }

      if (enc2Val > prevEnc2Val)
      {

        Joystick.pressButton(17);
        delay(testDelay);    
        Joystick.releaseButton(17);
        
      }

    }

    if (radioSwitchState == LOW && switchLState == HIGH && switchRState == HIGH)
    {

      if (enc2Val < prevEnc2Val)
      {

        Joystick.pressButton(32);
        delay(testDelay);
        Joystick.releaseButton(32);

      }

      if (enc2Val > prevEnc2Val)
      {

        Joystick.pressButton(33);
        delay(testDelay);    
        Joystick.releaseButton(33);
        
      }

    }

    prevEnc2Val = enc2Val;

  }

  //read encoder 3 activity
  if (enc3Val != prevEnc3Val)
  {

    if (enc3Val < prevEnc3Val)
    {

      Joystick.pressButton(4);
      delay(1500);      
      Joystick.releaseButton(4);
      delay(25);

    }

    if (enc3Val > prevEnc3Val)
    {

      Joystick.pressButton(5);
      delay(1500);
      Joystick.releaseButton(5);
      delay(25);

    }

    prevEnc3Val = enc3Val;

  }
  //read encoder 4 activity
  if (enc4Val != prevEnc4Val)
  {

    if (enc4Val < prevEnc4Val)
    {

      Joystick.pressButton(6);
      delay(1500);
      Joystick.releaseButton(6);
      delay(25);

    }

    if (enc4Val > prevEnc4Val)
    {

      Joystick.pressButton(7);
      delay(1500);
      Joystick.releaseButton(7);
      delay(25);  
      
    }

    prevEnc4Val = enc4Val;

  }
  //read encoder 5 activity, button pressed changes based on mod switch position
  if (enc5Val != prevEnc5Val)
  {

    if (switchLState == LOW)
    {

      if (enc5Val < prevEnc5Val)
      {

        Joystick.pressButton(8);
        delay(testDelay);
        Joystick.releaseButton(8);
        delay(25);
        Joystick.pressButton(8);
        delay(testDelay);
        Joystick.releaseButton(8);
        delay(25);
        Joystick.pressButton(8);
        delay(testDelay);
        Joystick.releaseButton(8);
        delay(25);
        Joystick.pressButton(8);
        delay(testDelay);
        Joystick.releaseButton(8);

      }

      if (enc5Val > prevEnc5Val)
      {

        Joystick.pressButton(9);
        delay(testDelay);
        Joystick.releaseButton(9);
        delay(25);
        Joystick.pressButton(9);
        delay(testDelay);
        Joystick.releaseButton(9);
        delay(25);
        Joystick.pressButton(9);
        delay(testDelay);
        Joystick.releaseButton(9);
        delay(25);
        Joystick.pressButton(9);
        delay(testDelay);
        Joystick.releaseButton(9);
        
      }

    }

    //this encoder changes its assigned button based on how many times the pushbutton underneath it has been pressed allowing many buttons to be assigned to one encoder
    if (switchRState == LOW && counter == 0)
    {
      
      if (enc5Val < prevEnc5Val)
      {

        Joystick.pressButton(18);
        delay(testDelay);
        Joystick.releaseButton(18);

      }

      if (enc5Val > prevEnc5Val)
      {

        Joystick.pressButton(19);
        delay(testDelay);
        Joystick.releaseButton(19);
        
      }

    }

    if (switchRState == LOW && counter == 1)
    {
      
      if (enc5Val < prevEnc5Val)
      {

        Joystick.pressButton(26);
        delay(testDelay);
        Joystick.releaseButton(26);

      }

      if (enc5Val > prevEnc5Val)
      {

        Joystick.pressButton(27);
        delay(testDelay);
        Joystick.releaseButton(27);
        
      }

    }

    if (switchRState == LOW && counter == 2)
    {
      
      if (enc5Val < prevEnc5Val)
      {

        Joystick.pressButton(28);
        delay(testDelay);
        Joystick.releaseButton(28);

      }

      if (enc5Val > prevEnc5Val)
      {

        Joystick.pressButton(29);
        delay(testDelay);
        Joystick.releaseButton(29);
        
      }
      
    }

    if (switchRState == LOW && counter == 3)
    {
      
      if (enc5Val < prevEnc5Val)
      {

        Joystick.pressButton(30);
        delay(testDelay);
        Joystick.releaseButton(30);

      }

      if (enc5Val > prevEnc5Val)
      {

        Joystick.pressButton(31);
        delay(testDelay);
        Joystick.releaseButton(31);
        
      }

    }

    if (radioSwitchState == HIGH &&switchLState == HIGH && switchRState == HIGH)
    {

      if (enc5Val < prevEnc5Val)
      {

        Joystick.pressButton(20);
        delay(testDelay);
        Joystick.releaseButton(20);

      }

      if (enc5Val > prevEnc5Val)
      {

        Joystick.pressButton(21);
        delay(testDelay);
        Joystick.releaseButton(21);
        
      }

    }

    if (radioSwitchState == LOW && switchLState == HIGH && switchRState == HIGH)
    {

      if (enc5Val < prevEnc5Val)
      {

        Joystick.pressButton(34);
        delay(testDelay);
        Joystick.releaseButton(34);

      }

      if (enc5Val > prevEnc5Val)
      {

        Joystick.pressButton(35);
        delay(testDelay);
        Joystick.releaseButton(35);
        
      }

    }

    prevEnc5Val = enc5Val;

  }

  //gear switching and position checking
  if (gearUpState == LOW)
  {

    currentGearPos = 1;

  }

  if (gearDownState == LOW)
  {

    currentGearPos = 0;

  }

  if (currentGearPos != prevGearPos)
  {

    if (currentGearPos == 0)
    {

      Joystick.pressButton(10);
      delay(100);      
      Joystick.releaseButton(10);

    }

    if (currentGearPos == 1)
    {

      Joystick.pressButton(11);
      delay(100);
      Joystick.releaseButton(11);

    }

    prevGearPos = currentGearPos;

  }

  //push button checking, button pressed changes based on mod switch position
  if (switchLState == LOW)
  {

    if (button1State == LOW)
    {

      Joystick.pressButton(12);
      delay(100);
      Joystick.releaseButton(12);

    }

    if (button2State == LOW)
    {

      Joystick.pressButton(13);
      delay(100);
      Joystick.releaseButton(13);

    }

  }

  //when mod switch is in right position, second pushbutton increments counter used by encoder above
  if (switchRState == LOW)
  {

    if (button2State == LOW)
    {

      counter += 1;
      delay(150);

    }

    if (counter == 4)
    {

      counter = 0;

    }

  }

  if (switchLState == HIGH && switchRState == HIGH)
  {

    if (button1State == LOW)
    {

      Joystick.pressButton(24);
      delay(100);
      Joystick.releaseButton(24);

    }

    if (button2State == LOW)
    {

      Joystick.pressButton(25);
      delay(100);
      Joystick.releaseButton(25);

    }

  }

}
