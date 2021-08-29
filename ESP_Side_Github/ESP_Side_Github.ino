//************************************************
//Author  : Kristian Noprianto (Telkom University)
//Purpose : (ESP side)
//********** **************************************


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP_Mail_Client.h>
#include <ArduinoJson.h>

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

#define AUTHOR_EMAIL "hidroverti@gmail.com"
#define AUTHOR_PASSWORD "HidroVerti123-"

#define RECIPIENT_EMAIL "kristiannoprianto66@gmail.com"

SMTPSession smtp;

String htmlMsg;

const char* ssid = "maumandi";//type your ssid
const char* password = "rtzA.560--@";//type your password
float pHval;
float Turbval;
float temp;
float TDSval;
float fst;
float sec;
float trd;
int sts;
#define startMarker '<'
#define endMarker '\n'
float sBuffer[40];
bool loading = true;
bool newData = false;
byte indeX = 0;
byte loadingLen = 0;
int State;

ESP8266WebServer server(80);
 
void setup() {
  Serial.begin(115200);
  delay(10);

  while(!Serial){
    ;
  }

  Serial.println("Go!");
   
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
   
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
   
  // Start the server
  server.on("/updateSenValue", handleRoot);
  server.on("/updateStateON", handleON);
  server.on("/updateStateOFF", handleOFF);
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
    
}
 
void loop() {
  recvData();
  server.handleClient();
}

typedef union{
  float number;
  uint8_t bytes[4];
}FLOATUNION_t;

void decodeData(){
  FLOATUNION_t myFloat;
  FLOATUNION_t myFloat1;
  FLOATUNION_t myFloat2;
  FLOATUNION_t myFloat3;
  FLOATUNION_t myFloat4;
  FLOATUNION_t myFloat5;
  FLOATUNION_t myFloat6;
  FLOATUNION_t myFloat7;
  
  byte head = sBuffer[0];
  if(head == 0x10){
    myFloat.bytes[0] = sBuffer[1];
    myFloat.bytes[1] = sBuffer[2];
    myFloat.bytes[2] = sBuffer[3];
    myFloat.bytes[3] = sBuffer[4];
    head = sBuffer[5];
    if(head == 0x11){
      myFloat1.bytes[0] = sBuffer[6];
      myFloat1.bytes[1] = sBuffer[7];
      myFloat1.bytes[2] = sBuffer[8];
      myFloat1.bytes[3] = sBuffer[9];
      head = sBuffer[10];
      if(head == 0x12){
        myFloat2.bytes[0] = sBuffer[11];
        myFloat2.bytes[1] = sBuffer[12];
        myFloat2.bytes[2] = sBuffer[13];
        myFloat2.bytes[3] = sBuffer[14];
        head = sBuffer[15];
        if(head == 0x13){
          myFloat3.bytes[0] = sBuffer[16];
          myFloat3.bytes[1] = sBuffer[17];
          myFloat3.bytes[2] = sBuffer[18];
          myFloat3.bytes[3] = sBuffer[19];
          head = sBuffer[20];
          if(head == 0x14){
            myFloat4.bytes[0] = sBuffer[21];
            myFloat4.bytes[1] = sBuffer[22];
            myFloat4.bytes[2] = sBuffer[23];
            myFloat4.bytes[3] = sBuffer[24];
            head = sBuffer[25];
            if(head == 0x15){
              myFloat5.bytes[0] = sBuffer[26];
              myFloat5.bytes[1] = sBuffer[27];
              myFloat5.bytes[2] = sBuffer[28];
              myFloat5.bytes[3] = sBuffer[29];
              head = sBuffer[30];
              if(head == 0x16){
                myFloat6.bytes[0] = sBuffer[31];
                myFloat6.bytes[1] = sBuffer[32];
                myFloat6.bytes[2] = sBuffer[33];
                myFloat6.bytes[3] = sBuffer[34];
                head = sBuffer[35];
                if(head == 0x17){
                  myFloat7.bytes[0] = sBuffer[36];
                  myFloat7.bytes[1] = sBuffer[37];
                  myFloat7.bytes[2] = sBuffer[38];
                  myFloat7.bytes[3] = sBuffer[39];
                }
              }
            }
          }
        }
      }
    }
  }
  pHval=myFloat.number;
  Turbval=myFloat1.number;
  temp=myFloat2.number;
  TDSval=myFloat3.number;
  fst=myFloat4.number;
  sec=myFloat5.number;
  trd=myFloat6.number;
  sts=myFloat7.number;
  if(sts == 2){
      Notification(1);
    }
    else if(sts == 4){
      Notification(2);
    }
    else if(sts == 5){
      Notification(3);
    }
    else if(sts == 6){
      Notification(4);
    }
    else if(sts == 7){
      Notification(5);
    }
    else if(sts == 8){
      Notification(6);
    }
}

void recvData(){
  while(Serial.available()>0){
    float sensorDat = Serial.read();
    
    if(loading){
      if(sensorDat != endMarker){
        sBuffer[indeX]=sensorDat;
        indeX++;
      }
      else{
        loading = false;
        newData = true;
        loadingLen = indeX;
        indeX = 0;
      }
    }
    else if(sensorDat == startMarker){
      loading = true;
      newData= false;
    }
  }
  if(newData){
    decodeData();
    newData=false;
  }
}

void handleON(){
  State=1;
  Serial.print(State);
  server.send(200, "text/plain", "success ON");
}

void handleOFF(){
  State=0;
  Serial.print(State);
  server.send(200, "text/plain", "success OFF");
}

void handleRoot() {
  String webPage;
  StaticJsonDocument<1024> doc;

  doc["PHV"] = pHval;
  doc["TRB"] = Turbval;
  doc["TEM"] = temp;
  doc["TDS"] = TDSval;
  doc["FST"] = fst;
  doc["SEC"] = sec;
  doc["TRD"] = trd;
  doc["STS"] = sts;
  serializeJson(doc, webPage);
  server.send(200, "application/json", webPage);
}

void Notification(int i){
  smtp.debug(1);

  ESP_Mail_Session session;

  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  SMTP_Message message;

  message.sender.name = "Hidroponik Vertikultur";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "Notifikasi dari Hidroponik Vertikultur";
  message.addRecipient("Kristian", RECIPIENT_EMAIL);

  if(i == 1){
    htmlMsg = "<div style=\"color:#2f4468;\"><h1>Halo!</h1><h3>Suhu air hidroponik melebihi batas</h3></div>";
  }
  else if(i == 2){
    htmlMsg = "<div style=\"color:#2f4468;\"><h1>Halo!</h1><h3>Air pada bak hidroponik dalam keadaan keruh</h3></div>";
  }
  else if(i == 3){
    htmlMsg = "<div style=\"color:#2f4468;\"><h1>Halo!</h1><h3>Proses pengurasan bak sedang berjalan</h3></div>";
  }
  else if(i == 4){
    htmlMsg = "<div style=\"color:#2f4468;\"><h1>Halo!</h1><h3>Bak hidroponik belum terisi</h3></div>";
  }
  else if(i == 5){
    htmlMsg = "<div style=\"color:#2f4468;\"><h1>Halo!</h1><h3>Bak AB Mix belum terisi</h3></div>";
  }
  else if(i == 6){
    htmlMsg = "<div style=\"color:#2f4468;\"><h1>Halo!</h1><h3>Bak pH Buffer belum terisi</h3></div>";
  }
  
  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  if (!smtp.connect(&session))
    return;

  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
}
