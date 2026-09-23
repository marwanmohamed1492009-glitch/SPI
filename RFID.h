#ifndef RFID_H_
#define RFID_H_

#include "STD_TYPES.h"

/* ====================================================================
 * 1. عناوين المسجلات المقيدة (Register Addresses)
 * ==================================================================== */
#define MFRC522_REG_COMMAND         0x01
#define MFRC522_REG_COM_IEN         0x02
#define MFRC522_REG_COM_IRQ         0x04
#define MFRC522_REG_ERROR           0x06
#define MFRC522_REG_FIFO_DATA       0x09
#define MFRC522_REG_FIFO_LEVEL      0x0A
#define MFRC522_REG_CONTROL         0x0C
#define MFRC522_REG_BIT_FRAMING     0x0D
#define MFRC522_REG_MODE            0x11
#define MFRC522_REG_TX_CONTROL      0x26  /* تم تصحيحه ليكون مطابقاً للـ Datasheet */
#define MFRC522_REG_TX_ASK          0x15
#define MFRC522_REG_T_MODE          0x2A
#define MFRC522_REG_T_PRESCALER     0x2B
#define MFRC522_REG_T_RELOAD_H      0x2C
#define MFRC522_REG_T_RELOAD_L      0x2D

/* ====================================================================
 * 2. الأوامر والقيم المشتركة (Commands & Masks)
 * ==================================================================== */
#define MFRC522_CMD_IDLE            0x00
#define MFRC522_CMD_TRANSCEIVE      0x0C
#define MFRC522_CMD_SOFT_RESET      0x0F

#define PICC_CMD_REQA               0x26
#define PICC_CMD_ANTICOLL           0x93
#define PICC_ANTICOLL_NVB           0x20

#define BIT_FRAMING_START_SEND      0x80
#define BIT_FRAMING_7BITS           0x07
#define BIT_FRAMING_FULL_BYTES      0x00

#define COM_IRQ_CLEAR_ALL           0x7F
#define COM_IRQ_RX_DONE             0x20
#define COM_IRQ_TIMER_TIMEOUT       0x01

#define FIFO_LEVEL_FLUSH            0x80
#define EXPECTED_ANTICOLL_BYTES     5

#define TIMER_MODE_AUTO_PRESCALER_H 0x8D
#define TIMER_PRESCALER_L           0x3E
#define TIMER_RELOAD_VAL_H          0x00
#define TIMER_RELOAD_VAL_L          0x1E

#define TX_ASK_FORCE_100            0x40
#define MODE_REG_TX_WAIT_RF         0x3D
#define TX_CONTROL_ANTENNA_ON       0x03

/* ====================================================================
 * 3. واجهة الدوال الخارجيه (APIs)
 * ==================================================================== */
void   RFID_vInit(void);
void   RFID_vWriteRegister(u8 Copy_u8RegAddr, u8 Copy_u8Val);
u8     RFID_u8ReadRegister(u8 Copy_u8RegAddr);
u8     RFID_u8CheckCard(void);
u8     RFID_u8ReadUID(u8 *Copy_pu8UIDBuffer);

#endif /* RFID_H_ */
