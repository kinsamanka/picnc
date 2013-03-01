/*********************************************************************
 *
 *                  PIC32 Boot Loader
 *
 *********************************************************************
 * FileName:        Uart.c
 * Dependencies:
 * Processor:       PIC32
 *
 * Compiler:        MPLAB C32
 *                  MPLAB IDE
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the �Company�) for its PIC32 Microcontroller is intended
 * and supplied to you, the Company�s customer, for use solely and
 * exclusively on Microchip PIC32 Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN �AS IS� CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *
 * $Id:  $
 * $Name: $
 *
 **********************************************************************/
#include <p32xxxx.h>
#include <stdlib.h>
#include <plib.h>
#include "Uart.h"
#include "HardwareProfile.h"
#include "BootLoader.h"
#include "Framework.h"


static UINT8 TxBuff[255];
BOOL GetChar(UINT8 *byte);
void PutChar(UINT8 txChar);


/********************************************************************
* Function: 	UartInit()
*
* Precondition: 
*
* Input: 		PB Clock
*
* Output:		None.
*
* Side Effects:	None.
*
* Overview:     Initializes UART.
*
*			
* Note:		 	None.
********************************************************************/	
void UartInit(UINT pbClk)
{
	// Open UART2 with Receive and Transmitter enable.
    UxBRG = (pbClk/16/DEFAULT_BAUDRATE-1); // calculate actual BAUD generate value.
    UxMODE = UART_EN;
    UxSTA = (UART_RX_ENABLE | UART_TX_ENABLE);	
}	


/********************************************************************
* Function: 	UartClose()
*
* Precondition: 
*
* Input: 		None
*
* Output:		None.
*
* Side Effects:	None.
*
* Overview:     Closes UART connection.
*
*			
* Note:		 	None.
********************************************************************/	
void UartClose(void)
{
	//TODO: do some closing operation if required.	
}	


/********************************************************************
* Function: 	UartTasks()
*
* Precondition: 
*
* Input: 		None
*
* Output:		None
*
* Side Effects:	None.
*
* Overview:     Runs some UART routines like receive bytes and transmit bytes.
*
*			
* Note:		 	None.
********************************************************************/
void UartTask(void)
{
	UINT8 TxLen;
	UINT8 Rx;
	UINT8 *ptr;
	// Check any character is received.
	if(GetChar(&Rx))
	{
		// Pass the bytes to frame work.
		FRAMEWORK_BuildRxFrame(&Rx, 1);
	}	
	
	ptr = TxBuff;
	// Get transmit frame from frame work.
	TxLen = FRAMEWORK_GetTransmitFrame(ptr);
	
	if(TxLen)
	{
		// There is something to transmit.
	   	while(TxLen--)
	   	{
		   PutChar(*(ptr++));
		} 
	} 
	
	
}


/********************************************************************
* Function: 	GetChar()
*
* Precondition: 
*
* Input: 		None
*
* Output:		True: If there is some data.
*
* Side Effects:	None.
*
* Overview:     Gets the data from UART RX FIFO.
*
*			
* Note:		 	None.
********************************************************************/
BOOL GetChar(UINT8 *byte)
{
	BYTE dummy;

	if(UxSTA & 0x000E)              // receive errors?
	{
		dummy = UxRXREG; 			// dummy read to clear FERR/PERR
		UxSTAbits.OERR = 0;			// clear OERR to keep receiving
	}

	if(UxSTAbits.URXDA)
	{
		*byte = UxRXREG;		        // get data from UART RX FIFO
		return TRUE;
	}
	
	return FALSE;

}


/********************************************************************
* Function: 	PutChar()
*
* Precondition: 
*
* Input: 		None
*
* Output:		None
*
* Side Effects:	None.
*
* Overview:     Puts the data into UART tx reg for transmission.
*
*			
* Note:		 	None.
********************************************************************/
void PutChar(UINT8 txChar)
{
    while(UxSTAbits.UTXBF); // wait for TX buffer to be empty
    UxTXREG = txChar;
}

/***************************************End Of File*************************************/
