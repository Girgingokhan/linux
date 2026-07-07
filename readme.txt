BeagleBone Black + Embedded Debian + Siemens PAC3200 + Modbus TCP
TI Sitara AM3358 Arm cortex A8


+-------------------+        Ethernet TCP/IP        +----------------------+
|                   |  -------------------------->  |                      |
| Arm cortex A8     |        Port 502               | Siemens PAC3200      |
| Debian Linux      |                               | Energy Analyzer      |
| C/C++ Application |  <--------------------------  | Modbus TCP Server    |
|                   |       Modbus Response         |                      |
+-------------------+                               +----------------------+


Devices	                 IP
BeagleBone Black	  192.168.1.10
PAC3200	            192.168.1.20
Subnet	            255.255.255.0
Modbus TCP Port 	  502


BeagleBone Black Modbus TCP Client (Master)
Siemens PAC3200'i Modbus TCP Server (Slave)

We read from the PAC3200
L1 Current (A)
L1 Voltage (V)
Active Power (W)



