#include <BIT_MATH.h>
#include <SPI.h>
#include <avr/io.h>



void SPI_masterInit()
{
    
    // Direction (input or output)
    SET_BIT(DDRB , PB3); // MOSI = output
     SET_BIT(DDRB , PB5);  // SCK = output
      SET_BIT(DDRB , PB2);  // SS = output
      CLR_BIT(DDRB , PB4); // MISO = input

      
      SET_BIT(PORTB , PB2); //  SS = High 
      CLR_BIT(PORTB ,PB3 );  // MOSI = LOW
      CLR_BIT(PORTB ,PB4 );   // MISO = LOW
       CLR_BIT(PORTB ,PB5 );  // SCK = LOW


       CLR_BIT(SPCR , SPIE);   // interrupt
       SET_BIT(SPCR , SPE);   // SPI Enable
        SET_BIT(SPCR , DORD);  // Data order (LSB)
        SET_BIT(SPCR , MSTR); // Master 
        SET_BIT(SPCR , CPOL); // CPOL 
        SET_BIT(SPCR , CPHA); // CPHA
        SET_BIT(SPCR , SPR1); // Freq
         SET_BIT(SPCR , SPI2X); // Freq
        CLR_BIT(SPSR , SPR0);  // Freq
       

       
      

}


void SPI_slaveInit()
{
     // Direction (input or output)
    CLR_BIT(DDRB , PB3); // MOSI = INPUT
     CLR_BIT(DDRB , PB5);  // SCK = INPUT
      CLR_BIT(DDRB , PB2);  // SS = INPUT
      SET_BIT(DDRB , PB4); // MISO = OUTPUT

    


       CLR_BIT(SPCR , SPIE);   // interrupt
       SET_BIT(SPCR , SPE);   // SPI Enable
        SET_BIT(SPCR , DORD);  // Data order (LSB)
        CLR_BIT(SPCR , MSTR); // Master 
        SET_BIT(SPCR , CPOL); // CPOL 
        SET_BIT(SPCR , CPHA); // CPHA
        

}




uint8_t SPI_transmit(uint8_t data)
{
    SPDR = data;
    
    return SPI_receive();
}


uint8_t SPI_receive(){

    while (GET_BIT(SPSR , SPIF) == 0);

    return SPDR;
}
