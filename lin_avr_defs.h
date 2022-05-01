/*
  LIN_defs.h- Library for Arduino LIN shield based on Hardware Serial Library for Arduino
  ver 1.0 created by Sherzaad Dinah
  Revision History
  ver1.0 - Newly created
*/

#ifndef	LIN_AVR_DRV_DEFS_H
#define	LIN_AVR_DRV_DEFS_H
#include "lin_config.h"

//----- include appropirate board library in case missing -----//
// #if defined (__AVR_ATmega64M1__)
// #include "iom64m1.h"
// #elif defined (__AVR_ATmega64C1__)
// #include "iom64c1.h"
// #elif defined (__AVR_ATmega32M1__)
// #include "iom32m1.h"
// #elif defined (__AVR_ATmega32C1__)
// #include "iom32c1.h"
// #elif defined (__AVR_ATtiny167__)
// #include "iotn167.h"
// #endif
//------------------------------------------------------------//

// LIN Configuration
#ifndef FOSC
#error	You must define FOSC
#elif	FOSC == 16000	// if device frequency = 16 MHz
#ifndef LIN_BAUDRATE
#error	You must define LIN_BAUDRATE
#elif	LIN_BAUDRATE == 19200
#define CONF_LINBRR	25	// (0x19) 19.2 kbps, error = 0.2%
#elif	LIN_BAUDRATE == 9600
#define CONF_LINBRR	51	// (0x33) 9.6 kbps, error= 0.2%
#elif	LIN_BAUDRATE == 4800
#define CONF_LINBRR	103	// (0x67) 4.8 kbps, error= 0.2%
#elif	LIN_BAUDRATE == 2400
#define CONF_LINBRR	207	// (0xCF) 2.4 kbps, error=-0.2%
#else
#error	Not available LIN_BAUDRATE value
#endif
#elif	FOSC == 8000	// if device frequency = 8 MHz
#ifndef LIN_BAUDRATE
#error	You must define LIN_BAUDRATE
#elif	LIN_BAUDRATE == 19200
#define CONF_LINBRR	12	// (0x0C) 19.2 kbps, error = 0.2%
//CONF_LINBRR = [round {(FOSC x 1000) / (32 x LIN_BAUDRATE)}]-1
#elif	LIN_BAUDRATE == 9600
#define CONF_LINBRR	25	// (0x19) 9.6 kbps, error= 0.2%
#elif	LIN_BAUDRATE == 4800
#define CONF_LINBRR	51	// (0x33) 4.8 kbps, error= 0.2%
#elif	LIN_BAUDRATE == 2400
#define CONF_LINBRR	103	// (0x67) 2.4 kbps, error=-0.2%
#else
#error	Not available LIN_BAUDRATE value 
#endif
#else
#error	Not available FOSC value 
#endif

// LIN protocols
#define LIN_1X	(1<<LIN13)
#define LIN_2X	(0<<LIN13)
#define IS_MASTER_NODE 1
#define IS_SLAVE_NODE 0

// LIN commands
#define LIN_CMD_MASK ((1<<LCMD1)|(1<<LCMD0))
#define LIN_RX_HEADER ((0<<LCMD1)|(0<<LCMD0))
#define LIN_ABORT ((0<<LCMD1)|(0<<LCMD0))
#define LIN_TX_HEADER ((0<<LCMD1)|(1<<LCMD0))
#define LIN_RX_RESPONSE	((1<<LCMD1)|(0<<LCMD0))
#define LIN_TX_RESPONSE	((1<<LCMD1)|(1<<LCMD0))
// LIN interrupt flags
#define LIN_ERROR	(1<<LERR )
#define LIN_IDOK	(1<<LIDOK)
#define LIN_RXOK	(1<<LRXOK)
#define LIN_TXOK	(1<<LTXOK)
// LIN ID masks
#define LIN_1X_ID_MASK ((1<<LID3)|(1<<LID2)|(1<<LID1)|(1<<LID0))
#define LIN_1X_LEN_MASK	((1<<LID5)|(1<<LID4))
#define LIN_2X_ID_MASK ((1<<LID5)|(1<<LID4)| LIN_1X_ID_MASK )

#if defined (__AVR_ATtiny167__)
// Specific to ATtiny167
#define LIN_PORT_IN PINA
#define LIN_PORT_DIR DDRA
#define LIN_PORT_OUT PORTA
#define LIN_INPUT_PIN 0
#define LIN_OUTPUT_PIN 1
#elif defined (__AVR_ATmega64M1__) || defined(__AVR_ATmega64C1__) || defined (__AVR_ATmega32M1__) || defined (__AVR_ATmega32C1__)
// Specific to ATmega32/64/M1/C1
#define LIN_PORT_IN PIND
#define LIN_PORT_DIR DDRD
#define LIN_PORT_OUT PORTD
#define LIN_INPUT_PIN 4
#define LIN_OUTPUT_PIN 3
#else
#error	Board is not supported by lin_avr library
#endif

// Use LIN 1.x protocol
#define Lin_1x_set_type()	( LINCR|= LIN_1X )
#define Lin_1x_enable()	( LINCR = LIN_1X|(1<<LENA)|(0<<LCMD2) )
#define Lin_1x_set_id(id)	{ LINIDR &= ~LIN_1X_ID_MASK; LINIDR |= id & LIN_1X_ID_MASK;}
#define Lin_1x_set_len(len) { LINIDR &= ~LIN_1X_LEN_MASK; LINIDR |= (((len+4)<<2) & LIN_1X_LEN_MASK);}
#define Lin_get_len()	( LINDLR & (0x0F << LRXDL0) )
// Lin_set_rx_len(len) - Automatic setting in LIN 1.x for response
// Lin_set_tx_len(len) - Automatic setting in LIN 1.x for response

// Use LIN 2.x protocol
#define Lin_2x_set_type()	( LINCR|= LIN_2X )
#define Lin_2x_enable()	( LINCR = LIN_2X|(1<<LENA)|(0<<LCMD2) )
#define Lin_2x_set_id(id)	{ LINIDR &= ~LIN_2X_ID_MASK; LINIDR |= id & LIN_2X_ID_MASK;}
// Lin_2x_set_len(len) - Not available in LIN 2.1
// Lin_get_len() ------- Not available in LIN 2.1
#define Lin_set_rx_len(len) ( LINDLR = len & (0x0F << LRXDL0) )
#define Lin_set_tx_len(len) ( LINDLR = len << LTXDL0 )

// Use of any protocol
#define Lin_set_type(ltype)(( ltype == LIN_2X ) ? Lin_2x_set_type(): Lin_1x_set_type() )
#define Lin_get_type()	( LINCR & (1<<LIN1X) )
#define Lin_set_len(len)	( LINDLR = (len<<LTXDL0)|(len &(0x0F<<LRXDL0) )
// Miscellaneous C-macros
#define Lin_get_error_status()( LINERR )
#define Lin_set_baudrate(br) { LINBRRH = (unsigned char)(((unsigned short)br)>>8); LINBRRL = (unsigned char) ((unsigned short)br);	}
#define Lin_sw_reset()	( LINCR = 1<<LSWRES )
#define Lin_full_reset()	{ Lin_sw_reset(); Lin_clear_enable_it(); \
    LINBRRL = 0x00; LINBRRH = 0x00;	}
// Interrupt handling
#define Lin_get_it() ( LINSIR & ((1<<LERR)|(1<<LIDOK)|(1<<LTXOK)|(1<<LRXOK)) )
#define Lin_set_enable_it()	( LINENIR = (1<<LENERR)|(1<<LENIDOK)|(1<<LENTXOK)|(1<<LENRXOK) )
#define Lin_clear_enable_it()	( LINENIR = (0<<LENERR)|(0<<LENIDOK)|(0<<LENTXOK)|(0<<LENRXOK) )
#define Lin_clear_err_it() ( LINSIR = 1<<LERR )
#define Lin_clear_idok_it() ( LINSIR = 1<<LIDOK )
#define Lin_clear_txok_it() ( LINSIR = 1<<LTXOK )
#define Lin_clear_rxok_it() ( LINSIR = 1<<LRXOK )
// LIN commands
#define Lin_abort()	(LINCR &= ~LIN_CMD_MASK)
#define Lin_rx_header()(LINCR &= ~LIN_CMD_MASK)
#define Lin_tx_header(){LINCR &= ~LIN_CMD_MASK; LINCR|= LIN_TX_HEADER	;}
#define Lin_rx_response(){LINCR &= ~LIN_CMD_MASK; LINCR|= LIN_RX_RESPONSE;}
#define Lin_tx_response(){LINCR &= ~LIN_CMD_MASK; LINCR|= LIN_TX_RESPONSE;}
// LIN checking
#define Is_lin_header_ready() ( (LINSIR & (1<<LIDOK)) ? 1 : 0 )
#define Is_lin_rx_response_ready()( (LINSIR & (1<<LRXOK)) ? 1 : 0 )
#define Is_lin_tx_response_ready()( (LINSIR & (1<<LTXOK)) ? 1 : 0 )
// ID & data handling
#define Lin_get_id()	( LINIDR & LIN_2X_ID_MASK)
#define Lin_clear_index() ( LINSEL = (0<<LAINC)|(0x00<<LINDX0) )
#define Lin_get_data() ( LINDAT )
#define Lin_set_data(data) ( LINDAT = data )

#endif