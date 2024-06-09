#include "I2Cdev.h"
#include "MPU6050.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 accelgyro;
int16_t gx, gy, gy_old, gz;

const float alpha = 0.1;  // Smoothing factor for the EMA, between 0 and 1
float gxFiltered = 0;
float gyFiltered = 0;
float gzFiltered = 0;

#define DIR_PIN_1 12  // D2 on ESP8266
#define STP_PIN_1 14  // D1 on ESP8266
#define DIR_PIN_2 13
#define STP_PIN_2 15

int new_delay;
int speed = 0;
float set_step;
float e=2.718;
float ln2=0.693;

void setup() {
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif
    
  Serial.begin(115200);

  // initialize device
//  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();

  // verify connection
//  Serial.println("Testing device connections...");
//  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  // Set the pin modes
  pinMode(DIR_PIN_1, OUTPUT);
  pinMode(DIR_PIN_2, OUTPUT);
  pinMode(STP_PIN_1, OUTPUT);
  pinMode(STP_PIN_2, OUTPUT);

  // Initialize pins to a low state
  digitalWrite(DIR_PIN_1, LOW);
  digitalWrite(DIR_PIN_2, LOW);
  digitalWrite(STP_PIN_1, LOW);
  digitalWrite(STP_PIN_2, LOW);
}

void stepperMotorControl(int steps, bool direction, int delayMicros) {
  // Set direction
  digitalWrite(DIR_PIN_1, !direction);
  digitalWrite(DIR_PIN_2, direction);
  new_delay = 0;
  speed = 0;
  set_step = 10.0;
  
  for (int step = 0; step < steps; step++) {
    accelgyro.getRotation(&gx, &gy, &gz);
    gy = gy - 135; // Offset
    gyFiltered = alpha * gy + (1 - alpha) * gyFiltered;

    // Create a pulse
    if (step > 1 && step <= set_step){    
      // Linear
      //new_delay = delayMicros / (step/set_step) ;
      
      // Log
      //new_delay = delayMicros / (log(step) / log(set_step));

      // Quadratic
      //new_delay = delayMicros / (pow(step , 2) / pow(set_step, 2));

      // Exponential 
      //new_delay =  delayMicros / (pow(e, step * ln2 / set_step) - 1);

      // Sin
      //new_delay = delayMicros / sin(1.571 * step/set_step);

      //Cubic 
      //new_delay = delayMicros / (pow(step, 3) / pow(set_step, 3));

      // Sigmoid
      new_delay = delayMicros / (1 / (1 + pow(e, -(step - (set_step/2)))));

      // Speed (RPM)
      speed = 60 / (200 * new_delay * pow(10, -6));
    }
    
    digitalWrite(STP_PIN_1, HIGH);
    digitalWrite(STP_PIN_2, HIGH);
    delayMicroseconds(new_delay);
    digitalWrite(STP_PIN_1, LOW);
    digitalWrite(STP_PIN_2, LOW);
    delayMicroseconds(new_delay);

//    Serial.print("Gyro (Deg/Sec) | Gyro_Filtered (Deg/Sec) | Speed (RPM):");
    Serial.print(gy); Serial.print(",\t");
    Serial.print(gyFiltered); Serial.print(",\t");
    Serial.println(speed);

  }
}

void loop() {
  // Rotate clockwise
//  Serial.println("Rotating clockwise");
  stepperMotorControl(300, HIGH, 2000); // Initial delay in microseconds

  // Wait for 2 seconds
  delay(1000);

  // Rotate counterclockwise
//  Serial.println("Rotating counterclockwise");
  stepperMotorControl(300, LOW, 2000); // Initial delay in microseconds

  // Wait for 2 seconds
  delay(1000);
}
