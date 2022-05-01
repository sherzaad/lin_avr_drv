//
// T I T L E : lin_master_example.c
//

//----- I N C L U D E S
#include "lin_avr.h"
//----- D E C L A R A T I O N S
#define	LIN_ID_0	0x12
#define	LIN_ID_1	0x13
#define	LEN_0	1
#define	LEN_1	2
#define	MAGIC_NUMBER	77

unsigned char lin_motor_command[LEN_0];
unsigned char lin_slave_node_status[LEN_1];

volatile unsigned char lin_master_err_nb = 0;
volatile unsigned char rtc_tics = 0;
//----- F U N C T I O N S

//. . . . . . . . . . . . . . . . . . . . . . . . .
//
// lin_id_task function of "lin_master_example.c"
//
// The LIN ID received must be checked and compared to
//	the ones that the node must manage.
//
void lin_id_task (void) {
  switch (Lin_get_id()) {
    case LIN_ID_0:
      LIN.tx_response(LIN_2X, lin_motor_command, LEN_0);
      break;
    case LIN_ID_1:
      LIN.rx_response(LIN_2X, LEN_1);
      break;
    default:
      // ID: absent/refused
      break;
  }
}

//. . . . . . . . . . . . . . . . . . . . . . . . .
//
// lin_rx_task function of "lin_master_example.c"
//
//	- Save response data
//	- Update application signals.
//
void lin_rx_task (void) {
  LIN.get_response (lin_slave_node_status);

  // If command OK and no error on slave node
  //	- PORTB[6] = 0
  //	- else PORTB[6] = 1
  if (lin_slave_node_status[0] != lin_motor_command[0]) {
    PORTB |= 1 << PORT6;
  } else if (lin_slave_node_status[1] != 0) {
    PORTB |= 1 << PORT6;
  } else {
    PORTB &= ~(1 << PORT6);
  }
}

//. . . . . . . . . . . . . . . . . . . . . . . . .
//
// lin_tx_task function of "lin_master_example.c"
//
//	- Update buffer array with application signals
//	to be ready for the next transmission
//
void lin_tx_task (void) {

  // Update of the application signals (active-low switches)
  lin_motor_command[0] = (~PINB) & 0x03;
}

//. . . . . . . . . . . . . . . . . . . . . . . . .
//
// lin_err_task function of "lin_master_example.c"
//
//	- If LIN error, increment the node error number
//
void lin_err_task (void) {

  // Global variable update
  lin_master_err_nb++;
}

//. . . . . . . . . . . . . . . . . . . . . . . . .
//
// OCR0A interrupt service routine of "lin_master_example.c"
//
//	AVR-GCC declaration (default):
//	ISR(TIMER0_COMPA_vect)
//	IAR declaration:
//	#pragma vector TIMER0_COMPA_vect
//	 interrupt void timer0_compa_vect(void)
//
//	The appropriate LIN header is sent following the rtc_tic value
//
ISR(TIMER0_COMPA_vect) {

  rtc_tics++;

  if ((rtc_tics & 0x01) == 0x01) {
    LIN.tx_header((unsigned char)LIN_2X, (unsigned char)LIN_ID_0, 0);
  }
  else {
    LIN.tx_header((unsigned char)LIN_2X, (unsigned char)LIN_ID_1, 0);
  }
}

//. . . M A I N ( ) . . . . . . . . . . . . . . . .
//
// main function of "lin_master_example.c"
//
//	In a 'no end' loop, do:
//	- if LIN_ID_0 (Tx response 1 Byte)
//	. PORT-B.0 = DC motor command -> clockwise
//	. PORT-B.1 = DC motor command -> counterclockwise
//	- if LIN_ID_1 (Rx response 2 bytes)
//	. Byte[0] = motor status
//	. Byte[1] = slave node error number
//	Timer0 ensures that each header is sent in 20ms intervals.
//
void setup() {
  // Port B setting for motor control
  // PORTB0 & PORTB1 switches active low with internal pull-up
  DDRB &= ~(1 << PORTB0); DDRB &= ~(1 << PORTB1); PORTB |= 1 << PORTB0; PORTB |= 1 << PORTB1;

  // Port B setting for motor status flag
  DDRB |= 1 << PORTB6; PORTB &= ~(1 << PORTB6);

  // LIN initialization
  LIN.init((unsigned char)LIN_2X, ((unsigned short)CONF_LINBRR),IS_MASTER_NODE);

  // Timer0 & INT initialization (no output)
  // Timer0 Reset
  TIMSK0 = 0; TCCR0B = 0; TCCR0A = 0; TCNT0 = 0; OCR0A = 0; TIFR0 = ((1 << OCF0A) | (1 << TOV0));
  // No output, mode 2, 10 ms interrupt
  TCCR0A = 0x02; OCR0A = MAGIC_NUMBER; TCCR0B = 0x05;
  // Timer0 compare A interrupt enable
  TIMSK0 |= 1 << OCIE0A; asm ("sei");
}

void loop() {
  switch (Lin_get_it()) {
    case LIN_IDOK:
      lin_id_task(); Lin_clear_idok_it(); 
      break;
    case LIN_RXOK:
      lin_rx_task(); Lin_clear_rxok_it(); 
      break;
    case LIN_TXOK:
      lin_tx_task(); Lin_clear_txok_it(); 
      break;
    case LIN_ERROR:
      lin_err_task(); Lin_clear_err_it(); 
      break;
    default:
      break;
  }
}
