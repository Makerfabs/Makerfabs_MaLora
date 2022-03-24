 Makerfabs Lora Node：Lora- 4 Channel MOSFET Driver
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

4-Channel Lora MOSFET based on the Arduino, users can program it with Arduino IDE, which is very easy especially suit for the none-programmers. There are also guide for users to learn how to create the first IoT project with this board, with which the starters can learn the hardware and programming skill quickly. 

Product Link ：[]()

Wiki Link : []()


## Feature

* ATMEL Atmega328P: High Performance, Low Power Atmel®AVR® 8-Bit Microcontroller
* Speed Grade:20Mhz
* Flash:32Kbytes
* RAM: 2KBytes
* EEPROM: 1Kbytes
* Rated current of relay contact: 10A ???
* Coil type of relay: Non Latching
* Coil voltage of relay: 5V
* Switching voltage of relay: (277VAC , 28VDC) Max ???
* DC12V input



### Front

![front](md_pic/front.jpg)



![back](md_pic/back.jpg)



 ## Pin OUT

Control relay pins: 

| Atmega328P | MOS  |
| ---------- | ---- |
| D5         | K1   |
| A0         | K2   |
| A1         | K3   |
| D3         | K4   |



# Command

## Lora Node Massage

```c
Type ID: 06
Short name: MOS4
```

## Action

| ACT  | PARAM             | Description          |
| ---- | ----------------- | -------------------- |
| 114  | 000000(not parse) | Querying node status |
| 000  | 000000(not parse) | Close all MOS        |
| 001  | 000000(not parse) | Open all MOS         |
| 002  | 00XXXX            | Set 4 channel MOS    |

Command example:

```c
ACT = 0 Close
  ID001ACT000PARAM000000

ACT = 1 All Open
  ID001ACT001PARAM000000

ACT = 2 Control 
  PARAM = 0000-8888 MOS Status	0,1/8,...,7/8,1
  ID001ACT002PARAM001010
```



## Reply

The reply format is as follows:

```c
ID06XXXX[space]REPLY[space]:[space]MOS4[space][Relay Status]
    
ID030001 REPLY : MOS4 0801
```

Spaces cannot be omitted.