/*
   RadioLib SX127x Receive Example

   This example listens for LoRa transmissions using SX127x Lora modules.
   To successfully receive data, the following settings have to be the same
   on both transmitter and receiver:
    - carrier frequency
    - bandwidth
    - spreading factor
    - coding rate
    - sync word
    - preamble length

   Other modules from SX127x/RFM9x family can also be used.

   For default module settings, see the wiki page
   https://github.com/jgromes/RadioLib/wiki/Default-configuration#sx127xrfm9x---lora-modem

   For full API reference, see the GitHub Pages
   https://jgromes.github.io/RadioLib/
*/

/*

Command example:

  ACT = 0 Close
    ID001ACT000PARAM000000#

  ACT = 1 All Open
    ID001ACT001PARAM000000#

  ACT = 2 PWM 
    PARAM = 0-255 Dimmer PWM 
    ID001ACT002PARAM005150

  ACT = 3 PWM DELAY ON  (delay and then close)
    PARAM % 1000 = 0-255 Dimmer PWM
    PARAM / 1000 = 0-999 Second Delay
    ID001ACT003PARAM000060

  ACT = 4 PWM DELAY OFF  (delay and then all on)
    PARAM % 1000 = 0-255 Dimmer PWM
    PARAM / 1000 = 0-999 Second Delay
    ID001ACT004PARAM005060

*/

// include the library
#include <RadioLib.h>
#include "avr/boot.h"

#define DIO0 2
#define DIO1 6
#define DIO2 7
#define DIO5 8

#define LORA_RST 9
#define LORA_CS 10

#define SPI_MOSI 11
#define SPI_MISO 12
#define SPI_SCK 13

#define LED_PIN A3  // Blinky on receipt
#define RELAY_PIN 5 // the pin that the Relay is attached to
#define TRIAC_PIN 4 // the pin that the TRIAC is attached to
#define ZCD_PIN 3   //Zero Crossing Detector

#define TRIAC_CTRL_OFF digitalWrite(TRIAC_PIN, LOW)
#define TRIAC_CTRL_ON digitalWrite(TRIAC_PIN, HIGH)
#define RELAY_OFF digitalWrite(RELAY_PIN, LOW)
#define RELAY_ON digitalWrite(RELAY_PIN, HIGH)

String node_id = String("ID") + "000004";
int id_number_length = 6;
int dim = 0; //Initial brightness level from 0 to 255, change as you like!
int delay_flag = 0;
long command_time = 0;
long delay_second = 0;
String debug_id = "IDXDEBUG";

/*
Begin method:
Carrier frequency: 434.0 MHz (for SX1276/77/78/79 and RFM96/98) or 915.0 MHz (for SX1272/73 and RFM95/97)
Bandwidth: 125.0 kHz (dual-sideband)
Spreading factor: 9
Coding rate: 4/7
Sync word: SX127X_SYNC_WORD (0x12)
Output power: 10 dBm
Preamble length: 8 symbols
Gain: 0 (automatic gain control enabled)
Other:
Over-current protection: 60 mA
Inaccessible:
LoRa header mode: explicit
Frequency hopping: disabled

*/

#define FREQUENCY 434.0
#define BANDWIDTH 125.0
#define SPREADING_FACTOR 9
#define CODING_RATE 7
#define OUTPUT_POWER 10
#define PREAMBLE_LEN 8
#define GAIN 0

// SX1278 has the following connections:
// NSS pin:   10
// DIO0 pin:  2
// RESET pin: 9
// DIO1 pin:  3
SX1278 radio = new Module(LORA_CS, DIO0, LORA_RST, DIO1);
//SX1278 radio = new Module(LORA_CS, DIO0, LORA_RST, DIO1, SPI, SPISettings());

// or using RadioShield
// https://github.com/jgromes/RadioShield
//SX1278 radio = RadioShield.ModuleA;

void setup()
{
    Serial.begin(115200);

    pin_init();

    //node_id = get_UID();

    // initialize SX1278 with default settings
    Serial.print(F("[SX1278] Initializing ... "));

    int state = radio.begin(FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, SX127X_SYNC_WORD, OUTPUT_POWER, PREAMBLE_LEN, GAIN);
    //int state = radio.begin();
    if (state == ERR_NONE)
    {
        Serial.println(F("success!"));
    }
    else
    {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true)
            ;
    }
}

void loop()
{
    //Serial.print(F("[SX1278] Waiting for incoming transmission ... "));

    String str;
    int state = radio.receive(str);

    if (state == ERR_NONE)
    {
        // packet was successfully received
        Serial.println(F("success!"));

        // print the data of the packet
        Serial.print(F("[SX1278] Data:\t\t\t"));
        Serial.println(str);

        // print the RSSI (Received Signal Strength Indicator)
        // of the last received packet
        Serial.print(F("[SX1278] RSSI:\t\t\t"));
        Serial.print(radio.getRSSI());
        Serial.println(F(" dBm"));

        // print the SNR (Signal-to-Noise Ratio)
        // of the last received packet
        Serial.print(F("[SX1278] SNR:\t\t\t"));
        Serial.print(radio.getSNR());
        Serial.println(F(" dB"));

        // print frequency error
        // of the last received packet
        Serial.print(F("[SX1278] Frequency error:\t"));
        Serial.print(radio.getFrequencyError());
        Serial.println(F(" Hz"));

        if (command_explain(str))
        {
            String back_str = node_id + " REPLY : DIM " + String(dim);
            radio.transmit(back_str);
        }
    }
    else if (state == ERR_RX_TIMEOUT)
    {
        // timeout occurred while waiting for a packet
        //Serial.println(F("timeout!"));
    }
    else if (state == ERR_CRC_MISMATCH)
    {
        // packet was received, but is malformed
        Serial.println(F("CRC error!"));
    }
    else
    {
        // some other error occurred
        Serial.print(F("failed, code "));
        Serial.println(state);
    }
}

void pin_init()
{
    pinMode(LED_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    //RELAY_OFF;
    RELAY_ON;

    pinMode(TRIAC_PIN, OUTPUT);
    TRIAC_CTRL_OFF;

    pinMode(ZCD_PIN, INPUT);
    pinMode(ZCD_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(ZCD_PIN), zero_cross_int, FALLING); //CHANGE FALLING
}

void zero_cross_int() // function to be fired at the zero crossing to dim the light
{
    // if (Serial.available())
    // {
    //   dim = Serial.read(); //get 0x00~0xff
    // }
    if (dim < 1)
    {
        RELAY_OFF;
        delay(10);
        //Turn TRIAC completely OFF if dim is 0
        TRIAC_CTRL_OFF; // digitalWrite(AC_pin, LOW);
    }
    else if (dim > 254)
    { //Turn TRIAC completely ON if dim is 255
        RELAY_ON;
        delay(10);
        TRIAC_CTRL_ON; //digitalWrite(AC_pin, HIGH);
    }
    else if (dim > 0 && dim < 255)
    {
        RELAY_ON;
        //Dimming part, if dim is not 0 and not 255
        delayMicroseconds(34 * (255 - dim));
        TRIAC_CTRL_ON;
        delayMicroseconds(500); //delayMicroseconds(500);
        TRIAC_CTRL_OFF;
    }
    else
        RELAY_OFF;
}

int command_explain(String str)
{
    //string spilt
    String txt = str;
    if (txt.startsWith(node_id) || txt.startsWith(debug_id))
    {
        //int node_id = (txt.substring(2, 5)).toInt();
        long node_act = txt.substring(id_number_length + 5, id_number_length + 8).toInt();
        int node_param = txt.substring(id_number_length + 14, id_number_length + 20).toInt();

        Serial.println("ACT:  " + String(node_act));
        Serial.println("PARAM: " + String(node_param));

        switch (node_act)
        {
        case 0:
            Serial.println("ALL CLOSE");
            dim = 0;
            delay_flag = 0;
            break;

        case 1:
            Serial.println("ALL OPEN");
            dim = 255;
            delay_flag = 0;
            break;

        case 2:
            Serial.println("PWM");
            dim = node_param % 1000;
            delay_flag = 0;
            break;

        case 3:
            Serial.println("PWM DELAY ON");
            dim = node_param % 1000;
            delay_second = node_param / 1000;
            command_time = millis();
            delay_flag = 1;
            break;

        case 4:
            Serial.println("PWM DELAY OFF");
            dim = node_param % 1000;
            delay_second = node_param / 1000;
            command_time = millis();
            delay_flag = -1;
            break;

        case 114:
            Serial.println("CHECK DIM");
            break;

        default:
            Serial.println("UNKNOWN ACT!");
            return 0;
        }
        return 1;
    }

    return 0;
}

//Get 328p UID
String get_UID()
{
    String UID = "ID";
    Serial.println("\nUID Bytes:");
    for (int i = 14; i < 14 + 10; i++)
    {
        char hex[5];
        sprintf(hex, "%X", boot_signature_byte_get(i));
        UID += String(hex);

        Serial.print(boot_signature_byte_get(i), HEX);
    }
    Serial.println("");
    return UID;
}