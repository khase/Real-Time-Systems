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
#ifndef CYAPICALLBACKS_H
#define CYAPICALLBACKS_H
    
    /*Define your macro callbacks here */

    /*For more information, refer to the Macro Callbacks topic in the PSoC Creator Help.*/
    
    // CAN receive
//    #define CAN_MSG_RX_ISR_CALLBACK  // message received callback from CAN_INT.c
//    void CAN_MsgRXIsr_Callback();
    
    /** 
     * \def CAN_RECEIVE_MSG_CALLBACK
     * Switch on callback
     * @see CAN_ReceiveMsg()
     */
    #define CAN_RECEIVE_MSG_CALLBACK
    
    /** @brief Declare CAN_ReceiveMsg_Callback */
    void CAN_ReceiveMsg_Callback();
    
#endif /* CYAPICALLBACKS_H */   
/* [] */
