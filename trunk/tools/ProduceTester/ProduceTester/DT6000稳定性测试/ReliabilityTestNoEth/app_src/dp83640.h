#pragma once

/*
 *  PHY_BMCR    Basic Mode Control Register 
 *  
 *  bit12   AUTO-NEGOTIATION ENABLE
 *  bit11   POWER DOWN
 *  bit9    RESTART AUTO-NEGOTIATION
 *  bit8    DUPLEX MODE         1 = Full Duplex operation.  0 = Half Duplex operation.
 */
#define PHY_BMCR        0x00
#define PHY_BMSR        0x01
#define PHY_PHYIDR1     0x02
#define PHY_PHYIDR2     0x03
#define PHY_ANAR        0x04
#define PHY_ANLPAR      0x05
#define PHY_ANER        0x06
#define PHY_ANNPTR      0x07

/*
 *  PHY_PHYSTS   PHY Status Register 
 * 
 *  bit4        AUTO-NEG COMPLETE  1 = Auto-Negotiation complete.
 *  bit2        DUPLEX STATUS      1 = Full duplex mode.  0 = Half duplex mode.
 */
#define PHY_PHYSTS      0x10
#define PHY_MICR        0x11
/*
 *  PHY_MISR     MII Interrupt Status and Event Control Register
 *  
 *  bit15       LQ_INT          Link Quality Intr
 *  bit14       ED_INT          Energy Detect Intr
 *  bit13       LINK_INT        Change of Speed Status Intr   
 *  bit12       SPD_INT         Change of Speed Status Intr     MICR[3] = 0
 *              SIPD_DUP_INT    Change of speed/Duplex Intr     MICR[3] = 1
 *  bit11       DUP_INT         Change of Duplex Status Intr    MICR[3] = 0
 *              PTP_INT         PTP Intr                        MICR[3] = 1
 *  bit10       ANC_INT         Auto Negotiation Complete Intr  
 *  bit9        FHF_INT         False Carrier Counter Half-Full Interrupt    PHYCR2 [8:7] are both 0
 *              CTR_INT         CTR Intr                       
 *  bit8        RHF_INT         Receive Error Counter half-full interrupt:   PHYCR2 [8:7] are both 0
 *              PCF_INT         PCF Interrupt:
 *  7           LQ_INT_EN       Enable Interrupt on Link Quality Monitor event.
 *  6           ED_INT_EN       Enable Interrupt on energy detect event.
 *  5           LINK_INT_EN     Enable Interrupt on change of link status.
 *  4           SPD_INT_EN      Enable Interrupt on change of speed status.
 *  3           DUP_INT_EN      Duplex Interrupt:  Enable Interrupt on change of duplex status. This function is selected if MICR[3] is set to 0.
 *              PTP_INT_EN      PTP Interrupt:   PTP interrupt. This function is selected if MICR[3] is set to 1.
 *  2           ANC_INT_EN      Enable Interrupt on auto-negotiation complete event.
 *  1           FHF_INT_EN      FHF Interrupt:  Enable Interrupt on False Carrier Counter Register halffull event. This function is selected if the PHYCR2[8:7] are both 0.
 *              CTR_INT_EN      CTR Interrupt: Enable interrupt on either Receive Error Counter Register half-full event or False Carrier Counter Register half-full event. This function is selected if either of PCFCR[7:6] are set.
 *  0           RHF_INT_EN      RHF Interrupt: Enable Interrupt on Receive Error Counter Register halffull event. This function is selected if the PHYCR2[8:7] are both 0.
                PCF_INT_EN      PCF Interrupt: Enable Interrupt on a PHY Control Frame event. This function is selected if either of PCFCR[7:6] are set.
 */
#define PHY_MISR        0x12
#define PHY_PAGESEL     0x13

/************************************************************************/
/* PAGE0                                                                */
/************************************************************************/
#define PHY_FCSCR       0x14
#define PHY_RECH        0x15
#define PHY_PCSR        0x16
#define PHY_RBR         0x17
#define PHY_LEDCR       0x18
/*
 *  PHY_PHYCR       PHY Control Register 
 *  bit[4:0]        PHYADDR
 */
#define PHY_PHYCR       0x19

#define PHY_10BTSCR     0x1A
#define PHY_CDCTRL1     0x1B
#define PHY_PHYCR2      0x1C
#define PHY_EDCR        0x1D
#define PHY_PCFCR       0x1F

/************************************************************************/
/* PAGE1                                                                */
/************************************************************************/
#define PHY_SD_CNFG     0x1E