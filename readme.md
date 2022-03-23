# Makerfabs Lora Node



```c++
/*
Version:		V1.0
Author:			Vincent
Create Date:	2022/9/28
Note:
	
*/
```
![](md_pic/main.jpg)


[toc]

# Makerfabs

[Makerfabs home page](https://www.makerfabs.com/)

[Makerfabs Wiki](https://makerfabs.com/wiki/index.php?title=Main_Page)



# What is Lora Node

Makerfabs Lora Node is a series of products based on Arduino Pro Mini (328P) + Lora(SX127X). 

Like Relay, AC Dimmer, Mosfet, etc.

For easy to use, UID and generic instructions are provided.

Some of them are our previous products, but have not added UID yet. Please refer to status of Lora Node List below.

## Feature

- ATMEL Atmega328P
- Semtech SX127X

- Unique ID, can be used directly without secondary programming.

## Lora Node List

| NUM  | Type                                                         | Short    | Status      |
| ---- | ------------------------------------------------------------ | -------- | ----------- |
| -1   | NULL                                                         | NULL     |             |
| 00   | Lora AC Dimmer                                               | DIMMER   | Not Ready   |
| 01   | [Lora Soil Moisture Sensor](https://github.com/Makerfabs/Lora-Soil-Moisture-Sensor/tree/master/V3) | SOIL     | **Already** |
| 02   | Lora Relay                                                   | RELAY    | Not Ready   |
| 03   | Lora Relay 4 Channel                                         | RELAY4   | Not Ready   |
| 04   | Lora TDS                                                     | TDS      | Not Ready   |
| 05   | Lora soil multi element sensor                               | SOIL-PRO | Not Ready   |
| 06   | Lora- 4 Channel MOSFET Driver                                | MOS4     | Not Ready   |
|      |                                                              |          |             |



# Generic Lora Command

## UID

The first two digits indicate the device type number, and the last four digits indicate the serial number.

```c
03					0001
Type:Relay			Serial number:0001
```



## Command

The Lora command is a generic isometric instruction that is transmitted as a string as follows:

```c
ID000123ACT114PARAM012345
```

It can be splitted into three parts:

- ID000123：Device ID

- ACT114：Action number (114 is a general query action, which must be implemented by all terminals)

- PARAM012345：Action parameters

The total length is: 8+6+11 = 25 

All zeros cannot be omitted.

## General Command

All LoraNode must implement the following two basic commands. (Including read-only terminals)

| ACT  | PARAM             | Description                                                  |
| ---- | ----------------- | ------------------------------------------------------------ |
| 114  | 000000(not parse) | Querying node status                                         |
| 000  | 000000(not parse) | All functions are disabled. The read-only terminal does not perform any action, but it is necessary to reply. |
|      |                   |                                                              |

 

## General Reply

Lora node does not actively upload data to ensure that the channel is smooth. Instead, LoraGetway actively sends commands to control the channel.

To facilitate parsing, use string reply.



The reply format is as follows:

```c
ID012345[space]REPLY[space]:[space][replay data]
```

Spaces cannot be omitted.