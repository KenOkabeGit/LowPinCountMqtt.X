/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using MPLAB® Code Configurator

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  MPLAB® Code Configurator - v2.25.2
        Device            :  PIC16LF1509
        Driver Version    :  2.00
    The generated drivers are tested against the following:
        Compiler          :  XC8 v1.34
        MPLAB             :  MPLAB X v2.35 or v3.00
 */

/*
Copyright (c) 2013 - 2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 */

#include "mcc_generated_files/mcc.h"
#include "wifly_lib.h"
#include "util_lib.h"

#include <stdio.h>
#include <string.h>

void sendUart(void);
void EUSART_Str_Write(uint8_t *txData);
void wiflyInitState(void);
int mqtt_pub(void);

int mqtt_connect(void);
int mqtt_subscribe(void);
int mqtt_get_msg(void);


volatile char Heart_beat_flag = 0;
volatile char Transmit_flag = 0;

/*
                         Main application
 */
void main(void) {

    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

    RC0=1;
    RC1=0;
    RC2=0;
    RC3=0;

  Transmit_flag = 0;
  while(!Transmit_flag);  //wait 1sec.
  Transmit_flag = 0;
  while(!Transmit_flag);  //wait 1sec.


    // EUSART_Str_Write((uint8_t *)"\r\nStart!abcdefghijklmnopqrstu\r\n");
  led_init();

// wifly_ready();

  while (1) {
      // Add your application code
      if(Heart_beat_flag){
          Heart_beat_flag = 0;
          RC0 ^= 1;
      }


      wiflyInitState();



  }
}
typedef enum{
  StateCmdDDD,
  StateOpenCmd,
  StateMqttConnect,
  StateMqttSubscribe,
  StateMqttGegMsg,
  StateCloseCmd,
  StateIdle,
  StateStop,
} StateWiflyInit;

void wiflyInitState(void)
{
  static StateWiflyInit state = StateCmdDDD;
  static char locate = 0;

  switch(state){
    case StateCmdDDD:
      wifly_command_mode();
      state = StateOpenCmd;
      break;

    case StateOpenCmd:
      wifly_open("test.mosquitto.org",1883);
      state = StateMqttConnect;
    break;

    case StateMqttConnect:
      if(!mqtt_connect()){
        state = StateCloseCmd;
      }
      state = StateMqttSubscribe;
    break;

    case StateMqttSubscribe:
      if(!mqtt_subscribe()){
        state = StateCloseCmd;
        RC3 = 1;
        break;
      }
      state = StateMqttGegMsg;
    break;

    case StateMqttGegMsg:
      if(!mqtt_get_msg()){
        state = StateCloseCmd;
        break;
      }
      state = StateCloseCmd;
    break;

    case StateCloseCmd:
      led_show();
      wifly_close();
      state = StateIdle;
      Transmit_flag = 0;
    break;

    case StateIdle:
      if(RA2==0){
        state = StateStop;
        break;
      }
      if(Transmit_flag){
        state = StateOpenCmd;
        break;
      }  //wait 1sec.
    break;

    case StateStop:
    break;


    default:
      // RC3=1;
      while (1) {
        /* code */
      }
  }
}

/**
 End of File
 */
