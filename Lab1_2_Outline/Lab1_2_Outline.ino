

#include <driverlib.h>

uint8_t count = 0;

void RGB_output(uint8_t count){
  // Function to output to RGB LED based on count value between 0 and 7

  GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
  
  if (count%2 == 1){
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
    }
   count = count/2;
  if (count%2 == 1){
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
    }
   count = count/2;
  if (count%2 == 1){
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
    }
    
}

void setup () {

    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    // Set P1.0 to output direction
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
}

void loop () {
  
    /* 
     *  Wait here until Sw1 on P1.1 is pressed
     *  This should be a tight polling loop
     *  The processor will execute the loop until SW1 is pressed
     *  Recall that SW1 is active low
     */

    while(GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1)){
      delay(50);
     }
    count++;
    RGB_output(count);      

    while(!GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1)){
      delay(50);
     }


}
