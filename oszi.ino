#include "pins_arduino.h"

///Array for storing the messurment Data
//#define size_ 1000  
const int size_ = 800;
int val[size_];

char buf [21];
int buf_size =20;
volatile byte pos;
volatile byte new_data_package = 1;
volatile boolean process_it;

volatile boolean process_it2;
char ralaisReg=0;

int analogPin = A7;
volatile int test_=0;

volatile byte resiv_reg;

volatile uint8_t reg_0 =0b00000000; //
volatile uint8_t reg_1 =0b00000000;
volatile uint8_t reg_2 =0b00000000;
volatile uint8_t reg_3 =0b00000000;
volatile uint8_t reg_4 =0b00000000;
volatile uint8_t reg_5 =0b00000000;
volatile uint8_t reg_E =0b00000000; //Read Data
volatile uint8_t reg_F =0b00000000; //print

#define ralais_1 7  //Vin/dGND
#define ralais_2 6  //AC/dDC
#define ralais_3 5  //Amp1
#define ralais_4 4  //Amp2
#define ralais_5 3  //Amp3

void readData(int size_, int delaytime, int * arr);

void setup (void){
  
  Serial.begin (9600);      // debugging

  pinMode(ralais_1, OUTPUT);  // initialisation of ralais
  pinMode(ralais_2, OUTPUT);
  pinMode(ralais_3, OUTPUT);
  pinMode(ralais_4, OUTPUT);
  pinMode(ralais_5, OUTPUT);

  digitalWrite(ralais_1,0);
  digitalWrite(ralais_2,0);
  digitalWrite(ralais_3,0);
  digitalWrite(ralais_4,0);
  digitalWrite(ralais_5,0);

  pinMode(MISO, OUTPUT);    // have to be turned on send on miso


  SPCR |= _BV(SPE);         // turn on SPI in slave mode
  SPCR |= _BV(SPIE);        // turn on interrupts
  
  SPCR = 0b11000011;
 
  pos = 0;
  process_it = false;
}                           // end of setup



ISR (SPI_STC_vect){          // SPI interrupt routine
  
  byte c = SPDR;

  if(new_data_package){
    new_data_package = 0;
    resiv_reg = c;
  }
  else if(resiv_reg == 0){
    reg_0 = c;
    new_data_package= 1;
  }
  else if(resiv_reg == 1){
    ralaisReg = c;
    process_it2 = true;
    new_data_package = 1;
  }
  else if(resiv_reg == 2){
    
  }
  else if(resiv_reg == 15){
    if (pos < buf_size){   // add to buffer if room
      buf [pos++] = c;
      if (c == 0xA){
        process_it = true;    //example: newline means time to process buffer
      }
    }                         // end of room available
  
    SPDR = test_>>2;  
  }
}


// main loop - wait for flag set in interrupt routine
void loop (void)
{
  bool bedingung=true;

  if (digitalRead(SS)){       //Stellt sicher das bei einem fehlerhaften SPI Trasfer beim nächsten mal wieder eine normale konversation stattfinden kann
                              //Wichtig! gesammte konversation zu einem Register muss am Stück durchgeführt werden.
    //! to Do   Fehlererkennung mit Register     00 bei keinem Fehler?
    new_data_package = 1;
  }

  if (process_it){
    if (buf[17]!='0') bedingung=true;
    test_ = analogRead(analogPin);    // read the input pin
    buf [20] = '\n';                  //New line After printing
    buf [21] = '\0';                  //end "string"
    Serial.print(buf);
    pos = 0;
    process_it = false;
  } // end of flag set 

  if(process_it2){
    digitalWrite(ralais_1,((ralaisReg>>0)&&1));
    digitalWrite(ralais_2,((ralaisReg>>1)&&1));
    digitalWrite(ralais_3,((ralaisReg>>2)&&1));
    digitalWrite(ralais_4,((ralaisReg>>3)&&1));
    digitalWrite(ralais_5,((ralaisReg>>4)&&1));
    process_it2=false;
  }
  
  if(bedingung){
    int test_size=10;
    int test_delay=200;
    int test_arr[10];
    uint8_t test_arr_2[10];
    readData(test_size,test_delay,test_arr);

    Serial.println("Data");
    for(int i=0;i<test_size;i++){
      Serial.print(test_arr[i]);
      Serial.print("\t");
      Serial.println(test_arr_2[i]);
      test_arr_2[i]=test_arr[i]>>2;
    }
    Serial.println("Data End");
    delay(1000);
  }

}   // end of loop


void readData(int size, int delaytime, int* arr){

  for(int i=0;i<size;i++){
    *(arr+i) = analogRead(analogPin); // read the input pin
//    delayMicroseconds(delaytime);    // wait for next step
    delay(delaytime);
  }
//  micros();
  millis();

  if (0){
    //Read settings
    SPCR &= 0x7F;
    reg_E = 0;
    SPCR |= 0x80;

    while(11 <=10){
      int test = analogRead(analogPin);
    }
  }

}
