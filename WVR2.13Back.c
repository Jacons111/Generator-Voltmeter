#include <avr/io.h>
#include <util/delay.h> // Declares _delay_ms
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Created by Jacob Constable 1/15/24
// Majority of functions were provided by Professor D. McLaughlin 
// This Project Turbine Voltmeter- DC Motor, Arduino Uno, Seven-Segment Display
// Arduino reads the voltage outputed from a DC motor connected to wind turbine blade. 
// Reads 0 - 5 Volts, Data updates every few seconds
// Data can be shown on Putty

#define PERSISTENCE 5 // Delay varaible
void uart_init(void);
void uart_send(unsigned char);
void send_string(char *stringAddress);
void adc_init(void);
unsigned int get_adc(void);

int main(void){
    unsigned char ledDigits[] = {0x7F, 0x0C, 0xB5, 0x9D, 0xCC, 0xD9, 0xF9, 0x0D, 0xFD, 0xDD}; // Hex to display numbers (0-9)
    unsigned int digitalValue, voltInt;
    unsigned char DIG0, DIG2, DIG3;  
    char buffer[6];
    adc_init();
    uart_init();
    DDRD = 0xFF; // 7 segment (a,b,c,d,e,f,g)
    DDRB = 0xFF; // 7 segment (dp)
    DDRC = 0b00111100; // Digit enable pins (Seven Segment)
    
            while(1){
            digitalValue = get_adc(); // digitalValue is the input from motor (some small number Ex. #<1)
            itoa (digitalValue, buffer, 10);
            send_string(buffer);
            voltInt = digitalValue*4.76; // converts motor input to voltage 
            int i = 0;
                while(i < 40){ // repeats 40 times (gives better display)
                                              // (EX. 4.85 V)
                    DIG2 = (voltInt/1000);   // (pulls out 4)
                    DIG3 = (voltInt/100)%10;// (pulls out 8)
                    DIG0 = (voltInt/10)%10;// (pulls out 5)
                    
                    // uart is the digital display of the arduino
                    uart_send(DIG2+ '0'); 
                    uart_send('.');
                    uart_send(DIG3+ '0');
                    uart_send(DIG0+ '0');
    
                    PORTD = ledDigits[DIG2]; // read uart, match to listed hex 
                    uart_send(DIG2+'0');
                    PORTC=~(1<<5); // Set digit 1 to high, display matched number on seventh segment 
                    _delay_ms(PERSISTENCE/2);

                    DIG2 = 0x01; // Set digit 2 to display the decimal
          
                    PORTB = DIG2; 
                    uart_send(DIG2+'0');
                    PORTC=~(1<<5); // Set digit 2 to high 
                    _delay_ms(PERSISTENCE/2);
                    PORTB = 0x00;

                    PORTD = ledDigits[DIG3];
                    uart_send(DIG3+'0');
                    PORTC=~(1<<4); // Set digit 2 to high 
                    _delay_ms(PERSISTENCE);
            
                    PORTD = ledDigits[DIG0];
                    uart_send(DIG0+'0');
                    PORTC=~(1<<3); // Set digit 3 to high 
                    _delay_ms(PERSISTENCE/2); 

                    PORTC= 0b00111100;
                    uart_send(13); //tx carriage return 
                    uart_send(10); //tx line feed
                    i++;
                } 
            
            }
            return 0;
    }

// Initialize ADC peripheral:  
void adc_init(void){
    ADMUX = 0x40; // External Reference A0
    ADCSRA = 0x87; // Vref=AVCC=5V; 
}

// Read ADC value
unsigned int get_adc(){
    ADCSRA |= (1<<ADSC);
    while ((ADCSRA & (1<<ADIF))==0);
    return ADCL | (ADCH << 8);
}
// initialize ATmega328P UART: enable TX, 8 bit, 1 stop bit, // no parity, asych, 9680 Baud (16 MHz clock)
void uart_init(void) {
    UCSR0B = (1 << TXEN0); //enable the UART transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //set 8 bit character size
    UBRR0L = 103; //set baud rate to 9600 for 16 MHz crystal
}
// send string of ASCII characters
void send_string (char *stringAddress) {
    unsigned char i;
    for (i = 0; i < strlen(stringAddress); i++)
        uart_send(stringAddress[i]);
}
void uart_send(unsigned char ch){
    while (!(UCSR0A & (1 << UDRE0))); //wait til tx data buffer empty
    UDR0 = ch; //write the character to the USART data register
}




