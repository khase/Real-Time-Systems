/* ========================================
 *
 * Copyright Ralf S. Mayer, 2017
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#include <stdio.h>
#include <string.h>     // eg. strtok
#include <stdlib.h>     // eg. atoi, atol, atof
#include <unistd.h>
// own includes
#include "swsign.h"
#include "help.h"

#include "myCAN.h"

/**
 * Raw CAN receive message to be captured in
 * @see CAN_ReceiveMsg
 * declare external in CAN_TX_RX_func.c
 * @see canRXmsg_t
 */
canRXmsg_t canRXmsgRaw; 

static canRXmsg_t canRXmsg; 


enum Color{color_RED = 0, color_GREEN, color_BLUE};

/**
 * Control the User LED on board
 * @param sts 0: off, 1: on
 */
inline void usrLED( uint8 sts ) {
    Pin_USR_LED_Write( sts );
}

/**
 * \def USERLED_ONOFF
 * switch user LED on or off
 * @param x  1: on, 0: off
 */
#define USERLED_ONOFF(x) ( Pin_USR_LED_Write(x) )

// +++ globals +++
char buffer[128];       //!< buffer (global) for sprintf
uint8 _verboseFlag = 1; //!< verbosity flag

uint8 flagUART_RX = 0;          //!< flag (global) itr has occured
static char cRx = 0;            //!< charcter from UART, visible within main.c

/** 
 * UART RX interrupt service routine.
 *
 * Collect a characters from UART into a global input variable.
 * @see cRx
 */
CY_ISR( MyIsrUartRX ) {
    cRx = UART_GetChar();     // copy char
}

static uint8 flagCAN_RX = 0;          //!< flag CAN itr has occured
static uint8 flagProcessCAN_RX = 0;   //!< flag process CAN msg (init on)
static uint32 canRXcnt  = 0;          //!< CAN receive counter

/**
 * CAN message receive callback
 * @see CAN_ReceiveMsg() in CAN_TX_RX_func.c
 */
void CAN_ReceiveMsg_Callback() {
    // blink user led
    USERLED_ONOFF( 1u );
    CyDelay( 1 );
    USERLED_ONOFF( 0u );
    // copy message
    memcpy( &canRXmsg, &canRXmsgRaw, sizeof(canRXmsg_t) );
    canRXcnt++;
    flagCAN_RX = 1;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++   Candidates for new include and implementation file   ++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ------------------------------------------------------------------------------------

/**
 * Basisprojekt mit CAN, UART mit Interrupt, User-Buttoun und -LED 
 * für 059-5LP. Configuration of XTAL 24MHz, see
 * @see http://www.cypress.com/file/121846/download
 * @see http://www.cypress.com/file/179861/download
 * @see http://www.cypress.com/documentation/application-notes/an52701-psoc-3-and-psoc-5lp-getting-started-controller-area-network
 * @see http://www.cypress.com/file/96411/download
 * @see https://archive.eetasia.com/www.eetasia.com/STATIC/PDF/200910/EEOL_2009OCT09_INTD_AN_01.pdf?SOURCES=DOWNLOAD
 * 
 * Help und verschieden Funktionalität
 * @see help.c
 *
 * Derived fromriginal BasicCan_1, ver. 1.6
 * @author Ralf S. Mayer
 * @date 2018-03-19
 * @version 1.6
 *
 * @date 2018-06-02
 * @version 1.0
 */


char buffer[128];

void set(uint8 r, uint8 g, uint8 b){
    Red_WriteCompare(r);
    Green_WriteCompare(g);
    Blue_WriteCompare(b);
}

enum Color start(){
    
    enum Color color = color_BLUE;
    int r = 0;
    for(int i = 50; i < 200; i += 0)
    {
        r = rand() % 10;
        
        set(50, 0, 0);
        i += r;
        if (i >= 200){
            set(255, 0, 0);
            color = color_RED;
            break;
        }
        CyDelay(i);
        
        set(0, 50, 0);
        i += r;
        if (i >= 200){
            set(0, 255, 0);
            color = color_GREEN;
            break;
        }
        CyDelay(i);
        
        set(0, 0, 50);
        i += r;
        if (i >= 200){
            set(0, 0, 255);
            color = color_BLUE;
            break;
        }
        CyDelay(i);
    }
    return color;
}

enum Color led_blink(){
    
    enum Color color;
    color = start();
    
    return color;
}

void init(){
 CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */    
    Red_Start();
    Green_Start();
    Blue_Start();
    
    PRS_1_Start();
    
    sprintf(buffer, "\n\rWelcome!\n\r");
    UART_PutString( buffer );
}

int main()
{
    
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */   
    UART_Start();           // start uart
    isr_UART_RX_StartEx( MyIsrUartRX ); // enable itr uart
    
    // Start CAN
    CAN_Start();
    CAN_GlobalIntEnable();  // enable CAN global interrupts ?
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    sprintf( buffer, "\n\rWelcome! %s\n\r", _VERSTR_);
    UART_PutString( buffer );
    // help text
    prtHelp( UART_PutString );
    
    sprintf( buffer, "getCAN_CSR_CFG()= %lXh\n\r", getCAN_CSR_CFG() );
    UART_PutString( buffer );
    
    sprintf( buffer, "BRP: %d, TSEG1: %d, TSEG2: %d, SJW: %d, sm: %d\n\r",
        getCANbrp(), getCANtseg1(), getCANtseg2(), getCANsjw(),
        getCANsamplingMode() );
    UART_PutString( buffer );
    
    cRx = 'F';  // dirty!!! jump to menue directly
    
    char cInput = 0;
    
    // CAN tx Message samples
    CAN_TX_MSG canMsg_1;
    CAN_DATA_BYTES_MSG canDataBuf_1; 
    uint8 data_1[8] = { 0x01, 0x23, 0x45, 0x67, 0, 0, 0, 0 };

    // clear buffer
    memset( &canDataBuf_1,0, sizeof(canDataBuf_1) );
    // copy databytes
    memcpy( &canDataBuf_1, data_1, 4 );
    // set CAN params
    canMsg_1.id  = 0x1234;  // msg ID
    canMsg_1.rtr = 0;       // no request 
    canMsg_1.ide = 0xFF;    // extended msg 29bit id, 0: 11bit
    canMsg_1.dlc = 4;       // 4 bytes payload
    canMsg_1.irq = 0;       // not itr
    canMsg_1.msg = &canDataBuf_1;   // reference data buffer 

    CAN_TX_MSG canMsg_2;
    CAN_DATA_BYTES_MSG canDataBuf_2; 
    uint8 data_red[8] = { 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
    uint8 data_green[8] = { 0x00, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
    uint8 data_blue[8] = { 0x00, 0x0, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0 };
    canMsg_2.rtr = 0;       // no request RTR
    canMsg_2.ide = 0x00;    // extended msg
    canMsg_2.dlc = 8;       // 4 bytes payload
    canMsg_2.irq = 0;       // not itr
    canMsg_2.msg = &canDataBuf_2;   // reference data buffer  
    
    int isButtonPressed = 0;
    int seed = 0;
    
    
    /* INITIALIZE RANDOM SEED */
    for(;;){
        seed++;
        if (Pin_USR_SW1_Read() == 0 || button_Read())
        {
             break;
        }
    }
    
    init();
    sprintf( buffer, "\n\rSeed: %i\n\r",seed);
    UART_PutString( buffer );
    srand(seed);
    //flag if a new Round will be played;
    int newRound = 0;
    sprintf( buffer, "\n\rMoegen die Spiele beginnen!\n\r");
    UART_PutString( buffer );
    enum Color color=0;
    
    // Main loop
    for(;;)     // forever
    {               
        if(Pin_USR_SW1_Read() == 0 || button_Read()){
            isButtonPressed = 1;     
            newRound = 1;
            sprintf( buffer, "\n\rButton pressed\n\r");
            UART_PutString( buffer );
        }
        
        if(isButtonPressed){
            color = led_blink();
            isButtonPressed = 0;
        }
        
        if(newRound){
            newRound = 0;
            switch(color){
                case color_RED:
                    // send message that the current color is red
                    memcpy( &canDataBuf_2, data_red, 8 );
                    sprintf( buffer, "\n\rRED Color\n\r");
                    UART_PutString( buffer );
                    break;
                case color_BLUE: 
                    // send message that the current color is blue
                     memcpy( &canDataBuf_2, data_blue, 8 );
                    sprintf( buffer, "\n\rBLUE Color\n\r");
                    UART_PutString( buffer );
                    break;
                case color_GREEN: 
                    // send message that the current color is green
                     memcpy( &canDataBuf_2, data_green, 8 );
                    sprintf( buffer, "\n\rGREEN Color\n\r");
                    UART_PutString( buffer );
                    break;
                default:
                    sprintf( buffer, "\n\rUps, something gone wrong!\n\r");
                    UART_PutString( buffer );
            }
            canMsg_2.msg = &canDataBuf_2;
            Pin_USR_LED_Write(1u);
            CAN_SendMsg( &canMsg_2 );
            Pin_USR_LED_Write(0u);
        }
        
        if ( cRx != 0 ) {           // char received
            isr_UART_RX_Disable();  // disable Itr
            cInput = cRx;           // copy char
            cRx = 0;                // reset
            isr_UART_RX_Enable();   // enable Itr
        }
        
        if ( cInput != 0 ) {        // has input
            switch ( cInput ) {     // handle input
                case 'S':
                case 's':
                    // wait for the buttonpress
                    isButtonPressed = 1;     
                    newRound = 1;
                    sprintf( buffer, "\n\rInvoke Button pressed\n\r");
                    UART_PutString( buffer );
            }
            cInput = 0;
        }
    } // end for ever
} // end main

/* [] END OF FILE */
