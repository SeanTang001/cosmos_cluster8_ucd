/**
 * Simple program for drawing in color on the screen of the Educational BoosterPack MK II
 * @author Robert Christensen
 * @version 1.0
 * @see Examples > 9.EducationalBP_MKII > LCD_Joystick
*/

// Core librar
#include <driverlib.h>
// Include application, user and local libraries
#include "SPI.h"
#include "Screen_HX8353E.h"
Screen_HX8353E myScreen;

volatile uint8_t timer_flag;
volatile int32_t x=5, y=128, oldx, oldy;
volatile int32_t vx=1, vy=1;
const int32_t radius=3;
volatile int xdis, ydis, zdis;
volatile int xzero=515, yzero=515;
volatile int points;
volatile int multiple = 1;

void GPIO_Handler(void){

    uint_fast16_t interrupts;


    interrupts = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    
    GPIO_clearInterruptFlag(GPIO_PORT_P1, interrupts);
    
    if (interrupts & GPIO_PIN1)
    {
      reset();

    }
  }


void eraseBall(int32_t x, int32_t y, int32_t radius) {
    // draw over old ball with background color to erase it
    myScreen.circle(x, y , radius, blackColour);
}

void drawBall(int32_t x, int32_t y, int32_t radius) {
    // draw ball with default foreground color
    myScreen.circle(x, y , radius, whiteColour);
}

int16_t preprocess(uint16_t x){
  if (x < 315){
    return 315;
    }
  if (x>715){
    return 715;
    }
  return x;
  }

void reset(void){
  points = 0;
  multiple = 1;
  }


void drawData(void){
  //myScreen.gText(20, 15,  "XA: " + String(ADC14_getResult(ADC_MEM0)) + String("     "));
  //myScreen.gText(20, 25,  "YA: " + String(ADC14_getResult(ADC_MEM1)) + String("     "));
  //myScreen.gText(20, 35,  "ZA: " + String(ADC14_getResult(ADC_MEM2)) + String("     "));
  myScreen.gText(20, 35, "Points: " + String(points) + String("  "));
  myScreen.gText(20, 45,  "x: " + String(x) + String(" "));
  myScreen.gText(20, 55,  "y: " + String(y) + String(" "));
  myScreen.gText(20, 65,  "r: " + String(rand()%128) + String(" "));
  //myScreen.gText(40, 105,  "LXV: " + String(last_velocity[0]) + String("     "));
  //myScreen.gText(40, 115,  "LYV: " + String(last_velocity[1]) + String("     "));
  //myScreen.gText(40, 105,  "state: " + String(state) + String("     "));
  //myScreen.gText(20, 105,  "x0o, y0: " + String(xzero) + String("   ") + String(yzero));
  
}

void ADC14_IRQHandler(void)
{
    uint64_t status;

    status = MAP_ADC14_getEnabledInterruptStatus();
    MAP_ADC14_clearInterruptFlag(status);

    /* ADC_MEM2 conversion completed */
    if(status & ADC_INT2)
    {

      xdis = preprocess((uint16_t)ADC14_getResult(ADC_MEM0)) - xzero;
      ydis = preprocess((uint16_t)ADC14_getResult(ADC_MEM1)) - yzero;

      if(xdis < 0){
        vx = -1;
        }
      else if (xdis == 0){
        vx = 0;
        }
      else{
        vx = 1;
        }
      if (ydis < 0){
        vy = 1;
      }
      else if (ydis == 0){
        vy = 0;
        }
      else{
        vy = -1;
        }

      
    }
       
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

  ADC14_configureConversionMemory(ADC_MEM3, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A26, 0);

  ADC14_setResolution(ADC_10BIT); //IMPORTANT -> This seemed to give me the least noisey values (8 bit res was too small though)

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

      GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);

    GPIO_registerInterrupt(GPIO_PORT_P1, GPIO_Handler);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_enableInterrupt(GPIO_PORT_P1,GPIO_PIN1);
    Interrupt_enableInterrupt(INT_PORT1);

  
}


void setup() {
  

    WDT_A_hold(WDT_A_BASE);

    /*

    SysTick_registerInterrupt(SysTick_Handler);
    SysTick_setPeriod(150000);
    SysTick->VAL = 0;
    SysTick_enableModule();
    SysTick_enableInterrupt();


    */
    
    drawBall(x,y,radius);
    
    //Initialize myScreen
    myScreen.begin();
    
    // Reset Screen
    myScreen.clear(blackColour);

    drawBall(64, 64, radius);

    ADC_init();
}

void loop() {
  
  drawData();

  oldx = x; 
  oldy = y;

  eraseBall(oldx, oldy, radius);

  x = x+vx*multiple;
  y = y+vy*multiple;


  if (x >= 120){
    x = 120;
    }
  if (x <= 5){
    x = 5;
    }


  if (y >= 120){
    y = 120;
    }
  if (y <= 5){
    y = 5;
    }

  if ((oldx >= 60 && oldx <= 67)&&(oldy >= 60 && oldy <= 67)){
    drawBall(64, 64 , radius);
    points++;
    //x = (rand() % 2) * 120 + 5;
    //y = (rand() % 2) * 120 + 5;    
    x = (rand() % 128);
    y = (rand() % 128);
    multiple++;
    }


  drawBall(x, y, radius);

  if (points == 8){
    multiple = 1;
    points = 0;
    }


    /*  Implement the bouncing ball logic here
     *  Use these variable defined above:
     *  
     *  int32    _t x=50, y=70, oldx, oldy;
     *  int32_t vx=1, vy=-1;
     *  const int32_t radius=3;
     *  
     *  Important Functions:
     *  eraseBall(x, y, radius);
     *  drawBall(x, y, radius);
     */



}
