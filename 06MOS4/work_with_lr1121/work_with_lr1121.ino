#include <RadioLib.h>

#define DIO0 2
#define DIO1 A1
#define DIO2 7
#define DIO5 8

#define LORA_RST 4
#define LORA_CS 10

#define SPI_MOSI 11
#define SPI_MISO 12
#define SPI_SCK 13

#define MOS_RATIO 100

#define DEBUG_MODE true

#define MAX_RETRY_COUNT 3
#define RETRY_DELAY 1000

int mos_status[4] = {0, 0, 0, 0};//0-->OFF  8-->ON
int mos_pin[4] = {5, 6, 9, 3};
int mos_value[9] = {0, 32, 64, 96, 128, 160, 192, 224, 255};

String type_name = "MOS4";

String node_id = String("ID") + "060000";
int id_number_length = 6;
String debug_id = "IDXDEBUG";

#define FREQUENCY 868.0         // 载波频率，单位MHz
#define BANDWIDTH 250.0         // 带宽，单位kHz
#define SPREADING_FACTOR 9     // 扩频因子
#define CODING_RATE 7           // 编码率
#define OUTPUT_POWER 20         // 输出功率，单位dBm
#define PREAMBLE_LEN 8          // 前导码长度
#define GAIN 0                  // 增益设置

SX1276 radio = new Module(LORA_CS, DIO0, LORA_RST, DIO1);
//SX1278 radio = new Module(LORA_CS, DIO0, LORA_RST, DIO1, SPI, SPISettings()); // SX1278备选配置（当前未使用）

unsigned long lastCommandSeq = 0;

void printLoRaConfig() {
    Serial.println("\n===== LoRa Config ======");
    Serial.print("Frequency: "); Serial.print(FREQUENCY); Serial.println(" MHz");
    Serial.print("Bandwidth: "); Serial.print(BANDWIDTH); Serial.println(" kHz");
    Serial.print("Spreading Factor: "); Serial.println(SPREADING_FACTOR);
    Serial.print("Coding Rate: "); Serial.println(CODING_RATE);
    Serial.print("Output Power: "); Serial.print(OUTPUT_POWER); Serial.println(" dBm");
    Serial.print("Preamble Length: "); Serial.println(PREAMBLE_LEN);
    Serial.println("\n============================\n");
}

void setup()
{
    Serial.begin(115200);
    
    Serial.println("ID: " + node_id);

    mos_init();
    mos_control(0); //OFF ALL MOS

    Serial.print(F("[SX1278] init... "));

    int state = radio.begin(FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, SX127X_SYNC_WORD, OUTPUT_POWER, PREAMBLE_LEN, GAIN);

    if (state == ERR_NONE)
    {
        Serial.println(F("success!"));
        
        //state = radio.invertIQ(true);
        // if (state == ERR_NONE)
        // {
        //     Serial.println(F("IQ Inverted!"));
        // }
        // else
        // {
        //     Serial.print(F("IQ Invert fail, error code: "));
        //     Serial.println(state);
        // }
        
        printLoRaConfig();
    }
    else
    {
        Serial.print(F("Init fail, error code: "));
        Serial.println(state);
        while (true);
    }
}

void loop()
{
    String str;
    int state = radio.receive(str);

    if (state == ERR_NONE)
    {
        Serial.println(F("Receive success!"));

        Serial.print(F("[SX1278] Data:\t\t\t"));
        Serial.println(str);

        Serial.print(F("[SX1278] RSSI:\t\t\t"));
        Serial.print(radio.getRSSI());
        Serial.println(F(" dBm"));

        Serial.print(F("[SX1278] SNR:\t\t\t"));
        Serial.print(radio.getSNR());
        Serial.println(F(" dB"));

        Serial.print(F("[SX1278] Frequency Error:\t"));
        Serial.print(radio.getFrequencyError());
        Serial.println(F(" Hz"));

        if (command_explain(str))
        {
            char reply_cstr[20];

            sprintf(reply_cstr, "%04d", get_mos_status());

            String back_str = type_name + ":" + reply_cstr;
            
            int tx_state = radio.transmit(back_str);
            
            if (tx_state == ERR_NONE) {
                Serial.println("Send: " + back_str);
            } else {
                Serial.print("Send fail, error code: ");
                Serial.println(tx_state);
            }
        }
    }
    else if (state == ERR_RX_TIMEOUT)
    {
        //Serial.println(F("ERR_RX_TIMEOUT"));
    }
    else if (state == ERR_CRC_MISMATCH)
    {
        Serial.println(F("ERR_CRC_MISMATCH!"));
    }
    else
    {
        Serial.print(F("ERR: "));
        Serial.println(state);
    }
}

int command_explain(String str)
{
    String txt = str;
    // 检查命令是否以有效的节点ID开头
    if (txt.startsWith(node_id) || txt.startsWith(debug_id))
    {
        long mos_act = 0;
        //ID060000MOS8888
        if (txt.length() >= id_number_length + 9)
        {
            mos_act = txt.substring(id_number_length + 5, id_number_length + 9).toInt();
        }
        else
        {
            Serial.println("ERR: Data incompleteness");
            return 0;
        }

        Serial.println("MOS:  " + String(mos_act));

        if (mos_act >= 0 && mos_act <= 8888)
        {
            mos_control(mos_act);
            return 1;
        }
        else
        {
            Serial.println("Parameter out of range (0-8888)");
            return 0;
        }
    }
    return 0;
}

void mos_control(int value)
{
    Serial.print("Set MOS Status: ");
    Serial.println(value);
    
    for (int i = 0; i < 4; i++)
    {
        // the lowest bit corresponds to the first MOS
        int status = value % 10;
        
        if (status > 8)
        {
            status = 8;
        }
        
        mos_status[i] = status;
        
        mos_set(mos_pin[i], mos_value[mos_status[i]]);
        // NEXT MOS SET
        value /= 10;
        
        if (DEBUG_MODE) {
            Serial.print("MOS ");
            Serial.print(i);
            Serial.print(" Status: ");
            Serial.print(mos_status[i]);
            Serial.print(", PWM Value: ");
            Serial.println(mos_value[mos_status[i]]);
        }
    }
}

int get_mos_status()
{
    int status = 0;

    for (int i = 3; i >= 0; i--)
    {
        status *= 10;
        status += mos_status[i];
    }
    return status;
}

void mos_init()
{
    for (int i = 0; i < 4; i++) 
    {
        pinMode(mos_pin[i], OUTPUT);
        digitalWrite(mos_pin[i], HIGH);
    }
    
    Serial.println("MOS init complete");
}

void mos_set(int8_t pin, uint16_t value)
{
    if (value > 255)
        value = 255;
    if (value < 0)
        value = 0;

    value = value * MOS_RATIO / 100;
    
    analogWrite(pin, 255 - value);
}