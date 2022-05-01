//
// T I T L E : lin_slave_example.c
//

//----- I N C L U D E S
#include "lin_avr.h"

//----- D E C L A R A T I O N S
#define LIN_ID_0  0x12
#define  LIN_ID_1  0x13
#define LEN_0 1
#define LEN_1 2

unsigned char lin_motor_contol[LEN_0];
unsigned char lin_node_status[LEN_1];

volatile unsigned char lin_slave_err_nb = 0;

//----- F U N C T I O N S

//. . . . . . . . . . . . . . . . . . . . . . . . .
//
// lin_id_task function of "lin_slave_example.c"
//
// The LIN ID received must be checked and compared to
//	the ones that the node must manage.
//
void lin_id_task (void) {
  switch (Lin_get_id()) {
    case LIN_ID_0:
      LIN.rx_response(LIN_2X, LEN_0);
      break;
    case LIN_ID_1:
      LIN.tx_response(LIN_2X, lin_node_status, LEN_1);
      break;
    default:
      // ID: absent/refused
      break;
  }
}

//. . . . . . . . . . . . . . . . . . . . . . . . .
//
// lin_rx_task function of "lin_slave_example.c"
//
//	- Save response data
//	- Update application signals.
//
void lin_rx_task (void) {
  unsigned char l_temp;
  LIN.get_response (lin_motor_contol);

  // Update of the application signals
  l_temp = lin_motor_contol[0] & 0x03;
  if ((l_temp == 0x01) || (l_temp == 0x03)) {
    PORTB |= 1 << PORT0;
  } else {
    PORTB &= ~(1 << PORT0);
    if (l_temp == 0x02) {
      PORTB |= 1 << PORT1;
    } else {
      PORTB &= ~(1 << PORT1);
    }
  }
}
//. . . . . . . . . . . . . . . . . . . . . . . . .
//
// lin_tx_task function of "lin_slave_example.c"
//
//	- Update buffer array with application signals
//	to be ready for the next transmission
//
void lin_tx_task (void) {

  // Update of the application signals
  lin_node_status[0] = PINB & 0x03; lin_node_status[1] = lin_slave_err_nb;
}

//. . . . . . . . . . . . . . . . . . . . . . . . .
//
// lin_err_task function of "lin_slave_example.c"
//
//	- If LIN error, increment the node error number
//
void lin_err_task (void) {

  // Global variable update
  lin_slave_err_nb++;
}

//. . . M A I N ( ) . . . . . . . . . . . . . . . .
//
// main function of "lin_slave_example.c"
//
//	In a 'no end' loop, do:
//	- if LIN_ID_0 (Rx response 1 Byte )
//	. PORT-B.0 = DC motor command -> clockwise
//	. PORT-B.1 = DC motor command -> counterclockwise
//	- if LIN_ID_1 (Tx response 2 bytes)
//	. Byte[0] = motor status
//	. Byte[1] = node error number
//
void setup() {
  // put your setup code here, to run once:
  // Port B setting for motor control
  DDRB |= 1 << PORTB0; DDRB |= 1 << PORTB1;
  PORTB &= ~(1 << PORTB0); PORTB &= ~(1 << PORTB1);

  // LIN Initialization
  LIN.init((unsigned char)LIN_2X, ((unsigned short)CONF_LINBRR),IS_SLAVE_NODE);
}

void loop() {
  // put your main code here, to run repeatedly:
  switch (Lin_get_it()) {
    case LIN_IDOK:
      lin_id_task();
      Lin_clear_idok_it();
      break;
    case LIN_RXOK:
      lin_rx_task();
      Lin_clear_rxok_it();
      break;
    case LIN_TXOK:
      lin_tx_task();
      Lin_clear_txok_it();
      break;
    case LIN_ERROR:
      lin_err_task();
      Lin_clear_err_it();
      break;
    default:
      break;
  }
}
