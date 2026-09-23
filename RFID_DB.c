#include <avr/io.h>
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "RFID_DB.h"

/* تقسيم عناوين الـ EEPROM الداخلية */
#define EEPROM_COUNT_ADDR    0x0000  /* العنوان الأول مخصص لعدد الكروت */
#define EEPROM_CARDS_START   0x0001  /* بداية عناوين تخزين الـ UIDs */

/* ====================================================================
 * دوال الـ EEPROM الخاصة بالـ Bare-Metal (Private Functions)
 * ==================================================================== */

static void EEPROM_vWriteByte(u16 Copy_u16Addr, u8 Copy_u8Data) {
    /* تحسين: القراءة قبل الكتابة لحفظ عمر الذاكرة (EEPROM Wear-Leveling) */
    while (EECR & (1 << EEPE));
    EEAR = Copy_u16Addr;
    EECR |= (1 << EERE);
    if (EEDR == Copy_u8Data) {
        return; // لا داعي لإعادة كتابة نفس القيمة
    }

    /* إجراءات الكتابة القياسية للـ AVR */
    EEAR = Copy_u16Addr;
    EEDR = Copy_u8Data;
    EECR |= (1 << EEMPE);
    EECR |= (1 << EEPE);
}

static u8 EEPROM_u8ReadByte(u16 Copy_u16Addr) {
    while (EECR & (1 << EEPE));
    EEAR = Copy_u16Addr;
    EECR |= (1 << EERE);
    return EEDR;
}

/* ====================================================================
 * تنفيذ الدوال العامة (Public Functions)
 * ==================================================================== */

u8 RFID_DB_u8GetCount(void) {
    u8 Local_u8Count = EEPROM_u8ReadByte(EEPROM_COUNT_ADDR);
    
    /* حماية لو الذاكرة جديدة ولم تُكتب من قبل (قيمتها الإفتراضية 0xFF) */
    if (Local_u8Count > RFID_DB_MAX_CARDS) {
        return 0;
    }
    return Local_u8Count;
}

u8 RFID_DB_u8IsCardRegistered(const u8 *Copy_pu8UID) {
    u8 Local_u8Count = RFID_DB_u8GetCount();

    for (u8 Local_u8Index = 0; Local_u8Index < Local_u8Count; Local_u8Index++) {
        u16 Local_u16BaseAddr = EEPROM_CARDS_START + (Local_u8Index * RFID_DB_UID_SIZE);

        if ((EEPROM_u8ReadByte(Local_u16BaseAddr + 0) == Copy_pu8UID[0]) &&
            (EEPROM_u8ReadByte(Local_u16BaseAddr + 1) == Copy_pu8UID[1]) &&
            (EEPROM_u8ReadByte(Local_u16BaseAddr + 2) == Copy_pu8UID[2]) &&
            (EEPROM_u8ReadByte(Local_u16BaseAddr + 3) == Copy_pu8UID[3])) 
        {
            return 1; // الكارت مسجل بالأصل
        }
    }
    return 0; // الكارت غير موجود
}

u8 RFID_DB_u8AddCard(const u8 *Copy_pu8UID) {
    /* 1. التحقق أولاً من عدم وجود الكارت مسبقاً */
    if (RFID_DB_u8IsCardRegistered(Copy_pu8UID) == 1) {
        return RFID_DB_EXISTS; // الكارت مكرر
    }

    u8 Local_u8Count = RFID_DB_u8GetCount();

    /* 2. التأكد من وجود مساحة فارغة */
    if (Local_u8Count < RFID_DB_MAX_CARDS) {
        u16 Local_u16BaseAddr = EEPROM_CARDS_START + (Local_u8Count * RFID_DB_UID_SIZE);

        /* كتابة البايتات الأربعة الخاصة بالـ UID */
        for (u8 Local_u8ByteIdx = 0; Local_u8ByteIdx < RFID_DB_UID_SIZE; Local_u8ByteIdx++) {
            EEPROM_vWriteByte(Local_u16BaseAddr + Local_u8ByteIdx, Copy_pu8UID[Local_u8ByteIdx]);
        }

        /* تحديث وحفظ عداد الكروت الجديد */
        Local_u8Count++;
        EEPROM_vWriteByte(EEPROM_COUNT_ADDR, Local_u8Count);

        return RFID_DB_OK; // تم التسجيل بنجاح
    }

    return RFID_DB_FULL; // الذاكرة ممتلئة
}
