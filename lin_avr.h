/*
  LIN.h- Library for Arduino LIN shield based on Hardware Serial Library for Arduino
  ver 1.0 created by Sherzaad Dinah
  Revision History
  ver1.0 - Newly created
  ver1.1 - Moved 'SendBreak' routine to file to fix sending serial break issue
  ver1.2 - converted LIN.h functions into class.
*/
//******************************************************************************
//! @file $Id: lin_drv.h 581 2008-02-25 10:22:47Z jtellier $
//!
//! Copyright (c) 2008 Atmel.
//!
//! Use of this program is subject to Atmel's End User License Agreement.
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains the prototypes and the macros of the
//!        low level functions (drivers) of:
//!             - LIN of LIN/UART Controller
//!             - for ATmega32/64/M1/C1
//!             - for ATtiny167.
//!
//! This file was validated with AVRStudio-413571/WinAVR-20071221.
//!
//******************************************************************************

#ifndef	LIN_AVR_H
#define	LIN_AVR_H

#include "lin_avr_defs.h"

class LINClass {
  public:
    LINClass() {};
    unsigned char init (unsigned char l_type, unsigned long b_rate, unsigned long l_mode);
    unsigned char tx_header (unsigned char l_type, unsigned char l_id, unsigned char l_len);
    unsigned char rx_response (unsigned char l_type, unsigned char l_len);
    unsigned char tx_response (unsigned char l_type, unsigned char *l_data, unsigned char l_len);
    void get_response (unsigned char *l_data);
  private:
};
extern LINClass LIN;
#endif
