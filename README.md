# magnetic-discs
Software tools, 3D printing files and hardware specs for building a Magnetic Disc. 

Hardware used: 
Esp32 TTGO V7 1.3 Mini
MPU6050
MLX90393 

Notes: 
DO Not daisy chain the I2C sensors, MPU6050 has limited operativity in this mode. Most reliable approach is to connect each sensor separately to the board.

Esp32 I2C default pins do not work correctly. Inside the Esp32 board manager library find header file for the specified board model (mine is /Users/nicola/Library/Arduino15/packages/esp32/hardware/esp32/2.0.6/variants/ttgo-t7-v13-mini32), and change default SDA pin to 19 and SCL pin to 23.




