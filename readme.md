# Makerfabs MaLora



```c++
/*
Version:		V1.1
Author:			Vincent
Create Date:	2022/9/28
Note:
				V1.0 Create Project
				V1.1 Add DIMMER and SOIL, change project name to MaLora
			
*/
```

[toc]

# Makerfabs

[Makerfabs home page](https://www.makerfabs.com/)

[Makerfabs Wiki](https://makerfabs.com/wiki/index.php?title=Main_Page)



# What is MaLora

**MaLora** is Makerfabs Loranode.

Makerfabs Lora Node is a series of products based on Arduino Pro Mini (328P) + Lora(SX127X). 

Like Relay, AC Dimmer, MOSFET, etc.

For easy to use, UID and generic instructions are provided. Some of them are our previous products, but have not added UID yet. 

Please refer to status of Lora Node List below.

## Feature

- ATMEL Atmega328P
- Semtech SX127X

- Unique ID, can be used directly without secondary programming.

## Lora Node List

| NUM  | Type                                                         | Short    | Status      |
| ---- | ------------------------------------------------------------ | -------- | ----------- |
| -1   | NULL                                                         | NULL     |             |
| 00   | [Lora AC Dimmer](00DIMMER) | DIMMER   | **Already** |
| 01   | [Lora Soil Moisture Sensor](01SOIL/) | SOIL     | **Already** |
| 02   | [Lora Relay](02RELAY/)                                       | RELAY    | **Already**   |
| 03   | [Lora Relay 4 Channel](03RELAY4/)                            | RELAY4   | **Already**   |
| 04   | Lora TDS                                                     | TDS      | Not Ready   |
| 05   | Lora soil multi element sensor                               | SOIL-PRO | Not Ready   |
| 06   | [Lora- 4 Channel MOSFET Driver](06MOS4/)                     | MOS4     | Not Ready   |
|      |                                                              |          |             |

## Transmission Device

You can use a Lora Node as the sending device.

You can also use our other products, or other Lora devices.

- [Maduino Lora Radio](https://www.makerfabs.com/wiki/index.php?title=Maduino_Lora_Radio_(433M/868M))



# Generic Lora Command

## ATTENTION

**Makerfabs products consist only of hardware, not software. The Lora Node instructions are not our product, but are intended as sample code for beginners. It is used to test hardware functions.**

**It does not guarantee the security of the code. It can only be used as a reference and cannot provide subsequent software support. Some products involve high voltage and other dangerous functions, please make sure you have enough electrical knowledge.**

**Makerfabs assumes no responsibility for any software errors and any associated problems.**

## Command

The Lora command is a generic isometric instruction that is transmitted as a string as follows:

```c
ID000123ACT114PARAM012345
```

Divided into three parts:

- Device ID: ID000123
- Action ID: ACT001
- Parameter: PARAM000000

The preceding zeros must not be omitted.

### Device ID(UID)

UID is the factory serial number of manual burning, manual modification when burning, has nothing to do with the equipment. The action continues only when a command with the same device ID is received.

Generally, the value is ID+6 digits.

The first two digits indicate the device type number, and the last four digits indicate the serial number.

```
ID030001

03					0001
Type:Relay			UID number:0001
```

Can be customized by the user, also provided to get the 328P chip UID routines.

### Action ID

All Lora Node must implement the following two basic commands. (Including read-only terminals)

| ACT  | PARAM             | Description                                                  |
| ---- | ----------------- | ------------------------------------------------------------ |
| 114  | 000000(not parse) | Querying node status                                         |
| 000  | 000000(not parse) | All functions are disabled. The read-only terminal does not perform any action, but it is necessary to reply. |
|      |                   |                                                              |

example：

```c
ACT = 0 Close
  ID012345ACT000PARAM000000
    
ACT = 114 Query Node Status
  ID012345ACT114PARAM000000
```



## General Reply

Lora node does not actively upload data to ensure that the channel is smooth. Instead,  Lora Gateway actively sends commands to control the channel.

To facilitate parsing, use string reply.

The reply format is as follows:

```c
ID012345[space]REPLY[space]:[space][replay data]
```

Spaces cannot be omitted.



# How To Use

## Complier Option

**If you have any questions，such as how to install the development board, how to download the code, how to install the library. Please refer to :[Makerfabs_FAQ](https://github.com/Makerfabs/Makerfabs_FAQ)**

- Please disconnect from the electrical equipment before burning!
- Do not burn with alternating current!
- Install library: RadioLib and other libs in Arduino IDE
- Upload codes, select "Arduino Pro or Pro Mini" and "3.3V 8MHz"

All Lora Nodes use this Lora library: [RadioLib](https://github.com/jgromes/RadioLib)



## Uploader

There is no USB socket or integrated serial port chip on some modules. But there are serial ports: VCC, GND, RX, TX, DTR. 

There are two ways to connect a serial port.



### MakerFabs CP2104 USB2UART

Product link : [CP2104 USB2UART](https://www.makerfabs.com/cp2104-usb-to-serial-converter.html)

![cp2104](md_pic/cp2104.png)

This CP2104 USB to Serial Converter is super tiny, a highly-integrated USB-to-UART Bridge Controller providing a simple solution for updating and programming.

The serial port module has the same pin position as the makerfabs module with no USB port. So just plug it into the hole.

![cp2104](md_pic/cp2104-2.png)

The usage of usb cable is the same after that.

### CP2102 MODULES USB TO TTL

There are many common USB serial port modules on the market, which are basically connected in a common way.

| Makerfabs | USB To TTL |
| --------- | ---------- |
| VCC       | 3.3V       |
| GND       | GND        |
| RX        | TXD        |
| TX        | RXD        |



