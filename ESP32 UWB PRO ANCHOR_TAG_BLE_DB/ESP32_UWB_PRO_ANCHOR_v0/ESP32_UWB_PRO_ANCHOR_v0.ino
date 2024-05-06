//For ESP32 UWB or ESP32 UWB Pro Anchor

#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000.h"

//Farklı ANCHOR'lar için ADDR değerini belirle!

//#define UWB_ANCHOR_0_ADDR "80:17:5B:D5:A9:9A:E2:9C"  //Anchor-0 DEC_ADDR: 6016 Short_ADDR: 1780
//#define UWB_ANCHOR_1_ADDR "81:17:5B:D5:A9:9A:E2:9C"  //Anchor-1 DEC_ADDR: 6017 Short_ADDR: 1781
//#define UWB_ANCHOR_2_ADDR "82:17:5B:D5:A9:9A:E2:9C"  //Anchor-2 DEC_ADDR: 6018 Short_ADDR: 1782
//#define UWB_ANCHOR_3_ADDR "83:17:5B:D5:A9:9A:E2:9C"  //Anchor-3 DEC_ADDR: 6019 Short_ADDR: 1783
//#define UWB_ANCHOR_4_ADDR "84:17:5B:D5:A9:9A:E2:9C"  //Anchor-4 DEC_ADDR: 6020 Short_ADDR: 1784
//#define UWB_ANCHOR_5_ADDR "85:17:5B:D5:A9:9A:E2:9C"  //Anchor-5 DEC_ADDR: 6021 Short_ADDR: 1785
#define UWB_ANCHOR_6_ADDR "86:17:5B:D5:A9:9A:E2:9C"  //Anchor-6 DEC_ADDR: 6022 Short_ADDR: 1786
//#define UWB_ANCHOR_7_ADDR "87:17:5B:D5:A9:9A:E2:9C"  //Anchor-7 DEC_ADDR: 6023 Short_ADDR: 1787

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4

// connection pins
const uint8_t PIN_RST = 27; // reset pin
const uint8_t PIN_IRQ = 34; // irq pin
const uint8_t PIN_SS = 4;   // spi select pin

void setup()
{
    Serial.begin(115200);
    delay(1000);
    //init the configuration
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin
    //define the sketch as anchor. It will be great to dynamically change the type of module
    DW1000Ranging.attachNewRange(newRange);
    DW1000Ranging.attachBlinkDevice(newBlink);
    DW1000Ranging.attachInactiveDevice(inactiveDevice);
    //Enable the filter to smooth the distance
    //DW1000Ranging.useRangeFilter(true);

    //we start the module as an anchor
    // DW1000Ranging.startAsAnchor("82:17:5B:D5:A9:9A:E2:9C", DW1000.MODE_LONGDATA_RANGE_ACCURACY);

    DW1000Ranging.startAsAnchor(UWB_ANCHOR_6_ADDR, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
    // DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_SHORTDATA_FAST_LOWPOWER);
    // DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_LONGDATA_FAST_LOWPOWER);
    // DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_SHORTDATA_FAST_ACCURACY);
    // DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_LONGDATA_FAST_ACCURACY);
    // DW1000Ranging.startAsAnchor(UWB_ANCHOR_0_ADDR, DW1000.MODE_LONGDATA_RANGE_ACCURACY, false); //En doğru mesafe bilgisi elde etmek için kullanılabilir!
}

void loop()
{
    DW1000Ranging.loop();
}

void newRange()
{
    Serial.print("from: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
    Serial.print("\t Range: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getRange());
    Serial.print(" m");
    Serial.print("\t RX power: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getRXPower());
    Serial.println(" dBm");
}

void newBlink(DW1000Device *device)
{
    Serial.print("blink; 1 device added ! -> ");
    Serial.print(" short:");
    Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device *device)
{
    Serial.print("delete inactive device: ");
    Serial.println(device->getShortAddress(), HEX);
}
