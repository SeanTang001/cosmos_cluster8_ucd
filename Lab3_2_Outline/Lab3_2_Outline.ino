//***************************************************************************************
//  COSMOS Lab 3, Part 2 - Gesture Detection
//  Using Interrupts and ADC14
//  Implementing a simple state machine
//
//***************************************************************************************
#include <stdio.h>
#include <driverlib.h>

/* LCD Screen libraries*/
#include <LCD_screen.h>
#include <LCD_screen_font.h>
#include <LCD_utilities.h>
#include <Screen_HX8353E.h>
#include <Terminal12e.h>
#include <Terminal6e.h>
#include <Terminal8e.h>
Screen_HX8353E myScreen;

// Volatile value to poll ADC from interrupt
volatile uint32_t resultsBuffer[3];
volatile uint32_t seconds;
volatile uint8_t hundreds;
volatile uint8_t state_detected;
volatile uint8_t gesture_detected;
volatile uint8_t flag;
volatile uint8_t current_state; // 2 for upside, 1 for rightside, 0 for checking

void drawAccelData(void);

void LCD_init(){
  
  myScreen.begin();

  // Find the LCD Screen Size
  Serial.print("X Screen Size: ");
  Serial.println(myScreen.screenSizeX());
  Serial.print("Y Screen Size: ");
  Serial.println(myScreen.screenSizeY());

  /* Let's make a title*/
  myScreen.gText(8, 20, "Accelerometer Data");
                  
}

void ADC_init(){
  /* Configures Pin 4.0, 4.2, and 6.1 ad ADC inputs */
  // ACC Z = P4.2
  // ACC Y = P4.0
  // ACC X = P6.1
  GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN0 | GPIO_PIN2, GPIO_TERTIARY_MODULE_FUNCTION);
  GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN1, GPIO_TERTIARY_MODULE_FUNCTION);

  ADC14_registerInterrupt(ADC14_IRQHandler);

  /* Initializing ADC (ADCOSC/64/8) */
  ADC14_enableModule();
  ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_64, ADC_DIVIDER_8, 0);

  /* Configuring ADC Memory (ADC_MEM0 - ADC_MEM2 (A11, A13, A14)  with no repeat)
   * with internal 2.5v reference */
  ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM2, true);
  ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A14, ADC_NONDIFFERENTIAL_INPUTS);

  ADC14_configureConversionMemory(ADC_MEM1, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A13, ADC_NONDIFFERENTIAL_INPUTS);

  ADC14_configureConversionMemory(ADC_MEM2, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A11, ADC_NONDIFFERENTIAL_INPUTS);

  /* Enabling the interrupt when a conversion on channel 2 (end of sequence)
   *  is complete and enabling conversions */
  ADC14_enableInterrupt(ADC_INT2);

  /* Enabling Interrupts */
  Interrupt_enableInterrupt(INT_ADC14);
  Interrupt_enableMaster();

  /* Setting up the sample timer to automatically step through the sequence
   * convert.*/
  ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

  /* Triggering the start of the sample */
  ADC14_enableConversion();
  ADC14_toggleConversionTrigger();

}

void drawAccelData(void){
  myScreen.gText(40, 50,  "X: " + String(resultsBuffer[0]));
  myScreen.gText(40, 60,  "Y: " + String(resultsBuffer[1]));
  myScreen.gText(40, 70,  "Z: " + String(resultsBuffer[2]));
  myScreen.gText(20, 90,  "State: " + String(current_state));
  myScreen.gText(10, 100,  "Detected State: " + String(state_detected));
  myScreen.gText(0, 110,  "gesture amount: " + String(gesture_detected));  
  myScreen.gText(20, 120,  "seconds: " + String(seconds));  

  /*
   * Add new Text on LCD "G"
   * This will increment each time a gesture is detected
   */
  
}

void ADC14_IRQHandler(void)
{

    uint64_t status;


    status = MAP_ADC14_getEnabledInterruptStatus();
    MAP_ADC14_clearInterruptFlag(status);

    /* ADC_MEM2 conversion completed */
    if(status & ADC_INT2)
    {
        /* Store ADC14 conversion results */
        resultsBuffer[0] = ADC14_getResult(ADC_MEM0);
        resultsBuffer[1] = ADC14_getResult(ADC_MEM1);
        resultsBuffer[2] = ADC14_getResult(ADC_MEM2);

        /*
         * Draw accelerometer data on display 
         */

        drawAccelData();

        /*
         * Implement Gesture Detection State Machine Here!
         */

        if ((resultsBuffer[2] >= 10000)&&(resultsBuffer[2] <= 12000)){
          state_detected = 0;
          }
        if ((resultsBuffer[2] >= 4000)&&(resultsBuffer[2] <= 6000)){
          state_detected = 1;
          }


        if (1 == current_state){
          hundreds++;
          if (hundreds == 10){
            seconds++;
            hundreds = 0;
            }
          }
        else if (state_detected != current_state){
          hundreds++;
          if (hundreds == 10){
            seconds++;
            hundreds = 0;
            }
          }


         if (seconds >= 2){
          hundreds=0;
          seconds=0;
          if ((current_state == 1)&&(state_detected==1)){
            flag = 0;
            }
          else if ((current_state == 1)&&(flag)){
            current_state = 2;
            gesture_detected++;
            }
          else{
            current_state = state_detected;
            flag = 1;
            }
          }

          
    }
}

void setup() {

  WDT_A_hold(WDT_A_BASE);

  Serial.begin(115200);


  current_state = 2;
  state_detected = 0;
  hundreds = 0;
  seconds = 0;
  flag = 0;
  
  LCD_init();
  ADC_init();

  
}

void loop() {
  // put your main code here, to run repeatedly: 
  
}
