// This #include statement was automatically added by the Particle IDE.
#include <SparkFun_Qwiic_Humidity_AHT20.h>

AHT20 humiditySensor;

// bool logging = FALSE; // set to true of you want Influx to catch the photon log to database
int sample_rate = 60 * 1000; // how often you want device to publish, where the number in front of a thousand is the seconds you want

//temperature and humidity values 
double t = 0;
double h = 0;
float tF = 0;


//constants for calculating absolute humidity
double EPSILON = 0.62198;  // ratio of molecular weights of water and dry air
double P_ATM = 102595.8;   // current barometric pressure, Pa - UPDATE!!!! at time of experiment


//fit to Magnus equation Psat = C1*exp(A1*t/(B1+t)) where t is in C, outputs Pa
double A = 17.625; //from Alduchov and Eskridge, 1996
double B = 243.04; // degrees C
double C = 610.94; //Pa

double g = 0;

void setup()
{
  
    Particle.variable("rate", sample_rate);  // Publish frequency
    // Particle.variable("logging", logging);
    Particle.variable("temp", t);
    Particle.variable("humidity", h);
    Particle.variable("g", g);
    
    
  Serial.begin(115200);
  Serial.println("AHT20 test");
  
  Wire.begin(); //Join I2C bus

  //Check if the AHT20 will acknowledge
  if (humiditySensor.begin() == false)
  {
    Serial.println("AHT20 not detected.");
    while (1);
  }
  Serial.println("AHT20 acknowledged.");
}

void loop()
{
    //If a new measurement is available
    //Get the new temperature and humidity value
    if (humiditySensor.available() == true) 
    {
        t = humiditySensor.getTemperature();
        h = humiditySensor.getHumidity();
        tF = (t* 9) /5 + 32;
        g = absolute_H(t,h);
        
        if (! isnan(t)) {  // check if 'is not a number'
            //Temperature in C
            Serial.print("Temp *C = "); Serial.println(t);
            //Temperature in F
            Serial.print("Temp *F = "); Serial.println(tF);
        } else { 
            Serial.println("Failed to read temperature");
        }
        
        if (! isnan(h)) {  // check if 'is not a number'
            Serial.print("Hum. % = "); Serial.println(h);
        } else { 
            Serial.println("Failed to read humidity");
        }
        
        Serial.println();
    
        char buf[256];
        snprintf(buf, sizeof(buf), "{\"air_temp\":%.3f,\"humidity\":%.3f,\"abs_humidity\":%.3f}", t, h, g);
        Particle.publish("room_only", buf, PRIVATE);
        
        delay(sample_rate);
    }
    humiditySensor.triggerMeasurement();
    //The AHT20 can respond with a reading every ~50ms. However, increased read time can cause the IC to heat around 1.0C above ambient.
    //The datasheet recommends reading every 2 seconds.
}

//helper function to compute absolute humidity
double absolute_H(double tempC, double rh) {
    double Psat = C*exp(A*tempC/(B+tempC));
    double Pvap = rh/100*Psat;
    double ah = EPSILON*Pvap/(P_ATM-Pvap)*1000; //convert to g/kg
    return ah; //absolute humidity in g/kg
}
