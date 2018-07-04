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
#include "etCount.h"    // execution time counter
#include <inttypes.h>
#include <time.h>
#include <stdlib.h>

int gj;                 // a global var

/**
 * Control the User LED on board
 * @param sts 0: off, 1: on
 */
inline void usrLED( uint8 sts ) {
    Pin_USR_LED_Write( sts );
}
   

int tsta(int a){
    int ret = a;
    for (int i = 0; i < 10000; i++ ) {
        //ret += a;
    }
    return ret;
}

int tstb(int a, int b){
    int ret = a + b;
    for (int i = 0; i < 10000; i++ ) {
        //ret += a + b;
    }
    return ret;
}

int tstc(int a, int b, int c){
    int ret = a + b + c;
    for (int i = 0; i < 10000; i++ ) {
        //ret += a + b + c;
    }
    return ret;
}

int tstd(int a, int b, int c, int d){
    int ret = a + b + c + d;
    for (int i = 0; i < 10000; i++ ) {
        //ret += a + b + c + d;
    }
    return ret;
}

int tste(int a, int b, int c, int d, int e){
    int ret = a + b + c + d + e;
    for (int i = 0; i < 10000; i++ ) {
        //ret += a + b + c + d + e;
    }
    return ret;
}

int tstf(int a, int b, int c, int d, int e, int f){
    int ret = a + b + c + d + e + f;
    for (int i = 0; i < 10000; i++ ) {
        //ret += a + b + c + d + e + f;
    }
    return ret;
}

int tstg(int a, int b, int c, int d, int e, int f, int g){
    int ret = a + b + c + d + e + f + g;
    for (int i = 0; i < 10000; i++ ) {
        //ret += a + b + c + d + e + f + g;
    }
    return ret;
}

unsigned long* statistics(void* func(int), int count){
    unsigned long* res;
    
    for (int i = 0; i < count; i++){
        unsigned long cnt;
        CNT_START_RES;  // reset counter
        func(1);
        CNT_LAP_STOP( cnt );   // get counter value, stop counter
        
        res[i] = cnt;
    }
    
    return res;
}

uint64_t fibbonacci(uint64_t n) {
   if(n == 0){
      return 0;
   } else if(n == 1) {
      return 1;
   } else {
      return (fibbonacci(n-1) + fibbonacci(n-2));
   }
}

// computes all prims up to range
int getHightestPrim(int range){
    int x, i,n;
    for (x = 2; x <= range; x++)
    {
        int rnd  = rand()/1000000; 
        int sum = 0;
        if ( rnd %2 == 0){
            for(int j = 0; j < rnd;j++){
                sum += j;
                sum /= 100;
            }
        }
        for (i = 2; i <= x; i++)
        {
            if (x%i == 0 && x != i) 
                break;
            if (i == x){
                n = x;
                sum++;
            }
        }
    }
    return n;
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
static char cRx = 0;            //!< caharcter from UART, visible within main.c

/** 
 * UART RX interrupt service routine.
 *
 * Collect a characters from UART into a global input variable.
 * @see cRx
 */
CY_ISR( MyIsrUartRX ) {
    cRx = UART_GetChar();     // copy char
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++   Candidates for new include and implementation file   ++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ------------------------------------------------------------------------------------

/**
 * Basic project with UART, RX-Interrupt, user-Button and -LED. With HW realization
 * of execution time counter. See the TopDesign and the macros in etCount.h
 * for FreeSoC2-5LP
 * 
 * Help and diverse functionalities
 * @see help.c
 *
 * @author Ralf S. Mayer
 * @date 2018-04-27
 * @version 1.1
 */
int main()
{
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */   
    UART_Start();                       // start UART
    isr_UART_RX_StartEx( MyIsrUartRX ); // enable itr uart
    
    
    // +++ Initialize Counter and Registers +++
    Control_Reg_Write( CNT_DISABLE_RESET );  // reset, disable
    unsigned long cnt, cnt1, cnt2,cnt3, cnt4;       // counter values
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    sprintf( buffer, "\n\rWelcome! %s\n\r", _VERSTR_);
    UART_PutString( buffer );
    // help text
    prtHelp( UART_PutString );
        
    // +++ Test the execution time counter +++
    CNT_START_RES;      // reset and start counter
    CNT_LAP( cnt );     // get counter value
    CyDelay(1);         // test delay in ms
    CNT_LAP( cnt1 );    // get counter value
    CyDelay(2);         // test delay in ms
    CNT_LAP( cnt2 );    // get counter value
    CyDelay(5);         // test delay in ms
    CNT_LAP( cnt3 );    // get counter value
    CyDelay(10);        // test delay in ms
    CNT_LAP_STOP( cnt4 );   // get counter value, stop counter
    CNT_START_RES;      // reset and start counter
    sprintf( buffer, "Delay 0, 1, 2, 5, 10ms: %lu, %lu, %lu, %lu, %lu\n\r",
        cnt, cnt1, cnt2, cnt3, cnt4 );
    UART_PutString( buffer );
    CNT_LAP_STOP( cnt );   // get counter value, stop counter
    sprintf( buffer, "Time for 'printf' %lu\n\r", cnt );
    UART_PutString( buffer );
    // --- End Test the execution time counter ---
    
    char cInput = 0;        // UART RX char copy
    int i;
    volatile int vi;
    int lj;                 // a lokal var
    int a, b = 0;
    
    // Main loop
    for(;;)     // forever
    {
        /* Place your application code here. */
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
//                    usrLED( 1u );   // on
                    break;
                case 'l':
//                    usrLED( 0u );   // off
                    break;
                    
                case 'I':
                    CNT_START_RES;  // reset counter
                    for ( i = 0; i < 10000; i++ );
                    CNT_LAP_STOP( cnt );   // get counter value, stop counter
                    sprintf( buffer, "Time for loop i 0...10000: %lu\n\r", cnt );
                    UART_PutString( buffer );
                    break;
                case 'i':
                    CNT_START_RES;  // reset counter
                    for ( vi = 0; vi < 10000; vi++ );
                    CNT_LAP_STOP( cnt );   // get counter value, stop counter
                    sprintf( buffer, "Time for loop vi 0...10000: %lu\n\r", cnt );
                    UART_PutString( buffer );
                    break;
                case 'J':
                    CNT_START_RES;  // reset counter
                    for ( i = 0; i < 10000; i++ ) {
                        gj = i;
                    }
                    CNT_LAP_STOP( cnt );   // get counter value, stop counter
                    sprintf( buffer, "Time for loop global=i 0...10000: %lu\n\r", cnt );
                    UART_PutString( buffer );
                    break;
                case 'j':
                    CNT_START_RES;  // reset counter
                    for ( i = 0; i < 10000; i++ ) {
                        lj = i;
                    }
                    CNT_LAP_STOP( cnt );   // get counter value, stop counter
                    sprintf( buffer, "Time for loop local=i 0...10000: %lu\n\r", cnt );
                    UART_PutString( buffer );
                    break;
                case 'a':
                    CNT_START_RES;  // reset counter
                    for ( i = 0; i < 999999; i++ ) {
                        if ((i == 100 && i < 100) || i % 2 == 0 ){
                            a++;
                        }
                        else{
                            b++;
                        }
                    }
                    lj = a + b;
                    CNT_LAP_STOP( cnt );   // get counter value, stop counter
                    sprintf( buffer, "Time for branch if/else: %lu\n\r", cnt );
                    UART_PutString( buffer );
                    break;
                case 'A':
                    CNT_START_RES;  // reset counter
                    for ( i = 0; i < 999999; i++ ) {
                        switch (i % 2){
                            case 0:
                                a++;
                                break;
                            case 1:
                                b++;
                                break;
                            default:
                                b++;
                                break;
                        }
                    }
                    lj = a + b;
                    CNT_LAP_STOP( cnt );   // get counter value, stop counter
                    sprintf( buffer, "Time for branch switch: %lu\n\r", cnt );
                    UART_PutString( buffer );
                    break;
                case 'f':
                    CNT_START_RES;  // reset counter
                    lj = tsta(1);
                    CNT_LAP_STOP( cnt );   // get counter value, stop counter
                    sprintf( buffer, "Time for function with 1 param: %lu\n\r", cnt );
                    UART_PutString( buffer );
                    CNT_START_RES;  // reset counter
                    lj = tstb(1, 2);
                    CNT_LAP_STOP( cnt );   // get counter value, stop counter
                    sprintf( buffer, "Time for function with 2 param: %lu\n\r", cnt );
                    UART_PutString( buffer );
                    CNT_START_RES;  // reset counter
                    lj = tstc(1, 2, 3);
                    CNT_LAP_STOP( cnt );   // get counter value, stop counter
                    sprintf( buffer, "Time for function with 3 param: %lu\n\r", cnt );
                    UART_PutString( buffer );
                    CNT_START_RES;  // reset counter
                    lj = tstd(1, 2, 3, 4);
                    CNT_LAP_STOP( cnt );   // get counter value, stop counter
                    sprintf( buffer, "Time for function with 4 param: %lu\n\r", cnt );
                    UART_PutString( buffer );
                    CNT_START_RES;  // reset counter
                    lj = tste(1, 2, 3, 4, 5);
                    CNT_LAP_STOP( cnt );   // get counter value, stop counter
                    sprintf( buffer, "Time for function with 5 param: %lu\n\r", cnt );
                    UART_PutString( buffer );
                    CNT_START_RES;  // reset counter
                    lj = tstf(1, 2, 3, 4, 5, 6);
                    CNT_LAP_STOP( cnt );   // get counter value, stop counter
                    sprintf( buffer, "Time for function with 6 param: %lu\n\r", cnt );
                    UART_PutString( buffer );
                    CNT_START_RES;  // reset counter
                    lj = tstg(1, 2, 3, 4, 5, 6, 7);
                    CNT_LAP_STOP( cnt );   // get counter value, stop counter
                    sprintf( buffer, "Time for function with 7 param: %lu\n\r", cnt );
                    UART_PutString( buffer );
                    break;
                case 'd':
                    CNT_START_RES;  // reset counter
                    float aa = 1.31;
                    float b = 3.31;
                    float sum = 0;
                    for ( i = 0; i < 10000; i++ ) {
                        sum = aa+b;
                    }
                    CNT_LAP_STOP( cnt );   // get counter value, stop counter
                    sprintf( buffer, "Time for floats: %lu\n\r", cnt );
                    UART_PutString( buffer ); 
                    
                    CNT_START_RES;  // reset counter
                    double a1 = 1.31;
                    double b2 = 3.31;
                    double sum1 = 0;
                    for ( i = 0; i < 10000; i++ ) {
                        sum1 = a1+b2;
                    }
                    CNT_LAP_STOP( cnt );   // get counter value, stop counter
                    sprintf( buffer, "Time for double: %lu\n\r", cnt );
                    UART_PutString( buffer );
                    break;
                case 't':
                    CNT_START_RES;  // reset counter
                    unsigned long* res = statistics((void*)tsta, 200);
                    CNT_LAP_STOP( cnt );   // get counter value, stop counter
                    sprintf( buffer, "result pointer: %d\n\r", res);
                    UART_PutString( buffer );
                    for (int i = 0; i < 200; i++){
                        sprintf( buffer, "Time for %d: %lu\n\r", i, res[i]);
                        UART_PutString( buffer );
                    }
                    break;
               case 'q':
                    CNT_START_RES;  // reset counter
                    
                    uint64_t fn_1 = 0;
                    uint64_t fn_2 = 1;
                    uint64_t fnew;
                                      
                    for ( i = 0; i < 45; i++ ) {
                        fnew = fn_1 + fn_2;
                        fn_2 = fn_1;
                        fn_1 = fnew;
                    }
                    CNT_LAP_STOP( cnt );   // get counter value, stop counter
                    sprintf( buffer, "Höchste gefundene Fibonaccizahl: %lu\n\r" , fnew);
                    UART_PutString(buffer);
                    sprintf( buffer, "Time for fibonacci in loop with 45 steps: %lu\n\r", cnt );
                    UART_PutString( buffer );
                    
                    // Rekursiver Aufruf
                    CNT_START_RES;  // reset counter
                    uint64_t fn;
                    fn = fibbonacci(20)
                    CNT_LAP_STOP( cnt );   // get counter value, stop counter
                    sprintf( buffer, "Höchste gefundene Fibonaccizahl : %lu\n\r", fn);
                    UART_PutString(buffer);
                    sprintf( buffer, "Time for fibonacci(rekursiv), depth = 20: %lu\n\r", cnt );
                    UART_PutString( buffer );
                    break;
                case 'y':
                    CNT_START_RES;  // reset counter
                    CNT_LAP_STOP( cnt )
                    long bestTime = -100;
                    sprintf( buffer, "STARTE !!!!\n\r");   
                    srand((unsigned) time(0));
                    UART_PutString( buffer );
                    long worstTime = 0;
                    int amount = 1000;
                    for(int i = 0; i < amount; i++)
                    {
                        CNT_START_RES;  // reset counter
                        int x;
                        x = getHightestPrim(100);
                        CNT_LAP_STOP( cnt );   // get counter value, stop counter
                        if(cnt > worstTime){
                            worstTime = cnt;
                        }
                        if(cnt < bestTime || bestTime <= 0){
                            bestTime = cnt;
                        }
                    }
                    sprintf( buffer, "Run getHightestPrim() for %i times. :\n\r ", amount);
                    UART_PutString( buffer );
                    sprintf( buffer, "Result:\nBest Execution Time for Prim: %lu\n\r", bestTime );
                    UART_PutString( buffer );
                    sprintf( buffer, "Worst Execution Time for Prim: %lu\n\r", worstTime );
                    UART_PutString( buffer );
                    
                    
                    break; 
                    default:    // repeat char
                        UART_PutChar( cInput );
                        break;
            } // end switch
            cInput = 0;             // reset, don't forget!
        }
        
        // for Test Only!
        if ( !Pin_USR_SW1_Read() ) { // Button pressed
//            usrLED( 1u );   // on
            CyDelay( 50 );
  //          usrLED( 0u );   // off          
        }
            
    } // end for ever
} // end main

/* [] END OF FILE */
