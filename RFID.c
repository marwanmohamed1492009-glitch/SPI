#include <avr/io.h> // مكتبة الـ AVR القياسية للمنافذ
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "SPI.h"
#include "RFID.h"  

#define CS_PORT   PORTB
#define CS_PIN    4

// --- CS Pin Control ---
static void CS_Select(void) {
	CLR_BIT(CS_PORT, CS_PIN); // CS = 0 (Active Low)
}

static void CS_Deselect(void) {
	SET_BIT(CS_PORT, CS_PIN); // CS = 1 (Idle High)
}

// --- Low-Level Register Access ---
void Write_Reg(u8 Copy_u8Addr, u8 Copy_u8Val) {
	CS_Select();
	SPI_transmit((Copy_u8Addr << 1) & 0x7E); // Bit 0 = 0 (Write)
	SPI_transmit(Copy_u8Val);
	CS_Deselect();
}

u8 Read_Reg(u8 Copy_u8Addr) {
	u8 Local_u8Val;
	CS_Select();
	SPI_transmit(((Copy_u8Addr << 1) & 0x7E) | 0x80); // Bit 7 = 1 (Read)
	Local_u8Val = SPI_transmit(0x00);                  // Send Dummy Data
	CS_Deselect();
	return Local_u8Val;
}

u8 ToCard(u8 Copy_u8Command, u8 *Copy_pu8SendData, u8 Copy_u8SendLen, u8 *Copy_pu8BackData, u16 *Copy_pu16BackLen) {
	u8 Local_u8Status = MI_ERR;
	u8 Local_u8IrqEn = 0x77;
	u8 Local_u8WaitIrq = 0x30;
	u8 Local_u8N;
	u16 Local_u16I;

	Write_Reg(CommIEnReg, Local_u8IrqEn | 0x80);
	Write_Reg(CommIrqReg, 0x7F);
	Write_Reg(CommandReg, PCD_IDLE);
	Write_Reg(FIFOLevelReg, 0x80); // Clear FIFO

	for (Local_u16I = 0; Local_u16I < Copy_u8SendLen; Local_u16I++) {
		Write_Reg(FIFODataReg, Copy_pu8SendData[Local_u16I]);
	}

	Write_Reg(CommandReg, Copy_u8Command);
	if (Copy_u8Command == PCD_TRANSCEIVE) {
		Write_Reg(BitFramingReg, Read_Reg(BitFramingReg) | 0x80);
	}

	Local_u16I = 2000;
	do {
		Local_u8N = Read_Reg(CommIrqReg);
		Local_u16I--;
	} while ((Local_u16I != 0) && !(Local_u8N & 0x01) && !(Local_u8N & Local_u8WaitIrq));

	if (Local_u16I != 0 && !(Read_Reg(ErrorReg) & 0x1B)) {
		Local_u8Status = MI_OK;
		if (Copy_u8Command == PCD_TRANSCEIVE) {
			Local_u8N = Read_Reg(FIFOLevelReg);
			*Copy_pu16BackLen = Local_u8N * 8;
			for (Local_u16I = 0; Local_u16I < Local_u8N; Local_u16I++) {
				Copy_pu8BackData[Local_u16I] = Read_Reg(FIFODataReg);
			}
		}
	}
	return Local_u8Status;
}

// --- High-Level Functions ---
void MFRC522_Init(void) {
	Write_Reg(CommandReg, PCD_RESETPHASE); // Soft Reset
	Write_Reg(ModeReg, 0x3D);
	
	// Turn Antenna ON
	u8 Local_u8Temp = Read_Reg(TxControlReg);
	if (!(Local_u8Temp & 0x03)) {
		Write_Reg(TxControlReg, Local_u8Temp | 0x03);
	}
}

u8 MFRC522_Request(u8 *Copy_pu8TagType) {
	u16 Local_u16BackBits;
	Write_Reg(BitFramingReg, 0x07);
	Copy_pu8TagType[0] = PICC_REQIDL;
	return ToCard(PCD_TRANSCEIVE, Copy_pu8TagType, 1, Copy_pu8TagType, &Local_u16BackBits);
}

u8 MFRC522_Anticoll(u8 *Copy_pu8SerNum) {
	u16 Local_u16UnLen;
	Write_Reg(BitFramingReg, 0x00);
	Copy_pu8SerNum[0] = PICC_ANTICOLL;
	Copy_pu8SerNum[1] = 0x20;
	return ToCard(PCD_TRANSCEIVE, Copy_pu8SerNum, 2, Copy_pu8SerNum, &Local_u16UnLen);
}
