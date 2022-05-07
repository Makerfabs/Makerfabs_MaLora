/*

这是基于Maduino Lora Radio的测试程序

Author:         Vincent
Create date:    2022/3/23
Version:        1.1

                V1.1: Added test program to send instructions and display back to serial port.

*/

#include <RadioLib.h>

#define DIO0 2
#define DIO1 6
#define DIO2 7
#define DIO5 8

#define LORA_RST 4
#define LORA_CS 10

#define SPI_MOSI 11
#define SPI_MISO 12
#define SPI_SCK 13

#define CYCLE_TIME 5000 // ms

String node_id = String("IDXDEBUG");
int id_number_length = 6;
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

#define FREQUENCY 434.0 // 433.0 868.0 915.0
#define BANDWIDTH 125.0
#define SPREADING_FACTOR 9
#define CODING_RATE 7
#define OUTPUT_POWER 10
#define PREAMBLE_LEN 8
#define GAIN 0

String command_text[4] = {"IDXDEBUGACT001PARAM000000#",
                          "IDXDEBUGACT000PARAM000000#",
                          "IDXDEBUGACT001PARAM000000#",
                          "IDXDEBUGACT000PARAM000000#"};
int count = 0;

SX1276 radio = new Module(LORA_CS, DIO0, LORA_RST, DIO1);

void setup()
{
    Serial.begin(115200);

    pin_init();

    // initialize SX1278 with default settings
    Serial.print(F("[SX1278] Initializing ... "));

    int state = radio.begin(FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, SX127X_SYNC_WORD, OUTPUT_POWER, PREAMBLE_LEN, GAIN);

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

int index = 0;

void loop()
{
    // lora_fake_command();
    // lora_fake_command2();
    // lora_node_general();
    // lora_fake_reply();

    lora_send_and_receive();
}

void lora_fake_command()
{

    String msg = "IDXDEBUGACT001PARAM000000";
    Serial.println(msg);
    radio.transmit(msg);
    delay(CYCLE_TIME);

    msg = "IDXDEBUGACT000PARAM000000";
    Serial.println(msg);
    radio.transmit(msg);
    delay(CYCLE_TIME);
}

void lora_fake_command2()
{

    String msg = "IDXDEBUGACT002PARAM008888";
    Serial.println(msg);
    radio.transmit(msg);
    delay(CYCLE_TIME);

    msg = "IDXDEBUGACT002PARAM003333";
    Serial.println(msg);
    radio.transmit(msg);
    delay(CYCLE_TIME);

    msg = "IDXDEBUGACT002PARAM000000";
    Serial.println(msg);
    radio.transmit(msg);
    delay(CYCLE_TIME);
}

void lora_fake_reply()
{
    radio.transmit((String) "ID010123 REPLY : SOIL INEDX:" + index + " H:48.85 T:30.50 ADC:896 BAT:1016");
    delay(CYCLE_TIME);
}

void lora_node_general()
{
    // Serial.print(F("[SX1278] Waiting for incoming transmission ... "));
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
            String back_str = node_id + " REPLY : TEST_NODE " + String("STATUS DATA");
            radio.transmit(back_str);
        }
    }
    else if (state == ERR_RX_TIMEOUT)
    {
        // timeout occurred while waiting for a packet
        // Serial.println(F("timeout!"));
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

void lora_send_and_receive()
{
    Serial.print(F("[SX1278] Transmitting packet ... "));

    Serial.print("          " + command_text[count % 4]);

    int state = radio.transmit(command_text[count++ % 4]);

    if (state == ERR_NONE)
    {
        // the packet was successfully transmitted
        Serial.println(F(" success!"));

        // print measured data rate
        Serial.print(F("[SX1278] Datarate:\t"));
        Serial.print(radio.getDataRate());
        Serial.println(F(" bps"));
    }
    else if (state == ERR_PACKET_TOO_LONG)
    {
        // the supplied packet was longer than 256 bytes
        Serial.println(F("too long!"));
    }
    else if (state == ERR_TX_TIMEOUT)
    {
        // timeout occurred while transmitting packet
        Serial.println(F("timeout!"));
    }
    else
    {
        // some other error occurred
        Serial.print(F("failed, code "));
        Serial.println(state);
    }

    String str;
    state = radio.receive(str);

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
    }
    else if (state == ERR_RX_TIMEOUT)
    {
        // timeout occurred while waiting for a packet
        // Serial.println(F("timeout!"));
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

    // wait for a second before transmitting again
    delay(3000);
}

void pin_init()
{
}

// ID001ACT002PARAM001010
int command_explain(String str)
{
    // string spilt
    String txt = str;
    if (txt.startsWith(node_id) || txt.startsWith(debug_id))
    {
        // int node_id = (txt.substring(2, 5)).toInt();
        long node_act = txt.substring(id_number_length + 5, id_number_length + 8).toInt();
        int node_param = txt.substring(id_number_length + 14, id_number_length + 20).toInt();

        Serial.println("ACT:  " + String(node_act));
        Serial.println("PARAM: " + String(node_param));

        switch (node_act)
        {
        case 0:
            Serial.println("ALL CLOSE");
            break;

        case 1:
            Serial.println("ALL OPEN");
            break;

        case 2:
            Serial.println("Control");
            break;

        case 114:
            Serial.println("CHECK STATUS");
            break;

        default:
            Serial.println("UNKNOWN ACT!");
            return 0;
        }
        return 1;
    }

    return 0;
}
