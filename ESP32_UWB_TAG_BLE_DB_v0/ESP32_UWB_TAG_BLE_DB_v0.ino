#include <WiFi.h>
#include <HTTPClient.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000.h"
#include "link.h"

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4

#define PIN_RST 27
#define PIN_IRQ 34

char tag_addr[] = "7D:00:22:EA:82:60:3B:9C";

struct MyLink *uwb_data;
struct UWB_ANCHOR *uwb_anchor;
int index_num = 0;
long runtime = 0;
String all_json = "";

// Replace with your network credentials
//const char* ssid     = "Rexoes";
//const char* password = "ue057921236";

const char* ssid     = "Elik_Family_2.4GHz";
const char* password = "057921236";

// REPLACE with your Domain name and URL path or IP address with path
const char* serverName = "http://192.168.10.35/post-esp-data.php";

// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key 
String apiKeyValue = "tPmAT5Ab3j7F9";

String sensorName = "UWB";
String sensorLocation = "Office";

//const uint16_t UWB_ANCHOR_HEX_ADDR[] = {1780, 1781, 1782, 1783, 1784, 1785, 1786, 1787};
uint16_t UWB_ANCHOR_DEC_ADDR[] = {6016, 6017, 6018, 6019, 6020, 6021, 6022, 6023};

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
BLECharacteristic* pCharacteristic_2 = NULL; //Mobil'den gelen veri için
BLEDescriptor *pDescr;
BLE2902 *pBLE2902;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_2 "09da84bf-b749-4d41-8de3-2b0a8ea46438" //Mobil'den gelen veri için

#define BLE_DEVICE_NAME "ESP32_UWB_TAG"

typedef struct{
  byte ID;
  byte POS_X;
  byte POS_Y;
  byte GRID;
  byte SAMPLE_RATE;
  bool SCAN_ACTIVE;
}UWB_TAG;
UWB_TAG TAG_INFO = {7, 0, 0, 0, 30, false};

bool deviceConnected = false;
bool oldDeviceConnected = false;
unsigned long timer = 0;
int sample_time = 2000;

void BLE_INIT(void);
void BLE_WRITE(String);
void BLE_CHECK_CONNECTION(void);
void BLE_DATA_PROCESS(String);
void WiFi_Init();
void LAMP_POST();

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      // İstemci bağlandığında yapılacak işlemler
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      // İstemci bağlantısı kesildiğinde yapılacak işlemler
      deviceConnected = false;
    }
};

/* Mobil Uygulamadan Gelen Configuration Verileri
 * 1. UWB TAG Position Set    -> "P0,2"  : TAG_X = 0, TAG_Y = 2
 * 2. UWB TAG Grid Set        -> "G15"   : TAG_GRID = 15
 * 3. UWB TAG Sample Rate Set -> "S1,30" : TAG_SAMPLE_RATE = 30, SampleStart / "S0" : SampleStop
 */

class CharacteristicsCallBack: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pChar) override {
    std::string pChar2_value_stdstr = pChar->getValue();
    
    String ble_data = String(pChar2_value_stdstr.c_str());
    //int pChar2_value_int = pChar2_value_string.toInt();
    
    Serial.println("pChar2: " + ble_data);
    BLE_DATA_PROCESS(ble_data);
  }
};

void setup() {
  Serial.begin(115200);
  BLE_INIT();
  WiFi_Init();
  
  //init the configuration
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    DW1000Ranging.initCommunication(PIN_RST, DW_CS, PIN_IRQ);
    DW1000Ranging.attachNewRange(newRange);
    DW1000Ranging.attachNewDevice(newDevice);
    DW1000Ranging.attachInactiveDevice(inactiveDevice);

    //we start the module as a tag
    DW1000Ranging.startAsTag(tag_addr, DW1000.MODE_LONGDATA_RANGE_LOWPOWER);

    uwb_data = init_link();
    
    uwb_anchor = (struct UWB_ANCHOR *)malloc(sizeof(struct UWB_ANCHOR));
    memcpy(uwb_anchor->ANCHOR_DEC_ADDR, UWB_ANCHOR_DEC_ADDR, sizeof(UWB_ANCHOR_DEC_ADDR));
    uwb_anchor->ANCHOR_SAMPLE_RATE = 0;
}

void loop() {
  yield();

  DW1000Ranging.loop();
//  if((millis() - runtime) > 1000){
//      make_link_json(uwb_data, &all_json);
//      make_link_http_post(uwb_data, uwb_anchor);
//      DISPLAY_UWB_DATA();
//      LAMP_POST();
//      print_link(uwb_data);
//      runtime = millis();
//  }

  if((millis() - runtime) > 1000){
    if(deviceConnected){
      if(TAG_INFO.SCAN_ACTIVE){
        if(!isLinkEmpty(uwb_data)){
          if(uwb_anchor->ANCHOR_SAMPLE_RATE < TAG_INFO.SAMPLE_RATE){
            make_link_json(uwb_data, &all_json);
            make_link_http_post(uwb_data, uwb_anchor);
            DISPLAY_UWB_DATA();
            //LAMP_POST();
            print_link(uwb_data);
            Serial.println("Kalan Ölçüm Adedi: " + String(TAG_INFO.SAMPLE_RATE - uwb_anchor->ANCHOR_SAMPLE_RATE - 1));
          }
          else{
            Serial.println("Tarama Bitti!");
            BLE_WRITE("UWB Scan Ended");
            uwb_anchor->ANCHOR_SAMPLE_RATE = 0;
            TAG_INFO.SCAN_ACTIVE = false;
          }
  
          uwb_anchor->ANCHOR_SAMPLE_RATE++;
        }
        else
          BLE_WRITE("No Anchors Found");
      }
    }
    runtime = millis();
  }

  
  BLE_CHECK_CONNECTION();
}

void newRange(){
    Serial.print("from: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
    Serial.print("\t Range: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getRange());
    Serial.print(" m");
    Serial.print("\t RX power: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getRXPower());
    Serial.println(" dBm");
    fresh_link(uwb_data, DW1000Ranging.getDistantDevice()->getShortAddress(), DW1000Ranging.getDistantDevice()->getRange(), DW1000Ranging.getDistantDevice()->getRXPower());
    //uwb_anchor->ANCHOR_SAMPLE_RATE++;
}

void newDevice(DW1000Device *device){
    Serial.print("ranging init; 1 device added ! -> ");
    Serial.print(" short:");
    Serial.println(device->getShortAddress(), HEX);
    Serial.println("ADDR: " + String(device->getShortAddress()));

    if(UWB_ANCHOR_DEC_ADDR[2] == device->getShortAddress()){
      Serial.println("Anchor-2 için DEC ADDR");
      Serial.println("Anchor-2 ADDR: " + String(device->getShortAddress()) + "| DEC ADDR: " + UWB_ANCHOR_DEC_ADDR[2]);
    }
    
    add_link(uwb_data, device->getShortAddress());
}

void inactiveDevice(DW1000Device *device){
    Serial.print("delete inactive device: ");
    Serial.println(device->getShortAddress(), HEX);

    delete_link(uwb_data, device->getShortAddress());
}



//{"links":[{"A":"1782","R":"1.0"}]}
void LAMP_POST(){
  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Prepare your HTTP POST request data
  String httpRequestData = "api_key=" + apiKeyValue + "&TAG_ID=" + String(TAG_INFO.ID)
                        + "&TAG_POS_X=" + String(TAG_INFO.POS_X) + "&TAG_POS_Y=" + String(TAG_INFO.POS_Y)
                        + "&TAG_GRID=" + String(TAG_INFO.GRID) + "&A0=" + String(uwb_anchor->Anchor_0) + "&A1=" + String(uwb_anchor->Anchor_1)
                        + "&A2=" + String(uwb_anchor->Anchor_2) + "&A3=" + String(uwb_anchor->Anchor_3) + "&A4=" + String(uwb_anchor->Anchor_4)
                        + "&A5=" + String(uwb_anchor->Anchor_5) + "&A6=" + String(uwb_anchor->Anchor_6) + "&A7=" + String(uwb_anchor->Anchor_7)
                        + "&LOCATION=" + sensorLocation + "";
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);
    
    // You can comment the httpRequestData variable above
    // then, use the httpRequestData variable below (for testing purposes without the BME280 sensor)
    //String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&location=Office&value1=24.75&value2=49.54&value3=1005.14";

    //yield();
    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
     
    // If you need an HTTP request with a content type: text/plain
    //http.addHeader("Content-Type", "text/plain");
    //int httpResponseCode = http.POST("Hello, World!");
    
    // If you need an HTTP request with a content type: application/json, use the following:
    //http.addHeader("Content-Type", "application/json");
    //int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");
        
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      BLE_WRITE("LAMP POST + (RS: " + String((TAG_INFO.SAMPLE_RATE - uwb_anchor->ANCHOR_SAMPLE_RATE - 1)) + ")");
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      BLE_WRITE("LAMP POST Failed");
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
    BLE_WRITE("WiFi Disconnected");
  }
}

void BLE_INIT(){
  // Create the BLE Device
  BLEDevice::init(BLE_DEVICE_NAME);

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );                   

  //Mobil'den gelen veri için
  pCharacteristic_2 = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_2,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE
                    ); 
                    
  // Create a BLE Descriptor
  
  pDescr = new BLEDescriptor((uint16_t)0x2901);
  pDescr->setValue("A very interesting variable");
  pCharacteristic->addDescriptor(pDescr);
  
  pBLE2902 = new BLE2902();
  pBLE2902->setNotifications(true);
  pCharacteristic->addDescriptor(pBLE2902);

  //Mobil'den gelen veri için
  pCharacteristic_2->addDescriptor(new BLE2902());
  pCharacteristic_2->setCallbacks(new CharacteristicsCallBack());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

/* ESP32 BLE Veri Gönderme
 * 1. setValue(): Bu yöntemle karakteristiğin değeri ayarlanır. Ardından, pCharacteristic->notify(); ile bağlı istemcilere bu değer bildirilir.
 * 
 *    pCharacteristic->setValue("Hello World");
 *    pCharacteristic->notify();
 *    delay(1000);
 * 
 * 2. writeValue(): Bu yöntemle karakteristiğin değeri ayarlanır ve bağlı istemcilere bildirim yapılmadan direkt olarak değer gönderilir.
 * 
 *    pCharacteristic->writeValue("Hello World");
 *    delay(1000);
 */
void BLE_WRITE(String value){
  pCharacteristic->setValue(value.c_str());
  pCharacteristic->notify();
  
  //String str = "Hello";
  //pCharacteristic->writeValue(str.c_str(), str.length());
}

void BLE_CHECK_CONNECTION(){
  // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        Serial.println("Connection established with BLE device");
        oldDeviceConnected = deviceConnected;
    }
}

void BLE_DATA_PROCESS(String BLE_DATA){
  char configType = BLE_DATA.charAt(0);
    BLE_DATA = BLE_DATA.substring(1);
    BLE_DATA += ",";
    String strTemp = "";
    byte index = 0;
    bool boolTemp = false;
    
    if(configType == 'P'){
      //Value -> "0,2,"  : TAG_X = 0, TAG_Y = 2
      for(byte i = 0; i < BLE_DATA.length(); i++){
        char ch = BLE_DATA.charAt(i);
        if(ch == ','){
          if(index == 0)
            TAG_INFO.POS_X = strTemp.toInt();
          else if(index == 1)
            TAG_INFO.POS_Y = strTemp.toInt();
          strTemp = "";
          index++;
        }
        else{
          strTemp += ch;
        }
      }
      //GRID = 14*X + Y ; 0,0 -> 0 | 0,5 -> 5 | 2,7 -> 35
      TAG_INFO.GRID = (14 * TAG_INFO.POS_X) + TAG_INFO.POS_Y;
      Serial.println("UWB TAG Position Setting to: (" + String(TAG_INFO.POS_X) + ", " + String(TAG_INFO.POS_Y) + ") | GRID-" + String(TAG_INFO.GRID));
    }
    else if(configType == 'G'){
      //Value -> "15,"   : TAG_GRID = 15
      for(byte i = 0; i < BLE_DATA.length(); i++){
        char ch = BLE_DATA.charAt(i);
        if(ch != ',')
          strTemp += ch;
        else
          TAG_INFO.GRID = strTemp.toInt();
      }
      TAG_INFO.POS_X = TAG_INFO.GRID / 14;
      TAG_INFO.POS_Y = TAG_INFO.GRID % 14;
      Serial.println("UWB TAG Grid Setting to: Grid-" + String(TAG_INFO.GRID) + "| (" + String(TAG_INFO.POS_X) + ", " + String(TAG_INFO.POS_Y) + ")");
    }
    else if(configType == 'S'){
      if(BLE_DATA.charAt(0) == '0'){
        TAG_INFO.SCAN_ACTIVE = false;
        uwb_anchor->ANCHOR_SAMPLE_RATE = 0;
      }
      else if(BLE_DATA.charAt(0) == '1'){
        //Value -> "1,30," : TAG_SAMPLE_RATE = 30, SampleStart / "S0" : SampleStop
        for(byte i = 0; i < BLE_DATA.length(); i++){
          char ch = BLE_DATA.charAt(i);
          if(ch == ','){
            if(index == 0)
              if(strTemp == "1")
                TAG_INFO.SCAN_ACTIVE = true;
              else
                TAG_INFO.SCAN_ACTIVE = false;
            else if(index == 1)
              TAG_INFO.SAMPLE_RATE = strTemp.toInt();
            strTemp = "";
            index++;
          }
          else{
            strTemp += ch;
          }
        }
      }
      if(TAG_INFO.SCAN_ACTIVE)
        Serial.println("UWB TAG Scan Mode: Active\nUWB TAG Sample Rate: " + String(TAG_INFO.SAMPLE_RATE));
      else
        Serial.println("UWB TAG Scan Mode: Passive");
    }
}

void DISPLAY_UWB_DATA(){
  Serial.println("Make Link HTTP POST Data;");
  Serial.println("Anchor_0 -> " + String(uwb_anchor->Anchor_0));
  Serial.println("Anchor_1 -> " + String(uwb_anchor->Anchor_1));
  Serial.println("Anchor_2 -> " + String(uwb_anchor->Anchor_2));
  Serial.println("Anchor_3 -> " + String(uwb_anchor->Anchor_3));
  Serial.println("Anchor_4 -> " + String(uwb_anchor->Anchor_4));
  Serial.println("Anchor_5 -> " + String(uwb_anchor->Anchor_5));
  Serial.println("Anchor_6 -> " + String(uwb_anchor->Anchor_6));
  Serial.println("Anchor_7 -> " + String(uwb_anchor->Anchor_7));
}

void WiFi_Init(){
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}
