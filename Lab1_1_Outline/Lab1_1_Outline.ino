#include <driverlib.h>

uint8_t count = 0;

void RGB_output(uint8_t count){
  // Function to output to RGB LED based on count value between 0 and 7

  GPIO_setAsOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
  
  if (count%4 == 1){
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
    }
  if (count%2 == 1){
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
    }
  if (count%1 == 1){
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
    }
    
}

void setup() {

  // Stop watchdog timer
  WDT_A_hold(WDT_A_BASE);

  GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
}

void loop() {

  // This code will loop indefinetly
  RGB_output(count);

  count++;
  if (count > 7){
    count = 0;
  }

  // Delay 1000 ms
  delay(1000);
  
}
