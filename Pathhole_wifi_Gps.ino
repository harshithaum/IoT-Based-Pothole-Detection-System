// I2C LCD DISPLAY
// NODE MCU ESP8266 Microcontroller are Used
#include <NewPing.h>
#include <LCD_I2C.h>
LCD_I2C lcd(0x27, 16, 2);
int Scrolling_Speed = 20;
String message = " IOT-BASED DAMAGED ROAD AND POTHOLE LOCATION DETECTION SYSTEM ";
#include <ESP8266WiFi.h>
WiFiServer server(80);
String header;


unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;
int Connection_Count = 0;

#include "time.h"
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;  // IST = UTC+5:30
const int   daylightOffset_sec = 0;

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int RXPin = D8, TXPin = D7;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

float lat = 0.00;
float lon = 0.00;

//Default value
float D_lat = 11.27;
float D_lon = 77.60;

int Gps_Getting , Gps_Checking = 0;
// CSV Data
String csvData = "Time,pot_hole,Ultrasonic,lat,lon\n";
unsigned long lastSave = 0;


// ************** Pin Configuration *********************************************** 
#define TRIG_PIN  D5
#define ECHO_PIN  D6
#define MAX_DISTANCE 200 
#define MAX_SPEED 190 // sets speed of DC  motors
#define MAX_SPEED_OFFSET 20

int distance = 100;
String dis = "100";

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

 
  const char* ssid     = "IOTCLOUD";
  const char* password = "12345678";

String pot_hole = "0";
unsigned long lastTriggerTime = 0;
unsigned long debounceDelay = 2000; // 2 seconds
// ************** Initial Setup Code *********************************************** 
void setup() { 

  Serial.begin(115200);
  ss.begin(GPSBaud);
  
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
lcd.begin();
      lcd.backlight();
      lcd.print("LCD Init..!");
      delay(1000);
      // Scroll message across first line
        for (int i = 0; i < message.length() - 15; i++) {
          lcd.setCursor(0, 0);
          lcd.print(message.substring(i, i + 16));  // Display 16 characters at a time
          delay(Scrolling_Speed);  // Scrolling_Speed
        }
        delay(1000);
        lcd.clear();

            lcd.setCursor(0,0);
            lcd.print("Initilized       ");
            lcd.setCursor(0,1);
            lcd.print(" Sucessfully     ");
            lcd.clear();
            delay(1000);
            Serial.println("Init..,");
  WiFi.mode(WIFI_STA);
  Wi_Fi_Inti();

  lcd.setCursor(0, 0);
  lcd.print("Wait for URL..,        ");

  //Initialize NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);


}




// ************** Main code , to run repeatedly  *********************************************** 
void loop() {

  getLocation();
/************************************Wifi IP Web Page - START ******************************/
if (Gps_Getting == 1)
{
  
  WiFiClient client = server.available();   // Listen for incoming clients
  if (client) {                             // If a new client connects,
    //Serial.println("New Client.");          // print a message out in the //Serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();

      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the //Serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

/************************************************* Download Excel CSV ***********************************************/
            if (header.indexOf("GET /csv") >= 0)
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/csv");
              client.println("Content-Disposition: attachment; filename=sensor_data.csv");
              client.println("Connection: close");
              client.println();
              client.println(csvData);
              client.stop();
              lcd.setCursor(0, 0);
              lcd.print("Excel Download   ");
              lcd.setCursor(0, 1);
              lcd.print("                  ");
              interval();
            }
/************************************************* Seperate Graph and Excel ***********************************************/
            if (header.indexOf("GET /graph") >= 0)
            {
              // Serve HTML with charts
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");
              client.println();
              client.println("<!DOCTYPE html>");
              client.println("<html>");
              client.println("<head>");
              client.println("<meta http-equiv='refresh' content='1'>"); // refresh every 1 second
              client.println("<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>");
              client.println("<style>");
              client.println("body{margin:0;font-family:Arial;background:linear-gradient(120deg,#1e3c72,#2a5298);color:white;text-align:center;}");
              client.println(".header{background:orange;color:black;padding:15px;font-size:28px;font-weight:bold;}");
              client.println(".container{width:95%;margin:auto;}");
              client.println(".card{background:white;color:black;width:180px;margin:10px;padding:15px;border-radius:10px;display:inline-block;}");
              client.println(".btn{background-color:#4CAF50;color:white;padding:12px 24px;font-size:16px;border:none;border-radius:8px;cursor:pointer;margin:5px;}");
              client.println(".btn:hover{background-color:#45a049;transform:scale(1.05);}");
              client.println(".chartBox{background:white;margin-top:20px;padding:15px;border-radius:10px;}");
              client.println("</style>");
              client.println("</head>");
              client.println("<body>");
              client.println("<div class='header'>" + message + "</div>");
              client.println("<div class='container'>");

              // Buttons
              client.println("<div class='cards'>");

              client.println("<div class='card'>");
              client.println("<a href='/'><button class='btn'>Back</button></a>");
              client.println("</div>");

              client.println("<div class='card'>");
              client.println("<a href='/csv'><button class='btn'>Download Excel</button></a>");
              client.println("</div>");

              client.println("</div>");


              // Start JavaScript
              client.println("<script>");
              // Load CSV
              client.println("let csvData = `" + csvData + "`;");
              client.println("let rows = csvData.trim().split('\\n');");

              // Extract headers (sensor names)
              client.println("let headers = rows[0].split(',');");

              // Data rows (without header)
              client.println("let dataRows = rows.slice(1);");

              // Extract time (X-axis)
              client.println("let labels = dataRows.map(r => r.split(',')[0].split(' ')[1]);");

              // Loop through sensors automatically
              client.println("for (let i = 1; i < headers.length; i++) {");

              // Create title
              client.println("  let title = document.createElement('h3');");
              client.println("  title.innerText = headers[i];");
              client.println("  document.querySelector('.container').appendChild(title);");

              // Create canvas
              client.println("  let canvas = document.createElement('canvas');");
              client.println("  canvas.id = 'chart' + i;");
              client.println("  canvas.style.backgroundColor = 'white';");
              client.println("  canvas.style.borderRadius = '8px';");
              client.println("  canvas.width = 400;");
              client.println("  canvas.height = 200;");
              client.println("  document.querySelector('.container').appendChild(canvas);");

              // Extract sensor data
              client.println("  let data = dataRows.map(r => {");
              client.println("    let val = parseFloat(r.split(',')[i]);");
              client.println("    return isNaN(val) ? 0 : val;");
              client.println("  });");

              // Create chart
              client.println("  new Chart(canvas.getContext('2d'), {");
              client.println("    type: 'line',");
              client.println("    data: {");
              client.println("      labels: labels,");
              client.println("      datasets: [{");
              client.println("        label: headers[i],");
              client.println("        data: data,");
              client.println("        borderWidth: 2,");
              client.println("        tension: 0.3,");
              client.println("        fill: false");
              client.println("      }]");
              client.println("    },");
              client.println("    options: {");
              client.println("      responsive: true,");
              client.println("      plugins: {");
              client.println("        title: { display: true, text: 'Time vs ' + headers[i] }");
              client.println("      },");
              client.println("      scales: {");
              client.println("        x: { title: { display: true, text: 'Time (HH:MM:SS)' } },");
              client.println("        y: { title: { display: true, text: 'Sensor Value' }, beginAtZero: true }");
              client.println("      }");
              client.println("    }");
              client.println("  });");

              client.println("}");

              client.println("</script>");
              client.println("</div>");
              client.println("</body>");
              client.println("</html>");

              client.stop();
            }
            
/************************************************* Tower Map Location ***********************************************/
            if (header.indexOf("GET /map") >= 0) {
              handleMap(client);
              client.stop();
            }
/************************************************* Web Upload ***********************************************/

            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a conDoor-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("ConDoor-type:text/html");
            client.println("Connection: OPEN");
            client.println();

            Sensor_detection();
            interval();
            lcd.setCursor(0, 0);
            lcd.print("Web Upload...,   ");
            lcd.setCursor(0, 1);
            lcd.print("                  ");



/************************************************* Main Web Page ***********************************************/
            // HTML Page
            client.println("<!DOCTYPE html><html>");
            client.println("<head>");
            client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
            client.println("<meta http-equiv='refresh' content='1'>"); // refresh every 2 seconds
            client.println("<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>");
            client.println("<style>");
            client.println("body{margin:0;font-family:Arial;background:linear-gradient(120deg,#1e3c72,#2a5298);color:white;text-align:center;}");
            client.println(".header{background:orange;color:black;padding:15px;font-size:28px;font-weight:bold;}");
            client.println(".container{width:95%;margin:auto;}");
            client.println(".cards{display:flex;flex-wrap:wrap;justify-content:center;margin-top:20px;}");
            client.println(".card{background:white;color:black;width:180px;margin:10px;padding:15px;border-radius:10px;box-shadow:0px 5px 15px rgba(0,0,0,0.4);}");
            client.println(".sensorTitle{font-size:18px;font-weight:bold;}");
            client.println(".sensorValue{font-size:26px;margin-top:8px;}");
            client.println(".good{color:green;font-weight:bold;}");
            client.println(".warning{color:red;font-weight:bold;}");
            client.println(".btn{background-color:#4CAF50;color:white;padding:12px 24px;font-size:16px;border:none;border-radius:8px;cursor:pointer;box-shadow:0 4px 6px rgba(0,0,0,0.2);transition:0.3s;}");
            client.println(".btn:hover{background-color:#45a049;transform:scale(1.05);}");
            client.println("table{width:100%;margin-top:20px;border-collapse:collapse;background:white;color:black;border-radius:10px;}");
            client.println("th{background:#2a5298;color:white;padding:10px;font-size:20px;}");
            client.println("td{padding:10px;border-bottom:1px solid #ddd;font-size:18px;}");
            client.println(".chartBox{background:white;margin-top:20px;padding:15px;border-radius:10px;}");

            client.println("</style>");
            client.println("</head>");

            client.println("<body>");
            client.println("<div class='header'>" + message + "</div>");
            client.println("<div class='container'>");

/************************************************* SENSOR CARDS ***********************************************/

            client.println("<div class='cards'>");

            client.println("<div class='card'>");
            client.println("<div class='sensorTitle'>ULTRASONIC</div>");
            client.println("<div class='sensorValue'>" + String(distance) + "CM</div>");
            client.println("</div>");

            client.println("<div class='card'>");
            client.println("<div class='sensorTitle'>Pot_Hole</div>");
            client.println("<div class='sensorValue'>" + String(pot_hole) + "</div>");
            client.println("</div>");

            
            client.println("</div>");

/************************************************* SENSOR TABLE ***********************************************/

            client.println("<table>");

            client.println("<tr>");
            client.println("<th>Sensor</th>");
            client.println("<th>Value</th>");
            client.println("<th>Status</th>");
            client.println("</tr>");

            client.println("<tr>");
            client.println("<td>ULTRASONIC</td>");
            client.println("<td>" + String(distance) + "CM</td>");

            if (distance >= 10 && distance <= 15)
              client.println("<td class='warning'>WARNING</td>");
            else
              client.println("<td class='good'>GOOD</td>");

            client.println("</tr>");

            client.println("<tr>");
            client.println("<td>Pot_Hole</td>");
            client.println("<td>" + String(pot_hole) + "</td>");

            if (pot_hole == "1")
              client.println("<td class='warning'>WARNING</td>");
            else
              client.println("<td class='good'>GOOD</td>");

            client.println("</tr>");



            client.println("</table>");
/************************************************* SENSOR TABLE ***********************************************/


/************************************************* OVER ALL SENSOR GRAPH ***********************************************/
            client.println("<div class='chartBox'>");
            client.println("<canvas id='sensorChart'></canvas>");
            client.println("</div>");

            client.println("<script>");
            client.println("var ctx=document.getElementById('sensorChart').getContext('2d');");
            client.println("var chart=new Chart(ctx,{");
            client.println("type:'line',");
            client.println("data:{");

            client.println("labels:['ULTRASONIC'],");

            client.println("datasets:[{");
            client.println("label:'Sensor Data',");
            client.println("data:[");

            client.println(String(distance));

            client.println("],");
            client.println("backgroundColor:['green']");
            client.println("}]");
            client.println("}");
            client.println("});");
            client.println("</script>");

            

/************************************************* MAIN PAGE GRAPH AND LOCATION BUTTON **********************************************/            
         
                client.println("<div class='cards'>");
                // Download Button
                client.println("<div class='card'>");
                client.println("<a href='/graph' ><button class='btn'>View Graph</button></a>");
                client.println("</div>");

                // MAP Button
                client.println("<div class='card'>");
                client.println("<a href='/map'><button class='btn'>View Location</button></a>");
                client.println("</div>");

                client.println("</div>");
       
            
/************************************************* MAIN PAGE GRAPH AND LOCATION BUTTON END ***********************************************/

            client.println("</div>");

            /*  if (statu != "Normal")
              {
                client.println("<img src='http://wapifast.bigtgs.in/api.php?type=text-message&token=0fd253f501c68ced0ac2&instance=uBNXPBvVEUqM&phone=91" + Ph_no + "&message= " + statu + " 'style='display:none;'>");
              }*/

            client.println("</body>");
            client.println("</html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    header = "";
    // OPEN the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Web Not           ");
    lcd.setCursor(0, 1);
    lcd.print("    Upload...,    ");
    interval();
    Sensor_detection();
  }
}
/************************************Wifi IP Web Page - END ******************************/





}
// ************** Main code END  *********************************************** 

int readPing() { 
  delay(70);
  int cm = sonar.ping_cm();
  if(cm==0)
  {
    cm = 250;
  }
  return cm;
}
void interval() {
  delay(800);
  lcd.clear();
}
void Sensor_detection() {
  
getLocation();

      
      if (lat == 0)
      {
          lat = D_lat;
          lon = D_lon; 
      }else{
          lat = gps.location.lat(), 6;
          lon = gps.location.lng(), 6;
      }
        lcd.setCursor(0,0);
        lcd.print("Lat: ");
        lcd.print(lat, 6);
        lcd.setCursor(0,1);
        lcd.print("Lon: ");
        lcd.print(lon, 6);
        interval();

        distance = readPing();
        lcd.setCursor(0,0);
        lcd.print(" US = ");
        lcd.print(distance);
        lcd.print("  ");
        interval();

         if(distance >= 10 && distance <= 15 && pot_hole == "0") {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(" -DAMAGED ROAD - ");
        lcd.setCursor(0,1);
        lcd.print("   -POTHOLE-     ");

        pot_hole = "1";                 // Set flag
        lastTriggerTime = millis();     // Store time
        interval();
    }

    // Debounce reset condition
    if(pot_hole == "1" && (millis() - lastTriggerTime > debounceDelay)) {
        pot_hole = "0";   // Reset after delay
        lcd.clear();
    }

        
// Save Data every seconds
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeinfo);
  // String Map_URL = "https://maps.google.com/?q=" + String(lat) + "," + String(lon) +"&z=15"
  String row = String(timestamp) + "," +
               String(pot_hole) + "," +
               String(distance) + "," +
               String(lat) + "," +
               String(lon) + "\n";
  
  csvData += row;
}

void getLocation() {
 Serial.println(ss.read());
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0)
    if (gps.encode(ss.read()))

  if (gps.location.isValid())
  {
      Serial.print("Lat: ");
      Serial.println(gps.location.lat(), 6);
      Serial.print("Lon: ");
      Serial.println(gps.location.lng(), 6);
      lat = gps.location.lat(), 6;
      lon = gps.location.lng(), 6;
    Gps_Getting = 1;
  }
  else
  {

    Serial.println(F("INVALID"));
    Gps_Checking = Gps_Checking + 1;
    if(Gps_Checking >= 10)
    {
      Gps_Getting = 1;
    }
    
  }


}


void handleMap(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html><html><head>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
  client.println("<link rel='stylesheet' href='https://unpkg.com/leaflet/dist/leaflet.css'/>");
  client.println("</head><body>");
  client.println("<h2 style='text-align:center;'>Road Damage Location</h2>");
  client.println("<div id='map' style='height:400px;'></div>");
  client.println("<script src='https://unpkg.com/leaflet/dist/leaflet.js'></script>");
  client.println("<script>");
  client.println("var map = L.map('map').setView([" + String(lat, 6) + "," + String(lon, 6) + "], 16);");
  client.println("L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png',{maxZoom:19}).addTo(map);");
  client.println("L.marker([" + String(lat, 6) + "," + String(lon, 6) + "]).addTo(map)");
  client.println(".bindPopup('Pothole Location').openPopup();");
  client.println("</script></body></html>");
}


void Wi_Fi_Inti()
{
  WiFi.begin(ssid, password);

  if (WiFi.status() != WL_CONNECTED) {
    lcd.setCursor(0, 0);
    lcd.print("SSID = ");
    lcd.print(ssid);
    lcd.print("             ");
    lcd.setCursor(0, 1);
    lcd.print("PASS = ");
    lcd.print(password);
    lcd.print("              ");
    delay(5000);
    lcd.clear();
    while (WiFi.status() != WL_CONNECTED ) {
      WiFi.begin(ssid, password);
      lcd.setCursor(0, 0);
      lcd.print("  Searching...");
      
      if(Connection_Count <= 15)
      {
        lcd.setCursor(0, 1);
        lcd.print("Checking = ");
        lcd.print(Connection_Count);
        lcd.print("       ");
      }else{
        lcd.setCursor(0, 0);
        lcd.print("     WiFi      ");
        lcd.setCursor(0, 1);
        lcd.print(" Not Connected...        ");
        delay(5000);
        return;
      }
      Connection_Count = Connection_Count + 1;
      delay(2000);
    }
    lcd.setCursor(0, 0);
    lcd.print(" Connected...,        ");
    delay(5000);
    delay(5000);
    lcd.clear();
    


    lcd.setCursor(0, 0);
    lcd.print("IP address:");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    delay(5000);
    delay(5000);

    server.begin();

  }
}


