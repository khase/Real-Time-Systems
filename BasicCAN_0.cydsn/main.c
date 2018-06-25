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
    
    CAN_TX_MSG canMsg_1;
    canMsg_1.id  = 0x01110;
    canMsg_1.rtr = canRXmsg.rtr; 
    canMsg_1.ide = canRXmsg.ide;
    canMsg_1.dlc = canRXmsg.msgLen;
    canMsg_1.irq = 0;
    canMsg_1.msg = (CAN_DATA_BYTES_MSG*)canRXmsg.msgData;
    CAN_SendMsg( &canMsg_1 );
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
    for(int i = 50; i < 200; i += 5)
    {
        r = rand() % 5;
        
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
    //sprintf( buffer, "Color: %i\n", color);
    //UART_PutString( buffer );
    return color;
}


void init(){
 CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */    
    Red_Start();
    Green_Start();
    Blue_Start();
    
    PRS_1_Start();
    
    sprintf( buffer, "\n\rWelcome!\n\r");
    UART_PutString( buffer );
}

enum Color led_blink(){
    
    enum Color color;
    Pin_USR_LED_Write(1u);
    color = start();
    Pin_USR_LED_Write(0u);
    
    return color;
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
    
    cRx = 'S';  // dirty!!! jump to menue directly
    
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
//    uint8 data_2[8] = { 'D', 'E', 'A', 'D', 'B', 'E', 'E', 'F' };
    uint8 data_2[8] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };
    memcpy( &canDataBuf_2, data_2, 8 );
//    canMsg_2.id  = 0x5678;  // standard msg id 11 bits (0x000 to 0x7EF)
    canMsg_2.id  = 0x1;  // extended msg id 29 bits (0x00000000 to 0x1FBFFFFF)
//    canMsg_2.rtr = 1;         //  request RTR
    canMsg_2.rtr = 0;       // no request RTR
//    canMsg_2.ide = 0x00;    // standard msg
    canMsg_2.ide = 0xFF;    // extended msg
    canMsg_2.dlc = 8;       // 4 bytes payload
    canMsg_2.irq = 0;       // not itr
//    canMsg_2.msg = &canDataBuf_1;   // reference data buffer 
    canMsg_2.msg = (CAN_DATA_BYTES_MSG*) data_2;   // reference data buffer 
    
    int isButtonPressed = 0;
    int seed = 0;
    
    
    /* INITIALIZE RANDOM SEED */
    for(;;){
        seed++;
        if (Pin_USR_SW1_Read() == 0)
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
        
        //wait for an message => all player are ready and ne new round can start
        //bla bla
        // fu ba r
        // newRound = 1;
        
        // wait for the buttonpress
        if(Pin_USR_SW1_Read() == 0){
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
            //sprintf( buffer, "Color: %i\n", color);
            //UART_PutString( buffer );
            switch(color){
                case color_RED:
                    //send message that the current color is red
                    sprintf( buffer, "\n\rRED Color\n\r");
                    UART_PutString( buffer );
                    break;
                case color_BLUE: 
                    //send message that the current color is blue
                    sprintf( buffer, "\n\rBLUE Color\n\r");
                    UART_PutString( buffer );
                    break;
                case color_GREEN: 
                    //send message that the current color is green
                    sprintf( buffer, "\n\rGREEN Color\n\r");
                    UART_PutString( buffer );
                    break;
                default:
                    sprintf( buffer, "\n\rUps, something gone wrong!\n\r");
                    UART_PutString( buffer );
            }
        }
        
        
        
        
        
        //send message
        /* Place your application code here. 
        // uart char received?
        if ( cRx != 0 ) {           // char received
            isr_UART_RX_Disable();  // disable Itr
            cInput = cRx;           // copy char
            cRx = 0;                // reset
            isr_UART_RX_Enable();   // enable Itr
        }

        if ( cInput != 0 ) {        // has input
            switch ( cInput ) {     // handle input
                case 'h':
                case 'H':
                    prtHelp( UART_PutString );
                    break;
                    
                case 'L':
                    USERLED_ONOFF( 1u );   // on
                    break;
                case 'l':
                    USERLED_ONOFF( 0u );   // off
                    break;
                    
                case 'c':           // send CAN msg
                    canMsg_1.ide = 0x0;
                    CAN_SendMsg( &canMsg_1 );
                    canMsg_2.ide = 0x0;
                    CAN_SendMsg( &canMsg_2 );
                    break;
                    
                case 'C':           // send extended CAN msg
                    canMsg_1.ide = 0xFF;
                    CAN_SendMsg( &canMsg_1 );
                    canMsg_2.ide = 0xFF;
                    CAN_SendMsg( &canMsg_2 );
                    break;
                    
                case 'S':
                    flagProcessCAN_RX = !flagProcessCAN_RX;
                    sprintf( buffer, "process CAN %s\n\r",
                        (flagProcessCAN_RX) ? "on" : "off");
                    UART_PutString( buffer );
                case 's':
                    sprintf( buffer, 
                        "CAN buffers received since start: %ld\n\r",
                        canRXcnt );
                    UART_PutString( buffer );
                    break;
                    
                default:    // repeat char
                    UART_PutChar( cInput );
                    break;
            } // end switch
            cInput = 0;             // reset, don't forget!
        }
        
        // CAN message
        if ( flagCAN_RX ) {
            if (flagProcessCAN_RX) {
                sprintf( buffer, ">> %6ld, mbx: %d, id: %08lXh, ide: %d, rtr: %d, len: %d\n\r",
                    canRXcnt,
                    canRXmsg.rxMailbox, canRXmsg.id, 
                    canRXmsg.ide, canRXmsg.rtr,
                    canRXmsg.msgLen );
                UART_PutString( buffer );
                uint8 i;
                for ( i = 0; i < canRXmsg.msgLen; i++ ) {
                    sprintf( buffer, " %02X", canRXmsg.msgData[i] );
                    UART_PutString( buffer );
                }
                UART_PutString( "\n\r" );
            }
            flagCAN_RX = 0;         // reset, don't forget!
        }
        
        // for Test Only!
        if ( !Pin_USR_SW1_Read() ) { // Button pressed
            USERLED_ONOFF( 1u );   // User LED on
            CyDelay( 50 );
            USERLED_ONOFF( 0u );   // User LED off
//            UART_PutChar( 0x07 );        // bell on Terminal
        }*/
        
            
    } // end for ever
} // end main

/* [] END OF FILE */
