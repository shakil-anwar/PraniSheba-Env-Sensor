#include "calibration.h"

float METHANE = 0, AMMONIA = 1, CO2 = 2, HUMIDITY = 3;


WebServer server(80);

String _tempMethane="", _tempAmmonia="";
static bool calibServerOn;

bool startCalibServer()
{
  bool status;
  IPAddress ip;
  calibServerOn = false;
  ip = WiFi.localIP();
  Serial.print("Got IP: ");  Serial.println(ip);
  if(ip != NULL)
  {
    if(!MDNS.begin("apsiot")) {
      Serial.println("Error starting mDNS");
      status = false;
    }
    else
    {
      status = true;
    }
  }
  else
  {
    status = false;
  }
  
  
  if(status)
  {
    server.on("/", handle_OnConnect);
    server.on("/calcfio2", handleData);
    server.on("/chkfw", handleChkFirmware);
    server.on("/fwupdate",handleUpdateFirmware);
    server.on("/methanePPM", calibMethane);
    server.on("/ammoniaPPM", calibAmmonia);
    server.onNotFound(handle_NotFound);

    server.begin();
    Serial.println("HTTP server started");
    calibServerOn = true;
  }
  return status;
  
  
}

void runCalibServer()
{
  if(calibServerOn)
  {
    server.handleClient();
  }
}


void handle_OnConnect() {

  server.send(200, "text/html", SendHTML(METHANE, AMMONIA, CO2, HUMIDITY, false));
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

void handleData()
{
   server.send(200, "text/html", sendCalibrationPage(mq4.calculateRs(), mq135.calculateRs()));
}

void handleChkFirmware()
{  
  if (FirmwareVersionCheck())
  {
    String fwVersion = "5.0";
    server.send(200,"text/html", sendNewFwPage(fwVersion));
  }
  else
  {
    server.send(200,"text/html", "Firmwire is already updated");
  }
  
}

void handleUpdateFirmware()
{
  server.send(200,"text/html", "firmware is updating...");
}

float calibMethane()
{
  _tempMethane = server.arg("methaneStandardPPM").c_str();
  float methane = _tempMethane.toFloat();
  if(methane != 0)
  {
    mq4.externCalibrate(methane);
  }
  Serial.print("Methane: "); Serial.println(methane);
  server.send(200, "text/html", sendCalibrationPage(mq4.calculateRs(), mq135.calculateRs()));
  return methane;
}


float calibAmmonia()
{
  _tempAmmonia = server.arg("ammoniaStandardPPM").c_str();
  float ammonia = _tempAmmonia.toFloat();
  if(ammonia != 0)
  {
    mq135.externCalibrate(ammonia);
  }
  Serial.print("Ammonia: "); Serial.println(ammonia);
  server.send(200, "text/html", sendCalibrationPage(mq4.calculateRs(), mq135.calculateRs()));
  return ammonia;
}




String SendHTML(float METHANE, float AMMONIA, float CO2, float HUMIDITY, bool autoRefresh) {
  String ptr = "<!DOCTYPE html>";
  ptr += "<html>";
  ptr += "<head>";
  ptr += "<title>ESP8266 WebServer</title>";
  ptr += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  ptr += "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.7.2/css/all.min.css'>";
  ptr += "<link rel='stylesheet' type='text/css' href='styles.css'>";
  ptr += "<style>";
  ptr += "body { background-color: #fff; font-family: sans-serif; color: #333333; font: 14px Helvetica, sans-serif box-sizing: border-box;}";
  ptr += "#page { margin: 20px; background-color: #fff;}";
  ptr += ".container { height: inherit; padding-bottom: 20px;}";
  ptr += ".header { padding: 20px;}";
  ptr += ".header h1 { padding-bottom: 0.3em; color: #008080; font-size: 45px; font-weight: bold; font-family: Garmond, 'sans-serif'; text-align: center;}";
  ptr += "h2 { padding-bottom: 0.2em; border-bottom: 1px solid #eee; margin: 2px; text-align: left;}";
  ptr += ".header h3 { font-weight: bold; font-family: Arial, 'sans-serif'; font-size: 17px; color: #b6b6b6; text-align: center;}";
  ptr += ".box-full { padding: 20px; border 1px solid #ddd; border-radius: 1em 1em 1em 1em; box-shadow: 1px 7px 7px 1px rgba(0,0,0,0.4); background: #fff; margin: 20px; width: 300px;}";
  ptr += "@media (max-width: 494px) { #page { width: inherit; margin: 5px auto; } #content { padding: 1px;} .box-full { margin: 8px 8px 12px 8px; padding: 10px; width: inherit;; float: none; } }";
  ptr += "@media (min-width: 494px) and (max-width: 980px) { #page { width: 465px; margin 0 auto; } .box-full { width: 380px; } }";
  ptr += "@media (min-width: 980px) { #page { width: 930px; margin: auto; } }";
  ptr += ".sensor { margin: 12px 0px; font-size: 2.5rem;}";
  ptr += ".sensor-labels { font-size: 1rem; vertical-align: middle; padding-bottom: 15px;}";
  ptr += ".units { font-size: 1.2rem;}";
  ptr += "hr { height: 1px; color: #eee; background-color: #eee; border: none;}";
  ptr += "</style>";

  ptr += "</head>";
  ptr += "<body>";
  ptr += "<div id='page'>";
  ptr += "<div class='header'>";
  ptr += "<h1>Gas Sensor</h1>";
  ptr += "</div>";

  ptr += "<div id='content' align='center'>";
  ptr += "<div class='box-full' align='left'>";
  ptr += "<h2>Calibration</h2>";
  ptr += "<div class='sensors-container'>";

  ptr += "<form method='get' action='calcfio2'>";

  ptr += "<br/><button type='submit'>Calibration</button></form>";

  ptr += "</div>";
  ptr += "</div>";
  ptr += "</div>";
  
  ptr += "<div id='content' align='center'>"; 
  ptr += "<div class='box-full' align='left'>";
  ptr += "<h2>Firmware Update</h2>"; 
  ptr += "<div class='firmware-container'>";

  ptr += "<form method='get' action='chkfw'>"; 

  ptr += "<br/><button type='submit'>Check for firmware update</button></form>";
  ptr += "</div> ";
  ptr += "</div>"; 
  ptr += "</div>";


  // ptr += "<div id='content' align='center'>";
  // ptr += "<div class='box-full' align='left'>";
  // ptr += "<h2>Sensor Readings</h2>";
  // ptr += "<div class='sensors-container'>";

  // //For Methane
  // ptr += "<p class='sensor'>";
  // ptr += "<span class='sensor-labels'> METHANE </span>";
  // ptr += METHANE;
  // ptr += "<sup class='units'>PPM</sup>";
  // ptr += "</p>";
  // ptr += "<hr>";

  // //For Ammonia
  // ptr += "<p class='sensor'>";
  // ptr += "<span class='sensor-labels'> AMMONIA </span>";
  // ptr += AMMONIA;
  // ptr += "<sup class='units'>PPM</sup>";
  // ptr += "</p>";
  // ptr += "<hr>";

  // //For CO2
  // ptr += "<p class='sensor'>";
  // ptr += "<span class='sensor-labels'> C02 </span>";
  // ptr += CO2;
  // ptr += "<sup class='units'>PPM</sup>";
  // ptr += "</p>";
  // ptr += "<hr>";

  // //For Humidity
  // ptr += "<p class='sensor'>";
  // ptr += "<span class='sensor-labels'> HUMIDITY </span>";
  // ptr += HUMIDITY;
  // ptr += "<sup class='units'>%</sup>";
  // ptr += "</p>";

  // ptr += "</div>";
  // ptr += "</div>";
  // ptr += "</div>";
  ptr += "</div>";
  ptr += "</div>";
  ptr += "</body>";
  ptr += "</html>";
  return ptr;
}

String sendCalibrationPage(float methaneR0, float ammoniaR0)
{
  String ptr = "<!DOCTYPE html>";
  ptr += "<html>";
  ptr += "<head>";
  ptr += "<title>ESP8266 WebServer</title>";
  ptr += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  ptr += "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.7.2/css/all.min.css'>";
  ptr += "<link rel='stylesheet' type='text/css' href='styles.css'>";
  ptr += "<style>";
  ptr += "body { background-color: #fff; font-family: sans-serif; color: #333333; font: 14px Helvetica, sans-serif box-sizing: border-box;}";
  ptr += "#page { margin: 20px; background-color: #fff;}";
  ptr += ".container { height: inherit; padding-bottom: 20px;}";
  ptr += ".header { padding: 20px;}";
  ptr += ".header h1 { padding-bottom: 0.3em; color: #008080; font-size: 45px; font-weight: bold; font-family: Garmond, 'sans-serif'; text-align: center;}";
  ptr += "h2 { padding-bottom: 0.2em; border-bottom: 1px solid #eee; margin: 2px; text-align: left;}";
  ptr += ".header h3 { font-weight: bold; font-family: Arial, 'sans-serif'; font-size: 17px; color: #b6b6b6; text-align: center;}";
  ptr += ".box-full { padding: 20px; border 1px solid #ddd; border-radius: 1em 1em 1em 1em; box-shadow: 1px 7px 7px 1px rgba(0,0,0,0.4); background: #fff; margin: 20px; width: 300px;}";
  ptr += "@media (max-width: 494px) { #page { width: inherit; margin: 5px auto; } #content { padding: 1px;} .box-full { margin: 8px 8px 12px 8px; padding: 10px; width: inherit;; float: none; } }";
  ptr += "@media (min-width: 494px) and (max-width: 980px) { #page { width: 465px; margin 0 auto; } .box-full { width: 380px; } }";
  ptr += "@media (min-width: 980px) { #page { width: 930px; margin: auto; } }";
  ptr += ".sensor { margin: 12px 0px; font-size: 2.5rem;}";
  ptr += ".sensor-labels { font-size: 1rem; vertical-align: middle; padding-bottom: 15px;}";
  ptr += ".units { font-size: 1.2rem;}";
  ptr += "hr { height: 1px; color: #eee; background-color: #eee; border: none;}";
  ptr += "</style>";


  ptr += "</head>";
  ptr += "<body>";
  ptr += "<div id='page'>";
  ptr += "<div class='header'>";
  ptr += "<h1>Calibrate Sensor</h1>";
  ptr += "</div>";

  ptr += "<div id='content' align='center'>";
  ptr += "<div class='box-full' align='left'>";
  ptr += "<h2>Calibrate Methane</h2>";
  ptr += "<div class='sensors-container'>";
  ptr += "<form method='get' action='methanePPM'>";

  ptr += "<label> R0 value: </label>"; 
  ptr += methaneR0;
  ptr += "<br/><label for=\"methaneStandardPPM\"> Standard PPM: </label>";
  ptr += "<input type=\"text\" id=\"methaneStandardPPM\" name=\"methaneStandardPPM\">";
  ptr += "<br/><button type='submit'>Calibrate</button></form>";

  ptr += "</div>";
  ptr += "</div>";
  ptr += "</div>";

  ptr += "<div id='content' align='center'>";
  ptr += "<div class='box-full' align='left'>";
  ptr += "<h2>Calibrate Ammonia</h2>";
  ptr += "<div class='sensors-container'>";
  ptr += "<form method='get' action='ammoniaPPM'>";

  ptr += "<label> R0 value: </label>"; 
  ptr += ammoniaR0;
  ptr += "<br/><label for=\"ammoniaStandardPPM\"> Standard PPM: </label>";
  ptr += "<input type=\"text\" id=\"ammoniaStandardPPM\" name=\"ammoniaStandardPPM\">";
  ptr += "<br/><button type='submit'>Calibrate</button></form>";

  ptr += "</div>";
  ptr += "</div>";
  ptr += "</div>";

  

  ptr += "</body>";
  ptr += "</html>";
  return ptr;
  
}


String displayValues(float methaneR0, float ammoniaR0)
{
  String ptr = "<!DOCTYPE html>";
  ptr += "<html>";
  ptr += "<head>";
  ptr += "<title>ESP8266 WebServer</title>";
  ptr += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  ptr += "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.7.2/css/all.min.css'>";
  ptr += "<link rel='stylesheet' type='text/css' href='styles.css'>";
  ptr += "<style>";
  ptr += "body { background-color: #fff; font-family: sans-serif; color: #333333; font: 14px Helvetica, sans-serif box-sizing: border-box;}";
  ptr += "#page { margin: 20px; background-color: #fff;}";
  ptr += ".container { height: inherit; padding-bottom: 20px;}";
  ptr += ".header { padding: 20px;}";
  ptr += ".header h1 { padding-bottom: 0.3em; color: #008080; font-size: 45px; font-weight: bold; font-family: Garmond, 'sans-serif'; text-align: center;}";
  ptr += "h2 { padding-bottom: 0.2em; border-bottom: 1px solid #eee; margin: 2px; text-align: left;}";
  ptr += ".header h3 { font-weight: bold; font-family: Arial, 'sans-serif'; font-size: 17px; color: #b6b6b6; text-align: center;}";
  ptr += ".box-full { padding: 20px; border 1px solid #ddd; border-radius: 1em 1em 1em 1em; box-shadow: 1px 7px 7px 1px rgba(0,0,0,0.4); background: #fff; margin: 20px; width: 300px;}";
  ptr += "@media (max-width: 494px) { #page { width: inherit; margin: 5px auto; } #content { padding: 1px;} .box-full { margin: 8px 8px 12px 8px; padding: 10px; width: inherit;; float: none; } }";
  ptr += "@media (min-width: 494px) and (max-width: 980px) { #page { width: 465px; margin 0 auto; } .box-full { width: 380px; } }";
  ptr += "@media (min-width: 980px) { #page { width: 930px; margin: auto; } }";
  ptr += ".sensor { margin: 12px 0px; font-size: 2.5rem;}";
  ptr += ".sensor-labels { font-size: 1rem; vertical-align: middle; padding-bottom: 15px;}";
  ptr += ".units { font-size: 1.2rem;}";
  ptr += "hr { height: 1px; color: #eee; background-color: #eee; border: none;}";
  ptr += "</style>";


  ptr += "</head>";
  ptr += "<body>";
  ptr += "<label> R0 value: </label>"; 
  ptr += methaneR0;
  ptr += "<label> R0 value: </label>"; 
  ptr += ammoniaR0;
  ptr += "</body>";
  ptr += "</html>";
  return ptr;

}


String sendNewFwPage(String fwVersion)
{
  String ptr = "<!DOCTYPE html>";
  ptr += "<html>";
  ptr += "<head>";
  ptr += "<title>ESP8266 WebServer</title>";
  ptr += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  ptr += "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.7.2/css/all.min.css'>";
  ptr += "<link rel='stylesheet' type='text/css' href='styles.css'>";
  ptr += "<style>";
  ptr += "body { background-color: #fff; font-family: sans-serif; color: #333333; font: 14px Helvetica, sans-serif box-sizing: border-box;}";
  ptr += "#page { margin: 20px; background-color: #fff;}";
  ptr += ".container { height: inherit; padding-bottom: 20px;}";
  ptr += ".header { padding: 20px;}";
  ptr += ".header h1 { padding-bottom: 0.3em; color: #008080; font-size: 45px; font-weight: bold; font-family: Garmond, 'sans-serif'; text-align: center;}";
  ptr += "h2 { padding-bottom: 0.2em; border-bottom: 1px solid #eee; margin: 2px; text-align: left;}";
  ptr += ".header h3 { font-weight: bold; font-family: Arial, 'sans-serif'; font-size: 17px; color: #b6b6b6; text-align: center;}";
  ptr += ".box-full { padding: 20px; border 1px solid #ddd; border-radius: 1em 1em 1em 1em; box-shadow: 1px 7px 7px 1px rgba(0,0,0,0.4); background: #fff; margin: 20px; width: 300px;}";
  ptr += "@media (max-width: 494px) { #page { width: inherit; margin: 5px auto; } #content { padding: 1px;} .box-full { margin: 8px 8px 12px 8px; padding: 10px; width: inherit;; float: none; } }";
  ptr += "@media (min-width: 494px) and (max-width: 980px) { #page { width: 465px; margin 0 auto; } .box-full { width: 380px; } }";
  ptr += "@media (min-width: 980px) { #page { width: 930px; margin: auto; } }";
  ptr += ".sensor { margin: 12px 0px; font-size: 2.5rem;}";
  ptr += ".sensor-labels { font-size: 1rem; vertical-align: middle; padding-bottom: 15px;}";
  ptr += ".units { font-size: 1.2rem;}";
  ptr += "hr { height: 1px; color: #eee; background-color: #eee; border: none;}";
  ptr += "</style>";

  ptr += "</head>";
  ptr += "<body>";
  ptr += "<div id='page'>";
  ptr += "<div class='header'>";
  ptr += "<h1>Firmware Update</h1>";
  ptr += "</div>";

  ptr += "<h2>New firmware version ";
  ptr += fwVersion;
  ptr += " found...</h2>";

  ptr += "<div id='content' align='center'>";
  ptr += "<div class='box-full' align='left'>";
  ptr += "<h4>Press Button to update firmware</h4>";
  ptr += "<div class='firmware-container'>";

  ptr += "<form method='get' action='fwupdate'>";

  ptr += "<br/><button type='submit'>update firmware</button></form>";

  ptr += "</div>";
  ptr += "</div>";
  ptr += "</div>";
  

  ptr += "</div>";
  ptr += "</div>";
  ptr += "</body>";
  ptr += "</html>";
  return ptr;
}