// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_MLX90614_B.h"

// This #include statement was automatically added by the Particle IDE.
#include <Adafruit_MLX90614.h>

#include "math.h"
#include <application.h>


int sample_rate = 60*1000; //the rate at which the sensor will collect and publish data to the internet


// defined constants
#define TEMPERATURENOMINAL 25   
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 5
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
// the value of the 'other' resistor
#define SERIESRESISTOR 10000  
uint16_t samples[NUMSAMPLES];
  uint8_t i;
  float average;
 
// What pin to connect the sensor to
int thermPin1 = A0; 
int thermPin2 = A1; 

// global variable definition for the temperature
double thermTemp1= 0;
double thermTemp2= 0;

double MLXobjectTemp1 = 0;
double MLXambientTemp1 = 0;
double MLXobjectTemp2 = 0;
double MLXambientTemp2 = 0;


Adafruit_MLX90614 mlx1 = Adafruit_MLX90614();
Adafruit_MLX90614_B mlx2 = Adafruit_MLX90614_B();

// setup loop
void setup() {
    
    //STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));
  

    Particle.variable("thermTemp1",thermTemp1);
    Particle.variable("thermTemp2",thermTemp2);
    
    Particle.variable("MLXobjectTemp1", MLXobjectTemp1);
    Particle.variable("MLXambientTemp1", MLXambientTemp1);
    
    Particle.variable("MLXobjectTemp2", MLXobjectTemp2);
    Particle.variable("MLXambientTemp2", MLXambientTemp2);

    mlx1.begin();  
    mlx2.begin();  

}

//void loop.
void loop() {
    
    delay(sample_rate);
    thermTemp1 = therm(thermPin1);
    thermTemp2 = therm(thermPin2);
    
    // Particle.publish("therm1.c,1 ", "thermistor1:"+String(thermTemp1)+" ");
    // Particle.publish("therm2.c,1 ", "thermistor2:"+String(thermTemp2)+" ");
    

    MLXobjectTemp1 = mlx1.readObjectTempC();
    MLXambientTemp1 = mlx1.readAmbientTempC();
    
    MLXobjectTemp2 = mlx2.readObjectTempC();
    MLXambientTemp2 = mlx2.readAmbientTempC();


    // Particle.publish("mlx1.c,2 ","object:"+String(MLXobjectTemp1)+" ambient:"+String(MLXambientTemp1)+" ");
    // Particle.publish("mlx2.c,2 ","object:"+String(MLXobjectTemp2)+" ambient:"+String(MLXambientTemp2)+" ");
    
    char buf1[256];
    snprintf(buf1, sizeof(buf1), "{\"thermistor\":%.3f,\"mlx_object\":%.3f,\"mlx_ambient\":%.3f}", thermTemp1, MLXobjectTemp1, MLXambientTemp1);
    Particle.publish("radiator1", buf1, PRIVATE);
    
    char buf2[256];
    snprintf(buf2, sizeof(buf2), "{\"thermistor\":%.3f,\"mlx_object\":%.3f,\"mlx_ambient\":%.3f}", thermTemp2, MLXobjectTemp2, MLXambientTemp2);
    Particle.publish("radiator2", buf2, PRIVATE);
}





// THERMISTOR FUNCTION
//this is the function that measures the voltage across the thermistor and 
//calcualtes the resistance. a number of samples are collected and averaged,
//NUMSAMPLES specifies this number above, to smooth noise
double therm(int pin) {
  
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(pin);
   delay(10);
  }
 
  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;
  
  double reading = average;

 
  // convert the value to resistance
  reading = (4095 / reading)  - 1;     // (4095/ADC - 1) 
  reading = SERIESRESISTOR / reading;  // 10K / (1023/ADC - 1)
  
  //the steinart method is a standard method of mapping the resistance reading
  //to a temperature
  float steinhart;
  steinhart = reading / SERIESRESISTOR;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
  
  
//return the calculated value, which is a temperature!  
  return steinhart;
}