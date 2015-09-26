////////////////////////////////////////////////////////////////////////////////////////////////////////
// Quadrature Trackball reader
// Author Gratefulfrog
// date 2015 09 26
//
// Usage:
//   1. Connect the Arduino pins to the trackball as per the indications below at "Arduino pins"
//   2. Set the resoltion to either (1) or (2); this will determine how much trackball roation is needed 
//      to increment the counters. At rsoluton 2, the counters increment twice as fast. (It is possible
//      to get 4x resolution as well, but that is left as an exercise ;-). Set it in the code at "RESOLUTION"
//   3. Upload the code to an Arduino - any type of Arduino should work, but this has only been tested
//      on an Uno.
//   4. Open the Serial Monitor.
//   5. Move the Trackball roller and watch the counter values displayed.
//   6. Enter any charcter in the monitor to reset the counter values to ZERO.
//    
// Notes:
//   Changing the resolution requires re-uploading the code.
///////////////////////////////////////////////////////////////////////////////////////////////////////////


// set resolution here: valid values (1) or (2)
#define RESOLUTION (1)

// these will store the number of ticks in x,y, and the last values too, for display purposes
int xCount=0,
    lxCount=0,
    yCount=0,
    lyCount=0;

// Arduino pins to be wired for GEC PLESSY KB34-400 trackball
int blue   = 2, // x1
    green  = 3, // y1
    yellow = 4, // x2
    white  = 5; // y2

// these are to fit with the algorithms, just for cosmetic pleasure
int x1 = blue,
    x2 = yellow,
    y1 = green,
    y2 = white;

// vector of pins to automate initiation.
int pins[] = {blue,green,yellow,white};


void x11(){
  // the interrupt handler for x1, resolution=1x
  // when x1 is RISING, then check the value of x2 to determinen the direction of rotation
  // if x2 is HIGH, then rotation is negative, otherwise it is positive
  xCount += digitalRead(x2) ? -1 : 1;  
}

void y11(){
  // the interrupt handler for pin y1, resolution=1x
  // same logic as x direction handler
  yCount += digitalRead(y2) ? -1 : 1;  
}

void x12(){
  // the interrupt handler for pin x1, resoluiton=2x
  // when x1 is either RISING OR FALLING, check both x1 and x2 values
  // if both are HIGH or both are LOW, then rotation is negative,
  // otherwise rotation is positive
  int value[][2] = {{-1 ,1}, // (0,0) and (0,1)
                    {1,-1}}; // (1,0) and (1,1)
  xCount += value[digitalRead(x1)][digitalRead(x2)];
}

void y12(){
  // the interrupt handler for y1, resolution=2x
  // same logic as x direction handler
  int value[][2] = {{-1 ,1}, // (0,0) and (0,1)
                    {1,-1}}; // (1,0) and (1,1)
  yCount += value[digitalRead(y1)][digitalRead(y2)];
}

void setup() {
  // init the serial comms for display purposes only
  Serial.begin(9600);
  // wait for the serial to be active
  while(!Serial);
  
  // set the pins to INPUT
  for (int i=0;i<4;i++){
    pinMode(pins[i],INPUT);
  }
  
  // if resolution is 1, count only on RISING, 
  // if resolution is 2 count on both RISING and FALLING
  int mode = RESOLUTION == 1 ? RISING : CHANGE;

  // create the appropriate function pointers depending on the resolution
  void (*isrx)() = RESOLUTION == 1 ? x11: x12;
  void (*isry)() = RESOLUTION == 1 ? y11: y12;
  
  // attach the interrupts
  attachInterrupt(digitalPinToInterrupt(blue),  isrx, mode);       
  attachInterrupt(digitalPinToInterrupt(green), isry, mode);       
  // reset the counters
  lxCount= xCount=lyCount=yCount=0;
}


void updateDisplay(){
  // print "(x,y)" to serial monitor and reset lxCount and lycount
  Serial.print('(');
  Serial.print(xCount);
  Serial.print(',');
  Serial.print(yCount);
  Serial.println(')');
  lyCount=yCount;
  lxCount=xCount;
}

void loop() {
  // if there is a serial input, then reset the x and y counters
  if (Serial.available() >0){
    char c = Serial.read();
    // any read resets counters.
    xCount=lxCount=yCount=lyCount=0;
  }
  // if the x or y count has changed, then display the (x,y) values
  if ((lxCount != xCount) || (lyCount != yCount)){
    updateDisplay();
  }
}
