// Copyright (c) 2002-2010,  Microchip Technology Inc.
//
// Microchip licenses this software to you solely for use with Microchip
// products.  The software is owned by Microchip and its licensors, and
// is protected under applicable copyright laws.  All rights reserved.
//
// SOFTWARE IS PROVIDED "AS IS."  MICROCHIP EXPRESSLY DISCLAIMS ANY
// WARRANTY OF ANY KIND, WHETHER EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  IN NO EVENT SHALL
// MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
// CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, HARM TO YOUR
// EQUIPMENT, COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY
// OR SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED
// TO ANY DEFENSE THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION,
// OR OTHER SIMILAR COSTS.
//
// To the fullest extent allowed by law, Microchip and its licensors
// liability shall not exceed the amount of fees, if any, that you
// have paid directly to Microchip to use this software.
//
// MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE
// OF THESE TERMS.
#ifndef __UART_H__
#define __UART_H__

#define DEFAULT_BAUDRATE 			115200

#define Ux(y)         U2##y

#define UxBRG           Ux(BRG)
#define UxMODE          Ux(MODE)
#define UxSTA           Ux(STA)
#define UxSTAbits       Ux(STAbits)
#define UxSTACLR        Ux(STACLR)
#define UxSTASET        Ux(STASET)
#define UxTXREG         Ux(TXREG)
#define UxRXREG         Ux(RXREG)

#define TRANS_LAYER_Init UartInit
#define TRANS_LAYER_Task UartTask
#define TRANS_LAYER_Close UartClose

extern void TRANS_LAYER_Init(UINT);
extern void TRANS_LAYER_Task(void);
extern void UartClose(void);
#endif
