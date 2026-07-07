/*
===========================================================
 Siemens PAC3200 Modbus TCP Client Example

 Platform:
 BeagleBone Black
 Embedded Debian Linux

 Communication:
 Modbus TCP over Ethernet

 Library:
 libmodbus

 Description:
 This application reads electrical measurement values
 from Siemens PAC3200 energy analyzer.

 BeagleBone Black:
     Modbus TCP Client (Master)

 Siemens PAC3200:
     Modbus TCP Server (Slave)

===========================================================
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <modbus/modbus.h>



/*
===========================================================
 Configuration
===========================================================
*/


#define PAC3200_IP      "192.168.1.20"

#define MODBUS_PORT     502



/*
===========================================================
 PAC3200 Modbus Register Addresses

 NOTE:
 Register addresses may change according to PAC3200
 firmware and configuration.

 These are example addresses.
===========================================================
*/


#define REG_VOLTAGE_L1      19000

#define REG_CURRENT_L1      19014

#define REG_ACTIVE_POWER    19050





/*
===========================================================
 Convert two Modbus registers into IEEE754 Float

 PAC3200 stores measurement values as 32-bit float.

 Example:

 Register High Word
 Register Low Word

 Combine:

 32-bit Integer

 Convert:

 Float

===========================================================
*/


float ModbusRegistersToFloat(uint16_t *data)
{
    uint32_t raw_value;
    float result;

    raw_value = ((uint32_t)data[0] << 16) | data[1];
    memcpy(&result, &raw_value, sizeof(float));
    return result;
}







/*
===========================================================
 Read Float32 value from PAC3200

 Input:

 ctx:
 Modbus connection

 address:
 First register address


 Output:

 Float measurement value

===========================================================
*/


float PAC3200_ReadFloat(modbus_t *ctx, int address)
{
    uint16_t registers[2];
    int result;
    /*
        Read two Modbus registers
        Function Code: 03 - Read Holding Registers
    */
    result = modbus_read_registers( ctx, address, 2, registers );

        if(result < 0)
        {
            // printf("Modbus Read Error: %s\n", modbus_strerror(errno));
            return -1.0f;
        }
    return ModbusRegistersToFloat(registers);

}



int main(){

    modbus_t *ctx;

    float voltage_L1;
    float current_L1;
    float active_power;
    /*
    =======================================================
    Create Modbus TCP Context
    IP: Siemens PAC3200 IP address
    Port:Standard Modbus TCP port 502
    =======================================================
    */

    ctx = modbus_new_tcp(PAC3200_IP, MODBUS_PORT );
    if(ctx == NULL)
    {
        //printf( "Unable to create Modbus TCP context\n" );
        return -1;
    }


    /*
    =======================================================
    Connect to PAC3200
    =======================================================
    */

    if( modbus_connect(ctx) == -1 )
    {
      //  printf( "Connection failed: %s\n", modbus_strerror(errno) );
        modbus_free(ctx);
        return -1;
    }
    printf( "\nConnected to Siemens PAC3200\n"  );

    // printf( "IP Address : %s\n", PAC3200_IP );


 //   printf( "Port       : %d\n\n", MODBUS_PORT);


    /*
    =======================================================
    Main Measurement Loop
    Read values every second
    =======================================================
    */
    while(1)
    {
        /* Read L1 phase voltage */

        voltage_L1 = PAC3200_ReadFloat( ctx, REG_VOLTAGE_L1 );
        
        /* Read L1 current */

        current_L1 = PAC3200_ReadFloat( ctx,REG_CURRENT_L1 );

        /* Read active power */

        active_power = PAC3200_ReadFloat( ctx,  REG_ACTIVE_POWER );

        printf("Voltage L1      : %.2f V\n", voltage_L1 );
        printf("Current L1      : %.2f A\n", current_L1 );
        printf("Active Power    : %.2f W\n", active_power);

        /* Sampling time 1000 ms */
        sleep(1);
    }

    /*
    =======================================================
    Close Modbus Connection
    =======================================================
    */
    modbus_close(ctx);
    modbus_free(ctx);
    return 0;
}
