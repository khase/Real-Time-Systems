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
#include "myCAN.h"

//#define CAN_CFG_REG_SJW_SHIFT         (2u)
///* Bits 3-2 within CSR_CFG */
//#define CAN_CFG_REG_SJW_MASK          ((uint8) ((uint8) 0x03u << CAN_CFG_REG_SJW_SHIFT))
//
//#define CAN_CFG_REG_TSEG2_SHIFT       (5u)
///* Bits 7-5 within CSR_CFG */
//#define CAN_CFG_REG_TSEG2_MASK        ((uint8) ((uint8) 0x07u << CAN_CFG_REG_TSEG2_SHIFT))
//
///* Bits 11-8 within CSR_CFG */
//#define CAN_CFG_REG_TSEG1_SHIFT       (8u)
//#define CAN_CFG_REG_TSEG1_MASK        (0x0Fu)

#define CAN_CSR_CFG__BITRATE_MASK   ( (uint16) 0x7FFF ) //!< bitrate can_cfg_cfr[30:16]
#define CAN_CSR_CFG__BITRATE_SHIFT  ( 0u )              //!< relative to bytes 3:2
#define CAN_CSR_CFG__TSEG1_MASK     ( (uint8) 0x0F )    //!< tseg1 can_cfg_cfr[11:8]
#define CAN_CSR_CFG__TSEG1_SHIFT    ( 0u )              //!< relative to byte 1
#define CAN_CSR_CFG__TSEG2_MASK     ( (uint8) 0xE0 )    //!< tseg2 can_cfg_cfr[7:5]
#define CAN_CSR_CFG__TSEG2_SHIFT    ( 5u )              //!< relative to byte 0
#define CAN_CSR_CFG__SWJ_MASK       ( (uint8) 0x0C )    //!< swj can_cfg_cfr[3:2]
#define CAN_CSR_CFG__SWJ_SHIFT      ( 2u )              //!< relative to byte 0
#define CAN_CSR_CFG__SAMPLEMODE_MASK  ( (uint8) 0x02 )  //!< sample mode can_cfg_cfr[1]
#define CAN_CSR_CFG__SAMPLEMODE_SHIFT ( 1u )            //!< relative to byte 0

/**
 * Get CAN configuration register.
 * @return CAN_CSR_CFG
 * @see http://www.cypress.com/file/136211/download page 1369f
 * @return register can_csr_cfg
 */
uint32 getCAN_CSR_CFG() {
    return CY_GET_REG32( (reg32 *) (CAN_CFG_REG.byte) );
}

/**
 * Get BRP (prescaler) from CAN_CSR_CFG
 * @return bitrate+1 in clock cycles (BRP)
 * @see http://www.cypress.com/file/136211/download page 1369f
 */
uint16 getCANbrp() {
    uint16 canCfgReg34 = CY_GET_REG16( (reg16 *) (&CAN_CFG_REG.byte[2u]) );
    return canCfgReg34 & CAN_CSR_CFG__BITRATE_MASK;
}

/**
 * Get TSeg1 from CAN_CSR_CFG
 * @return TSeg1 = tseg1(CAN_CSR_CFG)+1
 * @see http://www.cypress.com/file/136211/download page 1369f
 */
uint16 getCANtseg1() {
    uint8 canCfgReg1 = CY_GET_REG8( (reg8 *) (&CAN_CFG_REG.byte[1u]) );
    return (canCfgReg1 & CAN_CSR_CFG__TSEG1_MASK) + 1;
}

/**
 * Get TSeg2 from CAN_CSR_CFG
 * @return TSeg2 = tseg2(CAN_CSR_CFG)+1
 * @see http://www.cypress.com/file/136211/download pasge 1369f
 */
uint16 getCANtseg2() {
    uint8 canCfgReg0 = CY_GET_REG8( (reg8 *) (&CAN_CFG_REG.byte[0u]) );
    return ( (canCfgReg0 & CAN_CSR_CFG__TSEG2_MASK) >> 
              CAN_CSR_CFG__TSEG2_SHIFT ) + 1;
}

/**
 * Get SJW from CAN_CSR_CFG
 * @return SJW = sjw(CAN_CSR_CFG)+1
 * @see http://www.cypress.com/file/136211/download page 1369f
 */
uint16 getCANsjw() {
    uint8 canCfgReg0 = CY_GET_REG8( (reg8 *) (&CAN_CFG_REG.byte[0u]) );
    return ( (canCfgReg0 & CAN_CSR_CFG__SWJ_MASK) >> 
              CAN_CSR_CFG__SWJ_SHIFT) + 1;
}

/**
 * Get SamplingMode from CAN_CSR_CFG
 * @return SamplingMode 0: 1 sample, 1: 3 samples
 * @see http://www.cypress.com/file/136211/download page 1369f
 */
uint16 getCANsamplingMode() {
    uint8 canCfgReg0 = CY_GET_REG8( (reg8 *) (&CAN_CFG_REG.byte[0u]) );
    return (canCfgReg0 & CAN_CSR_CFG__SAMPLEMODE_MASK) >> CAN_CSR_CFG__SAMPLEMODE_SHIFT;
}


/* [] END OF FILE */
