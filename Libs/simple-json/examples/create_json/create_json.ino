#include "simple_json.h"



//Your sensor data type. everythin in structure for easy manupulation
typedef struct sensor_data_t 
{
  char sid[6];
  uint32_t unixTime;
  float hum;
  float temp;
};

sensor_data_t sensor;   //creating a sensor variable of sensor_data_t type

//Function prototype
char* json_packet(char *jsonPtr, sensor_data_t data);


void setup()
{
  Serial.begin(9600);
  Serial.println(F("All Setup Done"));

  
  strcpy(sensor.sid, "s1234");  //sensor id = "s1234"
  sensor.unixTime = 1592998627; //unix time
  sensor.hum      = 56.5;       //humidity
  sensor.temp     = 37.4;       //temperature
  char json_str[90] = "";       //buffer to keep json file
  
  json_packet(json_str,sensor);//This function convertes your sensor data into json
  Serial.println(json_str);

}

void loop()
{

}


char* json_packet(char *jsonPtr, sensor_data_t data)
{
  char *json = jsonPtr;
  *json = '{';
  json++;
  json = enquote(json, F("sid"));
  json = add_str(json, data.sid);
  
  json = enquote(json, F("time"));
  json = add_long(json, data.unixTime);

  json = enquote(json, F("tem"));
  json = add_float(json, data.temp);


  json = enquote(json, F("hum"));
  json = add_float(json, data.hum);
  json --;      //decrementing to replace last comma by brace
  *json = '}';
  json++;
  *json = '\0'; //null terminating as extra safety

  return jsonPtr;
}
