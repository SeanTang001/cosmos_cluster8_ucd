#include <driverlib.h>

uint8_t ped = 0;

uint8_t delay_and_poll(uint8_t seconds){
    uint8_t press=0;
    uint32_t i, j, k;

    for(i=0; i <seconds; i++){
        for (j=0; j<20; j++){
                for (k=0; k<15000; k++){
                    // Delay loop using k of about 1/20 of a second
                    // Read SW1 and set press to 1 if SW1 pressed
              if (!GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1)){
                press = 1;
              }
            }
        }
    }
    return press;
}

void RGB_output(uint8_t count) {
      /*
       * Use this function to update the LEDs for both ped and car light
       * count == 0 ---> Car light green, ped light red 
       * count == 1 ---> Car light yellow, ped light red
       * count == 2 ---> Car light red, ped light greeen
       */
   
  GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
  GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
  GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
  GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
  if (count == 0){
    //car
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
    }
  if (count == 1){
    //car
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
    //ped
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
    }
  if (count == 2){
    //car
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
    //ped
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN1);
    }       
}

void setup()
{
    
    
    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    // Set P1.0 to output direction
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);

    //GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);

    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);

}

void loop ()
{
    RGB_output(0);
     /*
      * Implement the the flow chart in figure 1 here
      * This function will loop infinetly
      */
    while(!delay_and_poll(8)){
      }
    
    RGB_output(1);

    ped = delay_and_poll(4);

    RGB_output(2);
    
    ped = delay_and_poll(8);

    


}
