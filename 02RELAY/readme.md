 Makerfabs Lora Node：Lora Relay
 ==



```
/*
Version:		V1.0
Author:			Vincent
Create Date:	2022/3/24
Note:
	
*/
```

![main](md_pic/main.jpg)

[toc]

# Makerfabs

[Makerfabs home page](https://www.makerfabs.com/)

[Makerfabs Wiki](https://makerfabs.com/wiki/index.php?title=Main_Page)



# Introduce

4-Channel Lora Relay-10A based on the Arduino, users can program it with Arduino IDE, which is very easy especially suit for the none-programmers. There are also guide for users to learn how to create the first IoT project with this board, with which the starters can learn the hardware and programming skill quickly. 

Product Link ：[Lora Relay](https://www.makerfabs.com/lora-relay-30a.html)

Wiki Link : [Lora Relay](https://www.makerfabs.com/wiki/index.php?title=Lora_Relay)


## Feature

* ATMEL Atmega328P: High Performance, Low Power Atmel®AVR® 8-Bit Microcontroller
* Speed Grade:20Mhz
* Flash:32Kbytes
* RAM: 2KBytes
* EEPROM: 1Kbytes
* Relay type: General Purpose 
* Rated current of relay contact: 10A
* Coil type of relay: Non Latching
* Coil voltage of relay: 5V
* Switching voltage of relay: (277VAC , 28VDC) Max
* DC12V or 5V input



### Front

![front](md_pic/front.jpg)



![back](md_pic/back.jpg)



 ## Pin OUT

Control relay pins: 

|Atmega328P	|Relay |
|---|---|
|D4	|K1 |
|	| |
|            |       |
|            |       |



# Command

The firmware of Lora Relay and Lora Relay 4 Channel is basically the same except that the number of relays is different.

## Lora Node Massage

```c
Type ID: 02
Short name: RELAY
```

## Action

| ACT  | PARAM             | Description          |
| ---- | ----------------- | -------------------- |
| 114  | 000000(not parse) | Querying node status |
| 000  | 000000(not parse) | Close all relay      |
| 001  | 000000(not parse) | Open all relay       |
| 002  | 00X000            | Set 1 channel relay  |

Command example:

```c
ACT = 0 Close
  ID001ACT000PARAM000000

ACT = 1 All Open
  ID001ACT001PARAM000000

ACT = 2 Control 
  PARAM = 0000-1000 Relay Status
  ID001ACT002PARAM00100
```



## Reply

The reply format is as follows:

```c
ID02XXXX[space]REPLY[space]:[space]RELAY[space][Relay Status]
    
ID020001 REPLY : RELAY 1000
```

Spaces cannot be omitted.

