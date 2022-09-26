//***************************************************************************************
//  COSMOS Lab 2, Part 2 - An Implementation of Morse Code
//
//
//
//***************************************************************************************

#include <driverlib.h>
#include <stdio.h>

uint8_t count = 0;          // Keeps track of number of presses
uint32_t time_passed = 0;   // Measure how long SW1 is pressed
uint32_t threshold = 530000;     // Compare time_passed to this to see if input is dot or dash
uint8_t input [5];          // Store either dot or dash in this array
uint8_t k;
// Establish the Morse Code Reference Arrays
uint8_t zero  [5] = {1, 1, 1, 1, 1};
uint8_t one   [5] = {0, 1, 1, 1, 1};
uint8_t two   [5] = {0, 0, 1, 1, 1};
uint8_t three [5] = {0, 0, 0, 1, 1};
uint8_t four  [5] = {0, 0, 0, 0, 1};
uint8_t five  [5] = {0, 0, 0, 0, 0};
uint8_t six   [5] = {1, 0, 0, 0, 0};
uint8_t seven [5] = {1, 1, 0, 0, 0};
uint8_t eight [5] = {1, 1, 1, 0, 0};
uint8_t nine  [5] = {1, 1, 1, 1, 0};

uint8_t compare_array(uint8_t a[], uint8_t b[], uint8_t size){
    uint8_t i;
    for (i=0; i<size; i++){
      if (a[i] != b[i]){
        return 0;
        }
    }

    //return 1 when both arrays are equal
    return 1;
}

void setup()
{
    // Start Serial Terminal
    Serial.begin(9600);
    
    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
}

void loop()
{

    /*
     * Add logic for Morse Code Detection here (figure 3)
     * You must debounce SW1
     * Acknowledge the duration that SW1 is pressed and compare this value to threshold
     */
     

     // When Count = 5, we have recieved a valid input
     // The following code will output the morse code number detected

    time_passed = 0;

    while(GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1)){
      }

    for(int i=0;i<1000;i++){
    }

    while(!GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1)){
      time_passed++;
      }

     if(time_passed >= threshold){
        Serial.println("long");
        input[count] = 1;
      }
     else{
        Serial.println("short");
        input[count] = 0;
      }
      

     count++;
     if (count >= 5){
         count = 0;

         // Find Morse Code Value
         if (compare_array(input, zero, 5)){
             Serial.println("0");
         }
         else if (compare_array(input, one, 5)){
             Serial.println("1");
         }
         else if (compare_array(input, two, 5)){
             Serial.println("2");
         }
         else if (compare_array(input, three, 5)){
             Serial.println("3");
         }
         else if (compare_array(input, four, 5)){
             Serial.println("4");
         }
         else if (compare_array(input, five, 5)){
             Serial.println("5");
         }
         else if (compare_array(input, six, 5)){
             Serial.println("6");
         }
         else if (compare_array(input, seven, 5)){
             Serial.println("7");
         }
         else if (compare_array(input, eight, 5)){
             Serial.println("8");
         }
         else if (compare_array(input, nine, 5)){
             Serial.println("9");
         }
         else{
             Serial.println("Invalid Input!");
         }

       }

}
