/************************************************************************/
/*                                                                      */
/*                      Debouncing 8 Keys                               */
/*                      Sampling 4 Times                                */
/*                                                                      */
/*              Author: Elegant Circuits                                */
/*              Credit: Peter Dannegger - Danni's Debounce              */
/*                                                                      */
/************************************************************************/

#include <avr/interrupt.h>
#include <avr/io.h>

// Two "active low" buttons connected between GND and  PC0, PC1
#define KEY_PIN     PINC
#define KEY_PORT    PORTC
#define KEY_DDR     DDRC
#define KEY0        0
#define KEY1        1

// Two LEDs connected between PB0, PB1 and GND 
#define LED_DDR     DDRB
#define LED_PORT    PORTB
#define LED0        0
#define LED1        1

unsigned char key_state;                // debounced and inverted key state:
                                        // bit = 1: key pressed
unsigned char key_press;                // key press detect

unsigned char ct0 = 0xFF, ct1 = 0xFF;   // internal debouncing states


ISR(TIMER0_OVF_vect ){
    unsigned char i;

    i = key_state ^ ~KEY_PIN;   // key changed ?
    ct0 = ~( ct0 & i );         // reset or count ct0
    ct1 = ct0 ^ (ct1 & i);      // reset or count ct1
    i &= ct0 & ct1;             // count until roll over ?
    key_state ^= i;             // then toggle debounced state
    key_press |= key_state & i; // 0->1: key press detect
}

unsigned char get_key_press( unsigned char key_mask ){
    cli();
    key_mask &= key_press;      // read key(s)
    key_press ^= key_mask;      // clear key(s)
    sei();
    return key_mask;
}

void debounce(void){
    if( get_key_press( 1<<KEY0 ))	// LED0 = toggle on keypress
      LED_PORT ^= 1<<LED0;

    if( get_key_press( 1<<KEY1 ))	// LED1 = toggle on keypress
      LED_PORT ^= 1<<LED1;
}

int main(void){

    TCCR0B = 1<<CS01;                      //divide by 256 * 256
    TIMSK0 = 1<<TOIE0;                     //enable timer interrupt

    KEY_DDR = 0;                // input
    KEY_PORT = 0xFF;            // pullups on
    LED_PORT = 0x00;            // LEDs off (low active)
    LED_DDR = 0xFF;             // LED output
    key_state = ~KEY_PIN;       // no action on keypress during reset
    sei();

    while(1){          // main loop
        debounce();
    }
}
