#include <Arduino.h>
#include <SPI.h>
#include <avr/io.h>

int main(void)
{

  // SPI_masterInit();

  // while(1)
  // {
  //   CLR_BIT(PORTB,PB2);
  
  //   SPI_transmit(1);

  //   SET_BIT(PORTB,PB2);
    

  // }

    // 1. تهيئة الـ SPI للـ Slave
    SPI_slaveInit();

    // 2. ضبط PC0 كـ Output للـ LED
    SET_BIT(DDRC, PC0);

    uint8_t received_data = 0;

    while (1) {
        // الانتظار لحين استقبال بيانات من الماستر
        received_data = SPI_receive();

        // لو البيانات المستلمة تساوي 1 ينور الليد
        if (received_data == 0x01) {
            SET_BIT(PORTC, PC0); // Turn ON LED
        } else {
            CLR_BIT(PORTC, PC0); // Turn OFF LED
        }
    }

}
