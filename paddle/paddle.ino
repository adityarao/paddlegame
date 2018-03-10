/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 by Aditya Rao
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

 #include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
 #include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
 SSD1306  display(0x3c, D2, D1); // SDA, SCL

// Left button and right button, change it accordingly 
#define LEFT_BUTTON D6
#define RIGHT_BUTTON D7

// Speed is 20ms 
#define SPEED 20

// Radius of the ball
#define RADIUS 5

// width of the paddle
#define PADDLE_WIDTH 16
#define PADDLE_HEIGHT 3

// how many pixels do I move the paddle ?
#define PADDLE_MOVEMENT 16 

int x = DISPLAY_WIDTH/2, y = DISPLAY_HEIGHT/2;
int xoffset = 1, yoffset = 1;
int radius = 5;
int speed = 20;

// score 
signed int score = 0;

int leftbuttonval = 0, rightbuttonval = 0; 
int prev_leftbuttonval = 0, prev_rightbuttonval = 0; 

int xpaddle = DISPLAY_WIDTH/2 - PADDLE_WIDTH/2;
int ypaddle = DISPLAY_HEIGHT - PADDLE_HEIGHT - 1;

int t_speedGap = 0; 

void setup() {
  display.init();

  display.flipScreenVertically();
  display.setContrast(255);

  pinMode(D5, OUTPUT); // sound buzzer is attached to D5, change accordingly 
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);

  t_speedGap = millis();

  // settings to change the score on the top. 
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);

  Serial.begin(115200);
}

void loop() { 

  // get the previous value of the button 
  prev_rightbuttonval = rightbuttonval;
  prev_leftbuttonval = leftbuttonval;

  // caputre the latest. 
  leftbuttonval = digitalRead(LEFT_BUTTON);
  rightbuttonval = digitalRead(RIGHT_BUTTON);

  // Just checking if the left button was released, not recongnizing long press
  // need to fix that to make the game more interactive.
  if(prev_leftbuttonval == 0 && leftbuttonval == 1) { 
    Serial.println("RELEASED LEFT BUTTON");

    // move the paddle left 
    xpaddle = xpaddle - PADDLE_MOVEMENT;
    if(xpaddle < 1)
      xpaddle = 0;
  }

  // Check of right pushbutton release
  if(prev_rightbuttonval == 0 && rightbuttonval == 1) { 
    Serial.println("RELEASED RIGHT BUTTON");

    // move paddle right
    xpaddle += PADDLE_MOVEMENT;

    // prevent slipping off the screen !
    if(xpaddle > DISPLAY_WIDTH - PADDLE_WIDTH)
      xpaddle -= PADDLE_MOVEMENT;
  }

  // come back only if we are ready to refresh (determined with speed variable)
  if(millis() - t_speedGap > speed) {

    display.clear();

    t_speedGap = millis();

    // this is the ball ! 
    display.fillCircle(x, y, RADIUS);

    // put this in a fn, but just draws the 3 borders 
    display.drawLine(0, 0, DISPLAY_WIDTH-1, 0);
    display.drawLine(0, 0, 0, DISPLAY_HEIGHT-1);
    display.drawLine(DISPLAY_WIDTH-1, 0, DISPLAY_WIDTH-1, DISPLAY_HEIGHT-1);

    // draw the paddle 
    drawPaddle(xpaddle, ypaddle, PADDLE_WIDTH, PADDLE_HEIGHT);

    //Check for conditions here 
    // Display the score only if the ball is not close to  
    // where the score is being displayed ! 
    // display the score if the ball is 1/3rd away
    if(y >= DISPLAY_HEIGHT/3) {
      String scoreText = "Score: ";
      scoreText += String(score);
      display.drawString(DISPLAY_WIDTH/2, 0, scoreText);  
    }

    // paint the screen 
    display.display();
    
    // move left if hitting the right border 
    if(x > DISPLAY_WIDTH - RADIUS - 1) {
      xoffset = -1;
      beep(1);
    }

    // move right if hitting left border
    if(x < RADIUS + 1) {
      xoffset = +1;
      beep(1);
    }

    // hitting the bottom paddle (if availables)
    if(y > DISPLAY_HEIGHT - RADIUS - 1) {
      yoffset = -1;

      // check to see if it hit the paddle !
      if(x >= xpaddle && x <= (xpaddle + PADDLE_WIDTH)) {
        beep(10);
        score++;
      } else {
        score--;
        beep(4);
        delay(20);
        beep(4);
      }
    }

    // hitting the top 
    if(y < RADIUS + 1){
      yoffset = +1;
      beep(1);
    }

    // move the ball !
    x += xoffset;
    y += yoffset;   

    //Serial.print("Leftbutton val : ");
    //Serial.println(leftbuttonval);

    //Serial.print("Rightbutton val : ");
    //Serial.println(rightbuttonval);    
  }
}

// simple beep function 
void beep(int length)
{
  digitalWrite(D5, HIGH);
  delay(length);
  digitalWrite(D5, LOW);
}

// drawing the paddle function 
void drawPaddle(int x, int y, int width, int height) 
{
  display.fillRect(x, y, width, height);
}