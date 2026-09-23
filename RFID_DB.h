#ifndef RFID_DB_H_
#define RFID_DB_H_

#include "STD_TYPES.h"

#define RFID_DB_MAX_CARDS   10   /* الحد الأقصى للكروت التي يمكن تخزينها */
#define RFID_DB_UID_SIZE    4    /* طول معرّف الكارت */

/* أشكال نواتج دالة الإضافة لتسهيل المتابعة */
#define RFID_DB_OK          1
#define RFID_DB_EXISTS      2
#define RFID_DB_FULL        0

/* ====================================================================
 * واجهة الدوال الخاصة بقاعدة البيانات (APIs)
 * ==================================================================== */
u8 RFID_DB_u8AddCard(const u8 *Copy_pu8UID);        /* تسجيل كارت جديد */
u8 RFID_DB_u8IsCardRegistered(const u8 *Copy_pu8UID);/* فحص هل الكارت مسجل مسبقاً */
u8 RFID_DB_u8GetCount(void);                       /* معرفة عدد الكروت المسجلة */

#endif /* RFID_DB_H_ */
