#include <avr/io.h>
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "SPI.h"
#include "RFID.h"

/* ====================================================================
 * إعدادات الطرف الخاص بـ Chip Select (عدله حسب بن التوصيل عندك)
 * ==================================================================== */
#define CS_PORT    PORTB
#define CS_DDR     DDRB
#define CS_PIN     PIN4_ID  /* أو رقم البن 4 مباشرة */

#define SPI_WRITE_MASK   0x7E
#define SPI_READ_MASK    0x80

/* ====================================================================
 * دوال التحكم المباشر بالـ Chip Select (Private Functions)
 * ==================================================================== */
static void CS_vSelect(void) {
    CLR_BIT(CS_PORT, CS_PIN); // Low = Active
}

static void CS_vDeselect(void) {
    SET_BIT(CS_PORT, CS_PIN); // High = Idle
}

/* ====================================================================
 * تنفيذ الدوال الأساسية (Public Functions)
 * ==================================================================== */

void RFID_vWriteRegister(u8 Copy_u8RegAddr, u8 Copy_u8Val) {
    CS_vSelect();
    SPI_vTransmit(((Copy_u8RegAddr << 1) & SPI_WRITE_MASK));
    SPI_vTransmit(Copy_u8Val);
    CS_vDeselect();
}

u8 RFID_u8ReadRegister(u8 Copy_u8RegAddr) {
    u8 Local_u8Val = 0;
    CS_vSelect();
    SPI_vTransmit(((Copy_u8RegAddr << 1) & SPI_WRITE_MASK) | SPI_READ_MASK);
    Local_u8Val = SPI_u8Receive();
    CS_vDeselect();
    return Local_u8Val;
}

void RFID_vInit(void) {
    /* ضبط طرف الـ CS كـ Output ورَفعه (Idle High) */
    SET_BIT(CS_DDR, CS_PIN);
    CS_vDeselect();

    /* إعادة ضبط الشريحة (Soft Reset) */
    RFID_vWriteRegister(MFRC522_REG_COMMAND, MFRC522_CMD_SOFT_RESET);

    /* ضبط التايمر ومهلة الحماية */
    RFID_vWriteRegister(MFRC522_REG_T_MODE, TIMER_MODE_AUTO_PRESCALER_H);
    RFID_vWriteRegister(MFRC522_REG_T_PRESCALER, TIMER_PRESCALER_L);
    RFID_vWriteRegister(MFRC522_REG_T_RELOAD_H, TIMER_RELOAD_VAL_H);
    RFID_vWriteRegister(MFRC522_REG_T_RELOAD_L, TIMER_RELOAD_VAL_L);

    /* ضبط التردد وهوائي الإرسال */
    RFID_vWriteRegister(MFRC522_REG_TX_ASK, TX_ASK_FORCE_100);
    RFID_vWriteRegister(MFRC522_REG_MODE, MODE_REG_TX_WAIT_RF);

    /* تشغيل الهوائي Antenna ON */
    u8 Local_u8TxState = RFID_u8ReadRegister(MFRC522_REG_TX_CONTROL);
    RFID_vWriteRegister(MFRC522_REG_TX_CONTROL, Local_u8TxState | TX_CONTROL_ANTENNA_ON);
}

u8 RFID_u8CheckCard(void) {
    RFID_vWriteRegister(MFRC522_REG_COMMAND, MFRC522_CMD_IDLE);
    RFID_vWriteRegister(MFRC522_REG_FIFO_LEVEL, FIFO_LEVEL_FLUSH);
    RFID_vWriteRegister(MFRC522_REG_COM_IRQ, COM_IRQ_CLEAR_ALL);

    RFID_vWriteRegister(MFRC522_REG_FIFO_DATA, PICC_CMD_REQA);
    RFID_vWriteRegister(MFRC522_REG_BIT_FRAMING, BIT_FRAMING_START_SEND | BIT_FRAMING_7BITS);
    RFID_vWriteRegister(MFRC522_REG_COMMAND, MFRC522_CMD_TRANSCEIVE);

    u16 Local_u16Timeout = 2000;
    while (Local_u16Timeout--) {
        u8 Local_u8IRQ = RFID_u8ReadRegister(MFRC522_REG_COM_IRQ);
        if (Local_u8IRQ & COM_IRQ_RX_DONE) {
            return 1; // تم اكتشاف كارت
        }
        if (Local_u8IRQ & COM_IRQ_TIMER_TIMEOUT) {
            break;
        }
    }
    return 0; // لم يتم إيجاد كارت
}

u8 RFID_u8ReadUID(u8 *Copy_pu8UIDBuffer) {
    RFID_vWriteRegister(MFRC522_REG_COMMAND, MFRC522_CMD_IDLE);
    RFID_vWriteRegister(MFRC522_REG_FIFO_LEVEL, FIFO_LEVEL_FLUSH);
    RFID_vWriteRegister(MFRC522_REG_COM_IRQ, COM_IRQ_CLEAR_ALL);

    RFID_vWriteRegister(MFRC522_REG_FIFO_DATA, PICC_CMD_ANTICOLL);
    RFID_vWriteRegister(MFRC522_REG_FIFO_DATA, PICC_ANTICOLL_NVB);

    RFID_vWriteRegister(MFRC522_REG_BIT_FRAMING, BIT_FRAMING_START_SEND | BIT_FRAMING_FULL_BYTES);
    RFID_vWriteRegister(MFRC522_REG_COMMAND, MFRC522_CMD_TRANSCEIVE);

    u16 Local_u16Timeout = 2000;
    while (Local_u16Timeout--) {
        u8 Local_u8IRQ = RFID_u8ReadRegister(MFRC522_REG_COM_IRQ);
        if (Local_u8IRQ & COM_IRQ_RX_DONE) break;
        if (Local_u8IRQ & COM_IRQ_TIMER_TIMEOUT) return 0;
    }

    /* قراءة الـ UID من الـ FIFO */
    for (u8 Local_u8Index = 0; Local_u8Index < EXPECTED_ANTICOLL_BYTES; Local_u8Index++) {
        Copy_pu8UIDBuffer[Local_u8Index] = RFID_u8ReadRegister(MFRC522_REG_FIFO_DATA);
    }

    /* فحص صحة البيانات بـ XOR Checksum */
    u8 Local_u8BCC = Copy_pu8UIDBuffer[0] ^ Copy_pu8UIDBuffer[1] ^ Copy_pu8UIDBuffer[2] ^ Copy_pu8UIDBuffer[3];
    return (Local_u8BCC == Copy_pu8UIDBuffer[4]) ? 1 : 0;
}
