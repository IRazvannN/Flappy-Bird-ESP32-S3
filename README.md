# ESP32-S3 Flappy Bird Engine

A lightweight, hardware-accelerated Flappy Bird game logic loop written in C++ for the ESP32-S3 microcontroller and optimized for a 128x32 pixel SSD1306 OLED display.

## Hardware Demonstration
Below is an operational demonstration of the collision matrix and physics loops rendering on the physical hardware module:



## Hardware Specifications
- **Microcontroller:** ESP32-S3
- **Display Module:** SSD1306 128x32 OLED Panel
- **I2C Data Line (SDA):** GPIO 47
- **I2C Clock Line (SCL):** GPIO 48
- **Input System:** USB Hardware CDC Serial Protocol

## Architectural Mechanics

### Physics Subsystem
The engine computes single-axis kinematics calculations at a fixed frame delay of 35 milliseconds. Acceleration transformations run on every loop iterations using separate gravity vector accumulations:

$$v_{t} = v_{t-1} + a_{\text{gravity}}$$
$$y_{t} = y_{t-1} + v_{t}$$

A external serial input event overrides the velocity register with a hardcoded negative impulse factor ($V_{\text{flap}} = -2.2$).

### Collision Matrix
Boundary validations enforce coordinate envelope intersections against static viewport boundaries and scrolling obstacle meshes:

```text
         [Upper Pipe Block] -> (0 <= y <= gapY - GAP/2)
  ---------------------------------------------------
   (BirdX, BirdY) O -> [Collision Envelope Bounds]
  ---------------------------------------------------
         [Lower Pipe Block] -> (gapY + GAP/2 <= y <= SCREEN_HEIGHT)