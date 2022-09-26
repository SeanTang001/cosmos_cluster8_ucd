
#include <stdio.h>
#include <driverlib.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <stdlib.h>

/* LCD Screen libraries*/
#include <LCD_screen.h>
#include <LCD_screen_font.h>
#include <LCD_utilities.h>
#include <Screen_HX8353E.h>
#include <Terminal12e.h>
#include <Terminal6e.h>
#include <Terminal8e.h>
Screen_HX8353E myScreen;


void drawAccelData(void);


// your network name also called SSID
char ssid[] = "MOTO69F6";
// your network password
char password[] = "k6fzz48pmt";
// MQTTServer to use
char server[] = "io.adafruit.com";

volatile int distance[] = {0,0};
volatile int velocity[] = {0,0};
volatile int last_velocity[] = {0,0};
volatile int acceleration[] = {0,0};
const int buffer_length = 100;                        
volatile int xbuff[buffer_length];
volatile int ybuff[buffer_length];
volatile int counter = 0;
volatile int velocity_counter = 0;
int dt = 1;
volatile int xzero = 0;
volatile int yzero = 0;
volatile int flag = 0;
volatile bool IO_button = 0;

volatile uint8_t state = 0;
volatile uint32_t seconds = 0;
volatile uint32_t adaseconds = 0;
volatile bool xy;
volatile int xcount = 0;
volatile int ycount = 0;
volatile int gesturecount = 0;

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

void callback(char* topic, byte* payload, unsigned int length) {

  // Type Cast Input Bytes to Char
  char* str = (char*)payload;

  /* Check the Second Character of the char* pointer
   *  str[1] == 'N' ---> IO_button = ON 
   *  str[1] == 'F' ---> IO_button = OFF
   */

  Serial.println(String(str));

  if (str[1] == 'N'){
    xy = 1;
    }
  if (str[1] == 'F'){
    xy = 0;
    }


   /*  feed data */
   /* Implement logic to decide whether Xacc or Yacc is published here */
   
}

void LCD_init(){
  
  myScreen.begin();

  /* Let's make a title*/
  myScreen.gText(8, 5, "Acceleration Data");
  //myScreen.gText(8, 35, "Velocity Data");
  //myScreen.gText(8, 65, "Displacement Data");
  //myScreen.gText(8, 95, "Gesture Detected"); 
                  
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
}

void drawAccelData(void){
  myScreen.gText(20, 15,  "XA: " + String(ADC14_getResult(ADC_MEM0)) + String("     "));
  myScreen.gText(20, 25,  "YA: " + String(ADC14_getResult(ADC_MEM1)) + String("     "));
  myScreen.gText(20, 35,  "ZA: " + String(ADC14_getResult(ADC_MEM2)) + String("     "));
  myScreen.gText(40, 45,  "XV: " + String(velocity[0])  + String("     "));
  myScreen.gText(40, 55,  "YV: " + String(velocity[1]) + String("     "));
  myScreen.gText(40, 75,  "XD: " + String(distance[0]) + String("     "));
  myScreen.gText(40, 85,  "YD: " + String(distance[1]) + String("     "));
  myScreen.gText(40, 95,  "second: " + String(seconds) + String("     ")); 
  //myScreen.gText(40, 105,  "LXV: " + String(last_velocity[0]) + String("     "));
  //myScreen.gText(40, 115,  "LYV: " + String(last_velocity[1]) + String("     "));
  myScreen.gText(40, 105,  "state: " + String(state) + String("     "));
  //myScreen.gText(20, 105,  "x0o, y0: " + String(xzero) + String("   ") + String(yzero));
  
}

void reset(void){
      distance[0] = 0;
      distance[1] = 0;
      velocity[1] = 0;
      velocity[0] = 0;
      acceleration[0] = 0;
      acceleration[1] = 0;
      //flag = 0;
      state = 0;
      seconds = 0;
      counter = 0;
  }

void GPIO_Handler(void){

    uint_fast16_t interrupts;


    interrupts = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    
    GPIO_clearInterruptFlag(GPIO_PORT_P1, interrupts);
    
    if (interrupts & GPIO_PIN1)
    {
      IO_button = !IO_button;
      reset();

    }
  }

void ADC14_IRQHandler(void)
{
    uint64_t status;

    status = MAP_ADC14_getEnabledInterruptStatus();
    MAP_ADC14_clearInterruptFlag(status);

    /* ADC_MEM2 conversion completed */
    if(status & ADC_INT2)
    {

        if (flag <= 30){
          xzero = ADC14_getResult(ADC_MEM0);
          yzero = ADC14_getResult(ADC_MEM1);
          flag++;
          return;
          }
        // if ([DATA IS NOT BEING PUBLISHED TO SERVER])

        if (velocity_counter == 3 && (last_velocity[0] == velocity[0] || last_velocity[1] == velocity[1])){
        //if ((last_velocity[0] == velocity[0] || last_velocity[1] == velocity[1])){       
            if ((last_velocity[0] == velocity[0])&&(last_velocity[1] == velocity[1])){
              last_velocity[0] = velocity[0];
              last_velocity[1] = velocity[1];
              velocity[0] = 0;
              velocity[1] = 0;
            }            
            if (last_velocity[0] == velocity[0]){
              last_velocity[0] = velocity[0];
              last_velocity[1] = velocity[1];
              velocity[0] = 0;
              }
            if (last_velocity[1] == velocity[1]){
              last_velocity[0] = velocity[0];
              last_velocity[1] = velocity[1];
              velocity[1] = 0;
              }
            velocity_counter = 0;
          }
        else if (velocity_counter == 3){
            last_velocity[0] = velocity[0];       
            last_velocity[1] = velocity[1];
            velocity_counter = 0;
          }

        xbuff[counter] = ADC14_getResult(ADC_MEM0) - xzero;
        ybuff[counter] = ADC14_getResult(ADC_MEM1) - yzero;

        //myScreen.gText(10, 105,  "x, y: " + String(xbuff[counter]) + String("   ") + String(ybuff[counter])+String("        "));

        if (counter == buffer_length-1){
          velocity_counter++;
          /* Store ADC14 conversion results */

          acceleration[0] = 0;
          acceleration[1] = 0;

          /*
          
          for (int i=0;i<buffer_length;i++){
            acceleration[0] += xbuff[i];
            acceleration[1] += ybuff[i];

            if (xbuff[i] == 0){
              xcount++;
              }
            if (ybuff[i] == 0){
              ycount++;
              }
            }

          */

          for (int i=1;i<buffer_length;i++){
            if (xbuff[i] == 0){
              xcount++;
              }
            if (ybuff[i] == 0){
              ycount++;
              }

            acceleration[0] += (xbuff[i] + xbuff[i-1])/2;
            acceleration[1] += (ybuff[i] + ybuff[i-1])/2;
//            acceleration[0] += xbuff[i];
//            acceleration[1] += ybuff[i];
            }


          if (xcount >= buffer_length/2){
            acceleration[0] = 0;
            }

          if (ycount >= buffer_length/2){
            acceleration[1] = 0;
            }
          xcount = 0;
          ycount = 0;

          acceleration[0] = acceleration[0]/buffer_length; // X Accelerometer
          acceleration[1] = acceleration[1]/buffer_length; // Y Accelerometer

          velocity[0] = velocity[0] + acceleration[0] * dt; // X Velocity
          velocity[1] = velocity[1] + acceleration[1] * dt; // Y Velocity
        
          distance[0] = distance[0] + velocity[0] * dt; // X Velocity
          distance[1] = distance[1] + velocity[1] * dt; // Y Velocity

          counter = 0;


        if ((abs(distance[1]) >= 150) && (abs(distance[0]) <= 200)&& (state == 0)){
          state = 1;
          distance[0] = 0;
          distance[1] = 0;
          } 
          
        if (state==1){
          seconds++;
          }

        if ((abs(distance[0]) >= 150) && (abs(distance[1]) <= 200) && (state == 1)){
          if (seconds >= 30){
            state = 2;
            distance[0] = 0;
            distance[1] = 0;
            seconds = 0;
            }
          else{
            state = 0;
            distance[0] = 0;
            distance[1] = 0;
            seconds = 0;
            }
          }

        
        }

        counter++;

        //drawAccelData();

    
        /* Below implement logic for a smoothing filter for Acc readings, numReadings = 10 */
        
       /************************************************************************************/
        
        // Calculate Velocity Readings 
        
            

        // Calculate Displacement Readings
       
       
        /* IMPLEMENT GESTURE DETECTION STATE MACHINE */
       

        // Draw accelerometer data on display 
        }
       
    }

void setup() {

  WDT_A_hold(WDT_A_BASE);
  Serial.begin(115200);

  // Start Ethernet with the build in MAC Address
  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to Network named: ");
  // print the network name (SSID);
  Serial.println(ssid); 
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED) {
    // print dots while we wait to connect
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nYou're connected to the network");

      GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);

    GPIO_registerInterrupt(GPIO_PORT_P1, GPIO_Handler);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_enableInterrupt(GPIO_PORT_P1,GPIO_PIN1);
    Interrupt_enableInterrupt(INT_PORT1);
  
  /* Initialize all variables to 0 here  */


  /***************************************/


  /* Initialize LCD and make some titles */
  LCD_init();
  /* Initialize ADC *SEE THIS FUNCTION IMPORTANT LINES ARE HIGHLIGHTED* */
  ADC_init();
}

void loop() {
  /* Everything for interacting with the MQTT server is in this loop */
  /* When publishing to MQTT, you should not be interfacing with the ADC */
  drawAccelData();
  // attempt to connect to Wifi network:


  if (adaseconds%1 == 0){
    Interrupt_disableInterrupt(INT_ADC14);
    ADC14_disableInterrupt(ADC_INT2);  
      if(!client.connect("seantang001", "seantang001", "aio_TrZV90DHx4yIUzEkpYfbK2oJuVYL")) {
        Serial.println("Connection failed");
      } else {
        //Serial.println("Connection success");
        if(client.subscribe("seantang001/feeds/button1dashboard")) {
          //Serial.println("Subscription successful");
        }
      }

    client.poll();
    Interrupt_enableInterrupt(INT_ADC14);
    ADC14_enableInterrupt(ADC_INT2);
    }

  
  adaseconds++;
  if (adaseconds >= 20){
    Interrupt_disableInterrupt(INT_ADC14);
    ADC14_disableInterrupt(ADC_INT2);  
      if(!client.connect("seantang001", "seantang001", "aio_TrZV90DHx4yIUzEkpYfbK2oJuVYL")) {
        Serial.println("Connection failed");
      } else {
        Serial.println("Connection success");
        if(client.subscribe("seantang001/feeds/button1dashboard")) {
          Serial.println("Subscription successful");
        }
      }

    client.poll();

    if (xy == 1){
      Serial.println("xaxis");
      if(client.subscribe("seantang001/feeds/xaxis")) {
        Serial.println("Subscription successful");
      }

    if(client.publish("seantang001/feeds/xaxis", String(distance[0]).c_str())) {
       Serial.println("Publish success");
      } else {
        Serial.println("Publish failed");
      }
    }

    if (xy == 0){
      Serial.println("yaxis");
      if(client.subscribe("seantang001/feeds/yaxis")) {
        Serial.println("Subscription successful");
      }

    if(client.publish("seantang001/feeds/yaxis", String(distance[1]).c_str())) {
       Serial.println("Publish success");
      } else {
        Serial.println("Publish failed");
      }
    }

  if(client.subscribe("seantang001/feeds/gesture")) {
   Serial.println("Subscription successful");
   }
  Serial.println(String(state == 2));
  if(client.publish("seantang001/feeds/gesture", String(state == 2).c_str())) {
      Serial.println("Publish success");
    } else {
      Serial.println("Publish failed");
    }
  
  if (state == 2){
    reset();
    }
    
    Interrupt_enableInterrupt(INT_ADC14);
    ADC14_enableInterrupt(ADC_INT2);

    adaseconds = 0;

  }
  
  Serial.println(String(adaseconds));


    /*
  while (IO_button){

    Serial.print("X: " + String(acceleration[0]) + String("     "));
    Serial.print ("Y: " + String(acceleration[1]) + String("     "));
    Serial.println("Z: " + String(ADC14_getResult(ADC_MEM2)) + String("     "));
    Serial.print("X: " + String(xbuff[counter]) + String("     "));
    Serial.print ("Y: " + String(ybuff[counter]) + String("     "));
    Serial.println("C:" + String(counter));
    Serial.print("XV: " + String(velocity[0])  + String("     "));
    Serial.println("YV: " + String(velocity[1]) + String("     "));
    delay(50);

    
    }
    */
}
