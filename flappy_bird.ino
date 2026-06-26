/**************************************************************************
 * @file flappy_bird.ino
 * @brief Self-contained Flappy Bird game engine for ESP32-S3 and SSD1306
 * * Hardware Configuration:
 * - MCU: ESP32-S3
 * - Display: SSD1306 128x32 OLED Panel
 * - SDA: GPIO 47
 * - SCL: GPIO 48
 * * Control Interface:
 * - Transmission of any character packet over USB Serial triggers a flap event.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define I2C_SDA 47
#define I2C_SCL 48

#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Physics Constants
const float GRAVITY = 0.35;
const float FLAP_STRENGTH = -2.2;
const int PIPE_SPEED = 2;
const int PIPE_GAP = 14;   
const int PIPE_WIDTH = 6;

// Entity Tracking Profiles
float birdY = 16.0;
float birdVelocity = 0.0;
const int BIRD_X = 20;
const int BIRD_RADIUS = 2;

int pipeX = 128;
int gapY = 10;             
int score = 0;
bool isGameOver = true;

void resetGame() {
  birdY = 16.0;
  birdVelocity = 0.0;
  pipeX = 128;
  gapY = random(6, 26);    
  score = 0;
  isGameOver = false;
}

void setup() {
  Serial.begin(115200);

  Wire.begin(I2C_SDA, I2C_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("ERROR: SSD1306 allocation failed."));
    for(;;); 
  }

  randomSeed(analogRead(0)); 

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(18, 4);
  display.println(F("FLAPPY BIRD S3"));
  display.setCursor(10, 18);
  display.println(F("Send text to Flap!"));
  display.display();
}

void loop() {
  bool actionTriggered = false;
  
  if (Serial.available() > 0) {
    while (Serial.available() > 0) {
      Serial.read(); 
    }
    actionTriggered = true;
  }

  if (isGameOver) {
    if (actionTriggered) {
      resetGame();
    }
    return; 
  }

  // Physics Computations
  if (actionTriggered) {
    birdVelocity = FLAP_STRENGTH; 
  }

  birdVelocity += GRAVITY;       
  birdY += birdVelocity;

  pipeX -= PIPE_SPEED;
  if (pipeX < -PIPE_WIDTH) {
    pipeX = 128;                 
    gapY = random(6, 26);        
    score++;
  }

  // Collision Verification Matrix
  if (birdY - BIRD_RADIUS < 0 || birdY + BIRD_RADIUS > SCREEN_HEIGHT) {
    isGameOver = true;
  }

  if (pipeX < BIRD_X + BIRD_RADIUS && pipeX + PIPE_WIDTH > BIRD_X - BIRD_RADIUS) {
    int upperPipeLimit = gapY - (PIPE_GAP / 2);
    int lowerPipeLimit = gapY + (PIPE_GAP / 2);
    
    if (birdY - BIRD_RADIUS < upperPipeLimit || birdY + BIRD_RADIUS > lowerPipeLimit) {
      isGameOver = true;
    }
  }

  // Rendering Pipeline
  display.clearDisplay();

  if (!isGameOver) {
    int upperPipeHeight = gapY - (PIPE_GAP / 2);
    int lowerPipeTopY = gapY + (PIPE_GAP / 2);
    
    display.fillRect(pipeX, 0, PIPE_WIDTH, upperPipeHeight, SSD1306_WHITE);
    display.fillRect(pipeX, lowerPipeTopY, PIPE_WIDTH, SCREEN_HEIGHT - lowerPipeTopY, SSD1306_WHITE);
    display.fillCircle(BIRD_X, (int)birdY, BIRD_RADIUS, SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(2, 2);
    display.print(score);
  } else {
    display.setTextSize(1);
    display.setCursor(35, 2);
    display.print(F("GAME OVER"));
    
    display.setCursor(38, 14);
    display.print(F("SCORE: "));
    display.print(score);

    display.setCursor(14, 24);
    display.print(F("Send msg to retry"));
  }

  display.display();
  delay(35); 
}