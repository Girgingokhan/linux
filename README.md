# linux
Embedded Linux
BeagleBone Black + Embedded Debian + Siemens PAC3200 + Modbus TCP
TI Sitara AM3358 Arm cortex A8


Compile on BeagleBone Debian

Modbus Libraray
sudo apt update

sudo apt install libmodbus-dev

Compile:

gcc pac3200_modbus_client.c \
-o pac3200_reader \
-lmodbus

Çalıştırma:

sudo ./pac3200_reader

Connected to Siemens PAC3200

IP Address : 192.168.1.20
Port       : 502




====================================
Voltage L1 : 230.45 V
Current L1 : 18.72 A
Active Power : 4200.35 W
====================================



