#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const int pwmPin = 6;         // PWM control pin
bool serialConnected = false;  // Flag to check serial connection
unsigned long startTime = 0;   // Variable to store the last time data was received

unsigned long rangeStartTime = 0;
int lastKnownRange = -1;

int currentPwmValue = 0; // Store the current PWM value
unsigned long lastPwmUpdateTime = 0;
const int pwmUpdateInterval = 20; // Interval in milliseconds for PWM updates
const int pwmStepSize = 5; // Increase the step size to 5

void setup() {
    lcd.begin(16, 2);
    lcd.clear();
    Serial.begin(9600); // Start serial communication
    while (Serial.available() == 0) {
        delay(100);
    }
    lcd.setCursor(0, 0);
    lcd.print("Temperature");
    lcd.setCursor(0, 1);
    lcd.print("Monitor");
    delay(2000);
    lcd.clear();

    pinMode(pwmPin, OUTPUT);
    analogWrite(pwmPin, 0);  // Set initial PWM to 0 (fan off)
}

void loop() {
    if (Serial.available() > 0) {
        serialConnected = true;
        startTime = millis();
        String data = Serial.readStringUntil('\n'); // Read data until newline
        Serial.println(data); // Print the raw data to the Serial Monitor for debugging

        int commaIndex = data.indexOf(',');
        if (commaIndex > 0) {
            String cpuTempStr = data.substring(0, commaIndex); // Extract CPU temp
            String gpuTempStr = data.substring(commaIndex + 1); // Extract GPU temp

            int cpuTemp = cpuTempStr.toInt();

            // Display CPU temp on the first line
            lcd.setCursor(0, 0);
            lcd.print("CPU: " + cpuTempStr + "\337C");

            // Determine target PWM value and fan speed percentage based on CPU temperature
            int targetPwmValue = 0; // Default target PWM value
            int fanSpeedPercentage = 0; // Fan speed percentage
            int currentRange = -1;

            if (cpuTemp >= 0 && cpuTemp <= 41) {
                currentRange = 0;
                targetPwmValue = 102; // 0-41 degrees: 102 PWM
                fanSpeedPercentage = 25; // 25%
            } else if (cpuTemp >= 42 && cpuTemp <= 48) {
                currentRange = 1;
                targetPwmValue = 153; // 42-48 degrees: 153 PWM
                fanSpeedPercentage = 50; // 50%
            } else if (cpuTemp >= 49 && cpuTemp <= 55) {
                currentRange = 2;
                targetPwmValue = 204; // 49-55 degrees: 204 PWM
                fanSpeedPercentage = 75; // 75%
            } else if (cpuTemp > 55) {
                currentRange = 3;
                targetPwmValue = 255; // Above 55 degrees: 255 PWM
                fanSpeedPercentage = 100; // 100%
            }

            if (currentRange != lastKnownRange) {
                // If the range has changed, reset the timer
                rangeStartTime = millis();
                lastKnownRange = currentRange;
            }

            // Only update the PWM and fan speed if the temperature has been in the current range for at least 5 seconds
            if (millis() - rangeStartTime >= 2000) {
                // Gradually update the PWM value for the fan
                updatePwmValue(targetPwmValue);
                // Update the fan speed percentage
            }
            lcd.setCursor(0, 1);
            lcd.print("GPU: " + gpuTempStr + "\337C " + fanSpeedPercentage + "% ");
        }
    } else if (serialConnected && millis() - startTime > 5000) {
        lcd.setCursor(0, 0);
        lcd.print("No Data  ");
        lcd.setCursor(0, 1);
        lcd.print("           ");
    } else if (!serialConnected) {
        lcd.setCursor(0, 0);
        lcd.print("No Signal  ");
        lcd.setCursor(0, 1);
        lcd.print("           ");

        if (Serial.available() > 0) {
            serialConnected = true;
            startTime = millis();
        }
    }
}

void updatePwmValue(int targetPwmValue) {
    if (millis() - lastPwmUpdateTime >= pwmUpdateInterval) {
        if (currentPwmValue < targetPwmValue) {
            currentPwmValue += pwmStepSize;
            if (currentPwmValue > targetPwmValue) {
                currentPwmValue = targetPwmValue; // Ensure it doesn't overshoot
            }
        } else if (currentPwmValue > targetPwmValue) {
            currentPwmValue -= pwmStepSize;
            if (currentPwmValue < targetPwmValue) {
                currentPwmValue = targetPwmValue; // Ensure it doesn't overshoot
            }
        }
        analogWrite(pwmPin, currentPwmValue);
        lastPwmUpdateTime = millis();
    }
}