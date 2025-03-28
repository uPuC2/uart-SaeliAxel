#include <avr/io.h>
#include <stdio.h>
#include "UART.h"
// Prototypes
// Initialization
#define frec 16000000


//UART 0 -> 12345 Baud, 8 bits, 1 parity, 2 stopbit
//UART 2 -> 115200 baud, 8 bit, 0 parity, 1 stopbit
//UART 3 -> 115200 baud 8 bit, 0 parity, 1 stopbit
void UART_Ini(uint8_t com, uint32_t baudrate, uint8_t size, uint8_t parity, uint8_t stop)
{
    uint8_t Velocidad = 0;
    uint32_t closesmatchNormal, ubrrNormal, closesmatchDoble, ubrrDoble, errorNormal, errorDoble;
	//--------------------------------------------------------------
    ubrrNormal = (frec / (16 * baudrate)) - 1;
    ubrrDoble = (frec / (8 * baudrate)) - 1;
	//--------------------------------------------------------------
	closesmatchNormal = frec / (16 * (ubrrNormal + 1));
    closesmatchDoble = frec / (8 * (ubrrNormal + 1));
	//--------------------------------------------------------------
    errorNormal = ((closesmatchNormal / ubrrNormal)-1) * 100;
    errorDoble = ((closesmatchDoble / ubrrDoble)-1) * 100;
	//--------------------------------------------------------------
    if (errorNormal> errorDoble)
        Velocidad = 2;

    if (parity == 1)
        parity = 3;

    switch (com)
    {
    case 0:
        UCSR0A = Velocidad;
        UCSR0B = (3<< TXEN0);
        UCSR0C = ((size - 5) << UCSZ00) | ((stop - 1) << USBS0) | (parity << UPM00);
        if (Velocidad==2)
            UBRR0 = ubrrDoble;
        else
            UBRR0 = ubrrNormal;
        break;
    case 1:
        UCSR1A = Velocidad;
        UCSR1B = (3<< TXEN0);
        UCSR1C = ((size - 5) << UCSZ10) | ((stop - 1) << USBS1) | (parity << UPM10);
        if (Velocidad==2)
            UBRR1 = ubrrDoble;
        else
            UBRR1 = ubrrNormal;
        break;
    case 2:
        UCSR2A = Velocidad;
        UCSR2B = (3<< TXEN0);
        UCSR2C = ((size - 5) << UCSZ20) | ((stop - 1) << USBS2) | (parity << UPM20);
        if (Velocidad==2)
            UBRR2 = ubrrDoble;
        else
            UBRR2 = ubrrNormal;
        break;
    case 3:
        UCSR3A = Velocidad;
        UCSR3B = (3<< TXEN0);
        UCSR3C = ((size - 5) << UCSZ30) | ((stop - 1) << USBS3) | (parity << UPM30);
        if (Velocidad==2)
            UBRR3 = ubrrDoble;
        else
            UBRR3 = ubrrNormal;
        break;
    }
}

// Send
void UART_puts(uint8_t com, char *str){
  while(*str){
    UART_putchar(com, *str);
    str++;
  }
}
void UART_putchar(uint8_t com, char data)
{
    switch (com)
    {
    case 0://UART 0 espera que el bit UDRE0 este en 1
        while (!(UCSR0A & (1 << UDRE0)))
            ;
        UDR0 = data; //Carga el dato para poderlo transmitir
        break;
    case 1:
        while (!(UCSR1A & (1 << UDRE1)))
            ;
        UDR1 = data;
        break;
    case 2:
        while (!(UCSR2A & (1 << UDRE2)))
            ;
        UDR2 = data;
        break;
    case 3:
        while (!(UCSR3A & (1 << UDRE3)))
            ;
        UDR3 = data;
        break;
    }
}

// Received
uint8_t UART_available(uint8_t com) {
    if(com == 0) {
        if(UCSR0A & (1 << RXC0)) {
            return 1;
        } else {
            return 0;
        }
    }
    else if(com == 2) {
        if(UCSR2A & (1 << RXC2)) {
            return 1;
        } else {
            return 0;
        }
    }
    else if(com == 3) {
        if(UCSR3A & (1 << RXC3)) {
            return 1;
        } else {
            return 0;
        }
    }
    else {
        return 0;
    }
}
char UART_getchar(uint8_t com ){

  switch(com){
    case 0:
      while(!(UCSR0A & (1 << RXC0)))
      ;
      return UDR0;
      break;
    case 1:
	  while(!(UCSR1A & (1<<RXC1)))
	  ;
	  return UDR1;
	  break;
	case 2:
	  while(!(UCSR2A & (1<<RXC2)))
	  ;
	  return UDR2;
	  break;
	case 3:
	  while(!(UCSR3A & (1<<RXC3)))
	  ;
	  return UDR3;
	  break;

	}
}
#define tamano 64

void UART_gets(uint8_t com, char *str)
{
    char BtnPressed;
    uint8_t index = 0;

    while(1) {
        BtnPressed = UART_getchar(com);

        if(BtnPressed == 13) { //13 en ascii es el enter
            str[index] = '\0';
            UART_putchar(com, '\r');
            UART_putchar(com, '\n');
            break;
        }
        else if (BtnPressed == 8 && index > 0) { 	//8 en ascii es para borrar o delete
            index--;
            UART_putchar(com, '\b');
            UART_putchar(com, ' ');	//Se deja de esta forma para que deje un espacio vacio en el lugar borrado
            UART_putchar(com, '\b');
        }
        else if (BtnPressed != 8) {
            UART_putchar(com, BtnPressed);
            str[index] = BtnPressed;
            index++;
        }

        if (index >= tamano - 1) {
            str[index] = '\0';
            break;
        }
    }
}

// Escape sequences
void UART_clrscr(uint8_t com)
{
    switch (com)
    {
    case 0:
        UART_puts(0, "\033[2J\033[H");
        break;
    case 1:
        UART_puts(1, "\033[2J\033[H");
        break;
    case 2:
        UART_puts(2, "\033[2J\033[H");
        break;
    case 3:
        UART_puts(3, "\033[2J\033[H");
        break;
    }
}

void UART_setColor(uint8_t com, uint8_t color)
{
	char secuencia[] = "\033[";	//Inicio de secuencia de escape
	char escape[]= "m";	//Final de secuencia de escape convierte "M" en valor mediante itoa
	char colorbase[5]; //Array para almacenar el valor de la cadena convertida
	
	itoa(color, colorbase, 10);	
	UART_puts(com,secuencia);
	UART_puts(com,colorbase);
	UART_puts(com, escape);
}

void UART_gotoxy(uint8_t com, uint8_t x, uint8_t y)
{
	char secEsc[] = "\033[";
	char separador[]= ";";
	char caracterFinal[] = "f";
	char coord_x[18];
	char coord_y[18];
	
	itoa(x, coord_x, 10);
	itoa(y, coord_y, 10); 	

	UART_puts(com,secEsc);
	UART_puts(com,coord_y);
	UART_puts(com,separador);
	UART_puts(com,coord_x);
	UART_puts(com,caracterFinal);
}


// Utils
void itoa(uint16_t number, char *str, uint8_t base)
{
    char *apunt = str; //Apuntador para recorrer la cadena saliente
    char *apunt_orig = apunt;	//Apuntador para guardar la posicion original
    uint16_t digit;	//Variable para almacenar el digito actual
	
	//Se verifica la base
    if (base < 2 || base > 16)
    {
        *str = '\0'; //Si la cadena es null se devuelve una cadena vacia
        return;
    }

    // ciclo para convertir el numero en la base necesaria
    while (number)
    {
        digit = number % base; //Se obtiene el digito menos significativo
        if (digit < 10)
            *apunt++ = digit + '0';//Se convierte el digito a ascii
        else
            *apunt++ = digit - 10 + 'A'; 
        number /= base; //Se reduce el numero
    }

    *apunt-- = '\0'; // Terminar la cadena

    // Ciclo para voltear la cadena para obtener el orden correcto
    while (apunt_orig < apunt)
    {
        char tmp = *apunt; //Mover o intercambiar la cadena de caracteres
        *apunt-- = *apunt_orig; //Se coloca el caracter en la otra posicion
        *apunt_orig++ = tmp;	//Se coloca el caracter original 
    }
}

uint16_t atoi(char *str)
{
	int result = 0;
	uint16_t idx = 0;
	while(str[idx] != '\0')
	{
		result = result * 10 + (str[idx]-'0');
		idx++;
	}
	return result;
}



