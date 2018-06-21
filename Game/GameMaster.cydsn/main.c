/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include <stdio.h>
#include <stdlib.h>

char buffer[128];

void set(uint8 r, uint8 g, uint8 b){
    Red_WriteCompare(r);
    Green_WriteCompare(g);
    Blue_WriteCompare(b);
}

void start(){
    int r = 0;
    for(int i = 50; i < 200; i += 5)
    {
        r = rand() % 5;
        sprintf( buffer, "\n\rBlabla\n\r");
        UART_1_PutString( buffer );
        
        set(50, 0, 0);
        i += r;
        if (i >= 200){
            set(255, 0, 0);
            break;
        }
        CyDelay(i);
        
        set(0, 50, 0);
        i += r;
        if (i >= 200){
            set(0, 255, 0);
            break;
        }
        CyDelay(i);
        
        set(0, 0, 50);
        i += r;
        if (i >= 200){
            set(0, 0, 255);
            break;
        }
        CyDelay(i);
    }
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UART_1_Start();  
    
    Red_Start();
    Green_Start();
    Blue_Start();
    
    PRS_1_Start();
    
    srand(PRS_1_Read());
    
    sprintf( buffer, "\n\rWelcome!\n\r");
    UART_1_PutString( buffer );
    
    Pin_USR_LED_Write(1u);
    start();
    Pin_USR_LED_Write(0u);
    for(;;)
    {
    }
}

/* [] END OF FILE */
