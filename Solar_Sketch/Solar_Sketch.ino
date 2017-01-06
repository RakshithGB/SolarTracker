#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <DS3231.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
DS3231  rtc(SDA, SCL);

Time t;
int d,m,y,h,mi,s;
float days;
float const LONGITUDE = 78;
float const LATITUDE = 13;
float const LSTM = 82.5;
float LT;
float currentMinute;
float previousMinute = 0;
int test = false;
int relay = 4;

void setup(void) 
{
  Serial.begin(115200);
  
  //Initialise the sensor 
  if(!mag.begin())
  {
    Serial.println("No HMC5883 detected!");  
    while(1);
  }
 
  //start rtc communication
  rtc.begin();
//  rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to SUNDAY
//  rtc.setTime(16, 14, 00);     // Set the time to 12:00:00 (24hr format)
//  rtc.setDate(3, 8, 2016);   // Set the date to January 1st, 2014

 t = rtc.getTime();
 mi = (int)t.min;
 s = (int)t.sec;
 float secs = ((float)s)/60;
 float minut = mi+secs;
 previousMinute = minut;
 

 digitalWrite(relay, LOW);
 test = 0;
}

void loop(void) 
{ 
  sensors_event_t event; 
  mag.getEvent(&event);



  //fetch date and time in int format
  t = rtc.getTime();
  m = (int)t.mon;
  d = (int)t.date;
  y = (int)t.year;
  h = (int)t.hour;
  mi = (int)t.min;
  s = (int)t.sec;
  
  //get LT in hour format
  float minute = ((float)mi)/60;
  LT = h + minute;
  Serial.print("Local Time:");
  Serial.println(LT);

  //get minut in seconds accuracy
  float secs = ((float)s)/60;
  float minut = mi+secs;

  //get number of days, accuracy +-2 days.
  float N1 = floor(275*m/9);
  float N2 = floor((m+9)/12);
  float N3 = (1 + floor((y-4*floor(y/4)+2)/3));
  float days = N1 - (N2*N3) + d - 30;
  Serial.print("Days: ");
  Serial.println(days);

  //perform calculations for solar angles.
  float B = (0.98*(days-81));
  float b_rad = toRad(B);
  float EOT = 9.87*sin(2*b_rad) - (7.53*cos(b_rad)) - (1.5*sin(b_rad)); 
  float TC = (4*(LONGITUDE - LSTM)) + EOT;
  float LST = LT + (TC/60);
  float HRA_deg = 15*(LST - 12);
  float decl_deg = 23.45*sin(b_rad);
  float elev_deg = toDegrees(asin(sin(toRad(decl_deg))*sin(toRad(LATITUDE)) + cos(toRad(decl_deg))*cos(toRad(LATITUDE))*cos(toRad(HRA_deg))  ));
  float azimuth_deg_num = sin(toRad(decl_deg))*cos(toRad(LATITUDE)) - cos(toRad(decl_deg))*sin(toRad(LATITUDE))*cos(toRad(HRA_deg));
  float azimuth_deg_den = cos(toRad(elev_deg));
  float azimuth_deg = toDegrees(acos(azimuth_deg_num / azimuth_deg_den));
  if(LST<12 || HRA_deg<0)
  {
    azimuth_deg = azimuth_deg;
  }
  else if(LST>12 || HRA_deg>0)
  {
    azimuth_deg = 360-azimuth_deg;
  }
  int rounded = abs((int)round(azimuth_deg));
    
  // Print all details
  Serial.print("Date: ");
  Serial.print(d);
  Serial.print(".");
  Serial.print(m);
  Serial.print(".");
  Serial.print(y);
  Serial.print("    ");
  Serial.print("Time: ");
  Serial.print(h);
  Serial.print(":");
  Serial.print(mi);
  Serial.print(":");
  Serial.print(s);
  Serial.println();
  Serial.print("TC : ");
  Serial.println(TC);
  Serial.print("EOT : ");
  Serial.println(EOT);
  Serial.print("HRA :");
  Serial.println(HRA_deg);
  Serial.print("decl_deg :");
  Serial.println(decl_deg);
  Serial.print("elev_deg : ");
  Serial.println(elev_deg);
  Serial.print("azimuth_deg :" );
  Serial.println(azimuth_deg);
  Serial.print("rounded value: ");
  Serial.println(rounded);
  
  // Calculate heading when the magnetometer is level, then correct for signs of axis.
  float heading = atan2(event.magnetic.y, event.magnetic.x);
 
  //set declinationAngle based on magnetic field strength in locality.
  float declinationAngle = 0.017;
  heading += declinationAngle;
  
  // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;
    
  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
   
  // Convert radians to degrees for readability.
  int headingDegrees =abs(heading * 180/M_PI); 

  //start rotating base
 if(headingDegrees>(rounded+25) || headingDegrees<(rounded-25))
 {
  if (test == 0)
   {
   Serial.println("Motor stoped");
  // digitalWrite(relay, LOW);
   }
   else 
   {
    Serial.println("Motor started");
    //digitalWrite(relay, HIGH);
   }
   currentMinute = minut;
   Serial.print("Current Minute: ");
   Serial.println(currentMinute);
   Serial.print("Previous Minute: ");
   Serial.println(previousMinute);
   Serial.println("Difference: ");
   Serial.println(currentMinute-previousMinute);
   if(currentMinute-previousMinute>0.3)
   {
    previousMinute = currentMinute;
    if(headingDegrees<(rounded+5) && headingDegrees>(rounded-5))
    {
      previousMinute = currentMinute;
      Serial.println("Motor stoped");
      digitalWrite(relay, LOW);
      test = 0;
    }
    else
    {
       Serial.println("Motor Started");
       digitalWrite(relay, HIGH);
       test = 1;
    }
   }
 }
 else
{
    if (test == 0)
    {
      previousMinute = currentMinute;
      Serial.println("Motor stoped");
      digitalWrite(relay, LOW);
     
     }
    else if (test == 1)
    {
      if(headingDegrees<(rounded+5) && headingDegrees>(rounded-5))
      {
        previousMinute = currentMinute;

        test = 0;
        Serial.println("Motor stoped");  
        digitalWrite(relay, LOW); 
      }
      else
     {
       
       test = 1;
       Serial.println("Motor Started");  
       digitalWrite(relay, HIGH); 
      }       
   }
}

  Serial.print("Heading (degrees): "); Serial.println(headingDegrees);  
  delay(1000);
}

float toDegrees(float angle)
{
  return angle*57.295;
}

float toRad(float angle)
{
  return angle/57.295;
}
