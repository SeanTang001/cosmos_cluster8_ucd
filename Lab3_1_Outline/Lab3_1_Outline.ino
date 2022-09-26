//***************************************************************************************
//  COSMOS Lab 3, Part 1 - Systick Timer
//  Using interrupts to implement a stopwatch
//
//
//***************************************************************************************

#include <systick.h>
#include <gpio.h>
#include <interrupt.h>


volatile uint8_t seconds;
volatile uint8_t hundredths;
volatile uint8_t count;

uint16_t i;

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

// Built-in Interrupt handler function name
void SysTick_Handler (void){

    RGB_output(count);

    hundredths++;
    if (hundredths == 100){
      seconds++;
      count++;
      hundredths = 0;
      }

    if (count == 7){
      count = 0;
      }

    /*
     * Implement stopwach 
     */
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  // Push button 1 is an input (start button)
  GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);

  // Push button 2 is an input (stop button)
  GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);

  // Set RGB Led as output and set low
  GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
  GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);

  SysTick_registerInterrupt(SysTick_Handler);
  SysTick_setPeriod(480000);
  SysTick->VAL = 0;
  // Disable Interrupt for now
  SysTick_disableInterrupt();
  
}

void loop() {

  Serial.println("Press S1 to Start, Press S2 to Stop");

  while (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1)){
    }

  count = 0;
  seconds = 0;
  hundredths = 0;

  SysTick_enableInterrupt();
  
  /*
   * Counting has started - set LED low, reset all variables
   * You must properly reset the Systick Registers / Enable Interrupts
   */


  /*
   * Poll SW2 Here
   * You must debounce this switch
   */
   
  while (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4)){
    
    }  

  SysTick_disableInterrupt();

  Serial.print(seconds);
  Serial.print(",");
  Serial.print(hundredths);
  Serial.print("\n");


}
