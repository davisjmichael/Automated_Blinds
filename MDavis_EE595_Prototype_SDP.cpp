/*
  EE595 RBlinds Prototoype Code
*/
//////////////////////////////////Included Packages ///////////////////////////////////////////////////////
#include <FastLED.h>
#include <LiquidCrystal.h>
#include <IRremote.h>
#include <Stepper.h>

//////////////////////////////////State Definition ///////////////////////////////////////////////////////

      //Block B
        #define INITIAL_STATE 0
        #define ROLLUP_BLINDS 1
        #define ROLLDOWN_BLINDS 2
        #define ERROR_STATE 3
        #define MANUAL_MODE 4
        #define AUTO_MODE 5
        #define OPEN_BLINDS 6
        #define CLOSE_BLINDS 7
        #define LIGHT_SENSOR 8
        #define WIND_SENSOR 9

        #define LED_PIN     14  //need to change
        #define NUM_LEDS    10

        uint8_t fsm_state = MANUAL_MODE;



//////////////////////////////////PIN Assignments ///////////////////////////////////////////////////////

        //BlockC 
          int buttonUp = 2;
          int buttonDown = 3;
          int buttonStateUp = 0;
          int buttonStateDown = 0;
          int M1step = 4;    //set the variable to the digital out pin thats connected to Motor 1 step signal - white
          int M1sleep = 5;     //set the variable to the digital out pin thats connected to Motor 1 sleep signal - red
          int M1direction = 6;     //set the variable to the digital out pin thats connected to Motor 1 direction signal - green

          const int stepsPerRevolution = 1100;
          Stepper myStepper(stepsPerRevolution, 18, 20, 19, 21);

        //Block D
          #define POWER 0x10EFD827 //auto Mode or manual mode
          #define A 0x10EFF807    //not used
          #define B 0x10EF7887    //not used
          #define C 0x10EF58A7    //not used

          #define UP 0x10EFA05F   //roll up blinds
          #define DOWN 0x10EF00FF //roll down blinds
          #define LEFT 0x10EF10EF //close blinds
          #define RIGHT 0x10EF807F //open blinds
          #define SELECT 0x10EF20DF //not used




//////////////////////////////////////////////////////////////////////////////////////////////////////////////


        //BlockE
          int photo_sensor = 1;
          int photo_sensor_val = 0;

          int wind_sensor = 10;
          int wind_sensor_val = 0;
          int RECV_PIN = 11;
          IRrecv irrecv(RECV_PIN);
          decode_results results;

        //BlockF
          CRGB leds[NUM_LEDS];
          LiquidCrystal lcd(52, 53, 48, 49, 50, 51); // initialize the library with the numbers of the interface pins
          int temp_output = 0;


 
//////////////////////////////////Setup Function ///////////////////////////////////////////////////////
void setup()
{

  //Block C
         pinMode(buttonUp, INPUT);
         pinMode(buttonDown, INPUT);
         
         pinMode(M1step, OUTPUT);
         pinMode(M1sleep, OUTPUT);
         pinMode(M1direction, OUTPUT);


        digitalWrite(M1sleep, LOW);
        digitalWrite(M1direction, LOW);
        digitalWrite(M1step, LOW);

         myStepper.setSpeed(10);


   //Block E

         pinMode(photo_sensor, INPUT);

   //BlockF
         FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS); //initialize RGB Driver
         lcd.begin(16, 2);  //initialize RGB Driver


         irrecv.enableIRIn(); // Start the receiver
   
   //Define serial input interface
          Serial.begin(9600);

  
}
 
void loop() 
{
  
 switch (fsm_state)
  { 
     /* 
       Currently not used. Possible implement?
      */
     case INITIAL_STATE:  
     lcd.clear();
     lcd.print("Initial State!");
    
      digitalWrite(13, HIGH);
      temperature();
      fsm_state = ROLLUP_BLINDS;
      break;



    
    case ROLLUP_BLINDS:
            lcd.clear();
            lcd.print("Roll Up Blinds");
            Serial.print("Roll Up Blinds");
            rollup();
            setup();

           temperature();
           FastLED.show();
           delay(1000);
           fsm_state = MANUAL_MODE;
            break;
      
    case ROLLDOWN_BLINDS:
           lcd.clear(); 
           lcd.print("Rolldown Blinds");
           Serial.println("Rolldown Blinds");
           rolldown();
           setup();
       
           temperature();
           FastLED.show();
           delay(1000);
           fsm_state = MANUAL_MODE;
           break;



     case OPEN_BLINDS:
           lcd.clear();
           lcd.print("Open Blinds");
           Serial.println("Open Blinds");

              if (buttonStateUp == 0)  //PWMDuration >= 3130 || PWMDuration <= 3310){ // PWMDuration is in us
                 {  
                     Serial.println("clockwise");
                     myStepper.step(stepsPerRevolution);
                     delay(500);
                 }
          temperature();
          FastLED.show();
          delay(500);
          buttonStateUp = 0;
          fsm_state = MANUAL_MODE;
          break;



      case CLOSE_BLINDS:
         lcd.clear();
         lcd.print("Close Blinds");
         Serial.println("Close Blinds");
    
         Serial.println("counterclockwise");
         myStepper.step(-stepsPerRevolution);
         delay(500);
         temperature();
         FastLED.show();
         delay(1000);
         fsm_state = MANUAL_MODE;
         break;


     case ERROR_STATE:  
      lcd.clear();
      lcd.print("Error State!");
      temperature();
      FastLED.show();
      delay(1000);
      fsm_state = INITIAL_STATE;
      break;

    case MANUAL_MODE:
      lcd.clear();
      lcd.print("RBLINDS EE595");
      temperature(); 

    while(!irrecv.decode(&results))
            {
                // do nothing until IR signal Recieved
            }

             if(irrecv.decode(&results)){
                  Serial.println("Button was pressed");
            if (results.value == UP) 
         {
            fsm_state = ROLLUP_BLINDS;    
          }
           if (results.value == DOWN) 
         {
            fsm_state = ROLLDOWN_BLINDS;    
          }
          
           if (results.value == RIGHT) 
         {
            fsm_state = OPEN_BLINDS;
     
          }
          
       if (results.value == LEFT) 
         {
            fsm_state = CLOSE_BLINDS;    
          }

       if (results.value == SELECT) 
         {
            fsm_state = LIGHT_SENSOR;    
          }

           if (results.value == A) 
         {
            fsm_state = WIND_SENSOR;    
          }
           irrecv.resume();
       }

       break;

    case LIGHT_SENSOR:

          lcd.clear();
          lcd.print("Light Sensor"); 

           
           photo_sensor_val = analogRead(photo_sensor);
            while (photo_sensor_val > 250)  // read the input pin
             { 
               photo_sensor_val = analogRead(photo_sensor);    
               Serial.println(photo_sensor_val);
             } 

             

          temperature();
          FastLED.show();
          delay(1000);
          fsm_state = OPEN_BLINDS;

          break;

        case WIND_SENSOR:

          lcd.clear();
          lcd.print("Wind Sensor"); 

           
           wind_sensor_val = analogRead(wind_sensor);
            while (wind_sensor_val < 100)  // read the input pin
             { 
               wind_sensor_val = analogRead(wind_sensor);    
               Serial.println(wind_sensor_val);
             } 

             

      temperature();
      FastLED.show();
      delay(1000);
      fsm_state = ROLLUP_BLINDS;

    break;


    case AUTO_MODE:
    
     lcd.clear(); 
     lcd.print("Auto Mode");
     Serial.print("Auto Mode");
     
     delay(5000);
       break;


    default:
    
     lcd.clear();
     Serial.println("Staring UP");
     delay(2000);
     fsm_state = MANUAL_MODE;
     
     break;
  }



}



void temperature(){

  int rawvoltage= analogRead(temp_output);
  float millivolts= (rawvoltage / 1024.0) * 5000;
  float fahrenheit= (millivolts * (0.032162162));

  if((fahrenheit >= 75) &&(fahrenheit < 83)) {
     leds[0] = CRGB(0, 255, 0);  // green
  }

    lcd.setCursor(0,1) ;
    lcd.print(fahrenheit); 
    lcd.print(" F");
    lcd.setCursor(0,0);  
    Serial.print(fahrenheit);
    Serial.println(" degrees Fahrenheit, ");

  if((fahrenheit >= 83) && (fahrenheit < 84)){
    leds[0] = CRGB(0, 0, 255);  // BLUE
    lcd.setCursor(0,1) ;
    lcd.print(fahrenheit); 
    lcd.print(" F");
    lcd.setCursor(0,0); 
   Serial.print(fahrenheit);
   Serial.println(" degrees Fahrenheit, ");
  }

  if(fahrenheit > 84){
    leds[0] = CRGB(255, 0, 0);  // red
    lcd.setCursor(0,1) ;
    lcd.print(fahrenheit); 
    lcd.print(" F");
    lcd.setCursor(0,0); 
   Serial.print(fahrenheit);
   Serial.println(" degrees Fahrenheit, ");
  }

  
}

void getCode() {
      if (results.value == POWER) 
         {
            lcd.clear();
            lcd.print("Power");
             Serial.println("POWER");
            delay(2000);
            fsm_state = MANUAL_MODE;    
          }
    
}

void rollup(){

          digitalWrite(M1sleep, HIGH);
          digitalWrite(M1direction, LOW); 
  
          for (int ROTM1B = 0; ROTM1B <= 12000; ROTM1B += 1)  //simulating one full rotation of M1 CW (12000 is for kyles table, 32 blind shades)
            {
              digitalWrite(M1step, HIGH);
              delay(1.1);
              digitalWrite(M1step, LOW);
              delay(1.1);
            }
          digitalWrite(M1sleep, LOW);
}

void rolldown(){

          digitalWrite(M1sleep, HIGH);
          digitalWrite(M1direction, HIGH); 
  
          for (int ROTM1B = 0; ROTM1B <= 12000; ROTM1B += 1)  //simulating one full rotation of M1 CW (12000 is for kyles table, 32 blind shades)
            {
              digitalWrite(M1step, HIGH);
              delay(1.1);
              digitalWrite(M1step, LOW);
              delay(1.1);
            }
          digitalWrite(M1sleep, LOW);
}


