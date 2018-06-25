/* ========================================
 *
 * Copyright Ralf S. Mayer, 2018
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
 */
#ifndef MYCAN_H_
#define MYCAN_H_

#include <project.h>
#include "CAN.h"

/* Bit 20 within RX_CMD[i] */
#define CAN_RX_RTR_SHIFT (5u)   //!< Bit 20 within RX_CMD[i], shift within reg
/** 
 * \def CAN_RX_RTR_MASK
 * Mask for RTR bit
 */
#define CAN_RX_RTR_MASK  ((uint8) ((uint8) 0x01u << CAN_RX_RTR_SHIFT))
/** 
 * \def CAN_GET_RX_RTR
 * Macro to get RTR bit in CAN_RX
 * @param i mailbox number
 * @return RTR bit
 */
#define CAN_GET_RX_RTR(i)         ((uint8) \
    (CAN_RX[i].rxcmd.byte[2u] & CAN_RX_RTR_MASK) >> CAN_RX_RTR_SHIFT)

/**
 * CAN receive message structure
 */
typedef struct {
    uint32 id;          //<! CAN id
    uint8  ide;         //<! IDE 0: standard (11bit), != 0: extended (29bit) ID
    uint8  rtr;         //<! RTR flag
    uint8  msgLen;      //<! CAN message data length
    uint8  rxMailbox;   //<! RX mailbox
    uint8  msgData[8];  //<! CAN message data payload
} canRXmsg_t;


/**
 * \def CAN_RX_DATA_SEQUENCE
 * Access to RX DATA for mailbox(i) using defines in CAN.h.
 * CAN data bytes are not sequentially ordered. 
 * See CAN Reveive Register Map, page 8 in
 * @see https://archive.eetasia.com/www.eetasia.com/STATIC/PDF/200910/EEOL_2009OCT09_INTD_AN_01.pdf?SOURCES=DOWNLOAD
 * @see http://www.cypress.com/file/123561/download
 * and
 * @see CAN_RX_STRUCT in CAN.h
 * Usage e.g. in
 * @see CAN_ReceiveMsg()
 *
 * @todo could also be realized by a logical function like
 * @see CAN_TX_DATA_BYTE
 */
#define CAN_RX_DATA_SEQUENCE {\
    CAN_DATA_BYTE_1, CAN_DATA_BYTE_2,\
    CAN_DATA_BYTE_3, CAN_DATA_BYTE_4,\
    CAN_DATA_BYTE_5, CAN_DATA_BYTE_6,\
    CAN_DATA_BYTE_7, CAN_DATA_BYTE_8}

// +++ declaration +++

/** @brief Get CAN configuration register */
uint32 getCAN_CSR_CFG();

/** @brief Get BRP (prescaler) from CAN_CSR_CFG */
uint16 getCANbrp();

/** @brief Get TSeg1 from CAN_CSR_CFG */
uint16 getCANtseg1();

/** @brief Get TSeg2 from CAN_CSR_CFG */
uint16 getCANtseg2();

/** @brief Get SJW from CAN_CSR_CFG */
uint16 getCANsjw();

/** @brief Get SamplingMode from CAN_CSR_CFG */
uint16 getCANsamplingMode();

#endif // MYCAN_H_
/* [] END OF FILE */
