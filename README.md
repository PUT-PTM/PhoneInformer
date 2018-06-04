# PhoneInformer

## 1.Overview
PhoneInformer is a project build on STM32. It receives of information, via Bluetooth, about incoming calls and SMS messages on smartphone, displays it on the screen and notifies with sound. It is a modern answer for requisition related with old pager.

## 2.Description
The project consists of an Android application and a STM32 application. Android app send information about calls and SMS to STM32 which displays received data on screen. If a number that contacts us is in our contacts list in smartphone, the name instead of number will be displayed on the screen. After receiving a message or a call, animations are displayed on the screen. You can use PhoneInformer, wherever your phone is not welcome.

## 3.Tools
* Microcontroller STM32F407VG DISCOVERY
* Bluetooth HC-06 module
* Nokia 3310 LCD screen 
* LM386M amplifier module with loudspeaker
* Smartphone with Android system
* System Workbench for STM32
* Android Studio

## 4.How to run
Wire connections:
* LCD screen:
 RST -> PC15	
 CE	 -> PC13		
 DC	 ->	PC14		
 DIN -> PC3			
 CLK -> PB10		
 VCC -> 3.3V		
 GND -> GND	
 * Bluetooth module				
 RX  -> PA9			
 TX  -> PA10		
 VCC -> 3.3V		
 GND -> GND	
 * Loudspeaker
 AIN -> PA4
 VCC -> 3.3V
 GND -> GND
 5V -> 5V
 
Releases of programs used to create apps:
* System Workbench for STM32 based on Eclipse IDE version: 4.6.3
* Android Studio 3.0.1

## 5.How to compile
Compilation takes place without any special instructions, you need just compilators build in Android Studio and System Workbench.

## 6.Future improvements
In the future, battery power supply can be can be added to the project. It can be also extended by adding control buttons and menu options. We will use Bluetooth 4.0 for a better range.

## 7.Attributions
In project were used: 
* NOKIA 3310/5110 LCD STM32F4/29 LIBRARY written by Tilen Majerle - [link](https://stm32f4-discovery.net/pcd8544-nokia-33105110-lcd-stm32f429-discovery-library/)

## 8.Credits
* **Jakub Kolasiński** - [kubakolas](https://github.com/kubakolas)
* **Kacper Kociubiński** - [kociuba1997](https://github.com/kociuba1997)

The project was conducted during the Microprocessor Lab course held by the Institute of Control and Information Engineering, Poznan University of Technology.
Supervisor: Tomasz Mańkowski
