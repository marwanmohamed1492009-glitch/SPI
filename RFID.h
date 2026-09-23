#ifndef MFRC522_H_
#define MFRC522_H_

#include "STD_TYPES.h"
#include "BIT_MATH.h"

// --- Status Codes ---
#define MI_OK         0
#define MI_NOTAGERR   1
#define MI_ERR        2

// --- MFRC522 Registers ---
#define CommandReg    0x01
#define CommIEnReg    0x02
#define CommIrqReg    0x04
#define ErrorReg      0x06
#define FIFODataReg   0x09
#define FIFOLevelReg  0x0A
#define ControlReg    0x0C
#define BitFramingReg 0x0D
#define ModeReg       0x11
#define TxControlReg  0x26

// --- Commands ---
#define PCD_IDLE      0x00
#define PCD_TRANSCEIVE 0x0C
#define PCD_RESETPHASE 0x0F

#define PICC_REQIDL   0x26
#define PICC_ANTICOLL 0x93

// --- Low-Level Functions ---
void Write_Reg(u8 Copy_u8Addr, u8 Copy_u8Val);
u8 Read_Reg(u8 Copy_u8Addr);
u8 ToCard(u8 Copy_u8Command, u8 *Copy_pu8SendData, u8 Copy_u8SendLen, u8 *Copy_pu8BackData, u16 *Copy_pu16BackLen);

// --- High-Level Functions ---
void MFRC522_Init(void);
u8 MFRC522_Request(u8 *Copy_pu8TagType);
u8 MFRC522_Anticoll(u8 *Copy_pu8SerNum);

#endif /* MFRC522_H_ */
