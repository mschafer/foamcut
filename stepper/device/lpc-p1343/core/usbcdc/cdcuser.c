/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    cdcuser.c
 *      Purpose: USB Communication Device Class User module 
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on NXP Semiconductors LPC microcontroller devices only. Nothing else 
 *      gives you the right to use this software.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include "projectconfig.h"

#include "usb.h"
#include "usbhw.h"
#include "usbcfg.h"
#include "usbcore.h"
#include "cdc.h"
#include "cdcuser.h"

CDC_LINE_CODING CDC_LineCoding = { CFG_USBCDC_BAUDRATE, 0, 0, 8 };
unsigned short CDC_DepInEmpty = 1; // Data IN EP is empty

#define CDC_BLOCK_COUNT 4
#define USB_PACKET_SIZE 64
typedef struct CDCBlock {
    struct CDCBlock *next;
    uint8_t data[USB_PACKET_SIZE];
    uint8_t count;
    uint8_t position;
} CDCBlock_t;

CDCBlock_t CDC_Pool[CDC_BLOCK_COUNT];

CDCBlock_t *CDC_FreeBlocks;
CDCBlock_t *CDC_FullBlocks;

CDCBlock_t *CDC_AllocBlock()
{
    CDCBlock_t *ret = NULL;
    if (CDC_FreeBlocks != NULL) {
        // remove a block from the free list
        ret = CDC_FreeBlocks;
        CDC_FreeBlocks = CDC_FreeBlocks->next;
        ret->next = NULL;
    }
    return ret;
}

void CDC_AppendFullBlock(CDCBlock_t *b) {
    CDCBlock_t *tail = CDC_FullBlocks;
    if (tail != NULL) {
        while (tail->next != NULL) tail = tail->next;
        tail->next = b;
    } else {
        CDC_FullBlocks = b;
    }
    b->next = NULL;
}

void CDC_FreeBlock(CDCBlock_t *b)
{
    b->next = CDC_FreeBlocks;
    b->count = 0;
    b->position = 0;
    CDC_FreeBlocks = b;
}

uint8_t CDC_ReadByte(uint8_t *rxByte)
{
    uint8_t ret = 0;
    CDCBlock_t *b = CDC_FullBlocks;
    if (b !=NULL) {
    	ret = 1;
    	*rxByte = b->data[b->position];
        b->position++;
        if (b->position == b->count) {
            CDC_FullBlocks = b->next;
            CDC_FreeBlock(b);
        }
    }
    return ret;
}

/*----------------------------------------------------------------------------
 CDC Initialisation
 Initializes the data structures and serial port
 Parameters:   None 
 Return Value: None
 *---------------------------------------------------------------------------*/
void CDC_Init(void) {

    //  ser_OpenPort ();
    //  ser_InitPort (CDC_LineCoding.dwDTERate,
    //                CDC_LineCoding.bDataBits,
    //                CDC_LineCoding.bParityType,
    //                CDC_LineCoding.bCharFormat);

    int i;
    CDC_DepInEmpty = 1;

    CDC_FullBlocks = NULL;
    CDC_FreeBlocks = NULL;
    for (i=0; i<CDC_BLOCK_COUNT; i++) {
        CDC_FreeBlock(&CDC_Pool[i]);
    }
}

/*----------------------------------------------------------------------------
 CDC SendEncapsulatedCommand Request Callback
 Called automatically on CDC SEND_ENCAPSULATED_COMMAND Request
 Parameters:   None                          (global SetupPacket and EP0Buf)
 Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_SendEncapsulatedCommand(void) {

    return (TRUE);
}

/*----------------------------------------------------------------------------
 CDC GetEncapsulatedResponse Request Callback
 Called automatically on CDC Get_ENCAPSULATED_RESPONSE Request
 Parameters:   None                          (global SetupPacket and EP0Buf)
 Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_GetEncapsulatedResponse(void) {

    /* ... add code to handle request */
    return (TRUE);
}

/*----------------------------------------------------------------------------
 CDC SetCommFeature Request Callback
 Called automatically on CDC Set_COMM_FATURE Request
 Parameters:   FeatureSelector
 Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_SetCommFeature(unsigned short wFeatureSelector) {

    /* ... add code to handle request */
    return (TRUE);
}

/*----------------------------------------------------------------------------
 CDC GetCommFeature Request Callback
 Called automatically on CDC Get_COMM_FATURE Request
 Parameters:   FeatureSelector
 Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_GetCommFeature(unsigned short wFeatureSelector) {

    /* ... add code to handle request */
    return (TRUE);
}

/*----------------------------------------------------------------------------
 CDC ClearCommFeature Request Callback
 Called automatically on CDC CLEAR_COMM_FATURE Request
 Parameters:   FeatureSelector
 Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_ClearCommFeature(unsigned short wFeatureSelector) {

    /* ... add code to handle request */
    return (TRUE);
}

/*----------------------------------------------------------------------------
 CDC SetLineCoding Request Callback
 Called automatically on CDC SET_LINE_CODING Request
 Parameters:   none                    (global SetupPacket and EP0Buf)
 Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_SetLineCoding(void) {

    CDC_LineCoding.dwDTERate = (EP0Buf[0] << 0) | (EP0Buf[1] << 8)
            | (EP0Buf[2] << 16) | (EP0Buf[3] << 24);
    CDC_LineCoding.bCharFormat = EP0Buf[4];
    CDC_LineCoding.bParityType = EP0Buf[5];
    CDC_LineCoding.bDataBits = EP0Buf[6];

    //  ser_ClosePort();
    //  ser_OpenPort ();
    //  ser_InitPort (CDC_LineCoding.dwDTERate,
    //                CDC_LineCoding.bDataBits,
    //                CDC_LineCoding.bParityType,
    //                CDC_LineCoding.bCharFormat);
    return (TRUE);
}

/*----------------------------------------------------------------------------
 CDC GetLineCoding Request Callback
 Called automatically on CDC GET_LINE_CODING Request
 Parameters:   None                         (global SetupPacket and EP0Buf)
 Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_GetLineCoding(void) {

    EP0Buf[0] = (CDC_LineCoding.dwDTERate >> 0) & 0xFF;
    EP0Buf[1] = (CDC_LineCoding.dwDTERate >> 8) & 0xFF;
    EP0Buf[2] = (CDC_LineCoding.dwDTERate >> 16) & 0xFF;
    EP0Buf[3] = (CDC_LineCoding.dwDTERate >> 24) & 0xFF;
    EP0Buf[4] = CDC_LineCoding.bCharFormat;
    EP0Buf[5] = CDC_LineCoding.bParityType;
    EP0Buf[6] = CDC_LineCoding.bDataBits;

    return (TRUE);
}

/*----------------------------------------------------------------------------
 CDC SetControlLineState Request Callback
 Called automatically on CDC SET_CONTROL_LINE_STATE Request
 Parameters:   ControlSignalBitmap 
 Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_SetControlLineState(unsigned short wControlSignalBitmap) {

    /* ... add code to handle request */
    return (TRUE);
}

/*----------------------------------------------------------------------------
 CDC SendBreak Request Callback
 Called automatically on CDC Set_COMM_FATURE Request
 Parameters:   0xFFFF  start of Break 
 0x0000  stop  of Break
 0x####  Duration of Break
 Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_SendBreak(unsigned short wDurationOfBreak) {

    /* ... add code to handle request */
    return (TRUE);
}

/**
 * Called by ISR
 */
void CDC_BulkIn(void) {
}

/**
 * Called by ISR.
 */
void CDC_BulkOut(void) {
  // get data from USB into intermediate buffer
    uint32_t s = USB_EPBufState(CDC_DEP_OUT);
    //uint32_t numBytesRead = USB_ReadEP(CDC_DEP_OUT, &BulkBufOut[0]);
}

/*----------------------------------------------------------------------------
 Get the SERIAL_STATE as defined in usbcdc11.pdf, 6.3.5, Table 69.
 Parameters:   none
 Return Value: SerialState as defined in usbcdc11.pdf
 *---------------------------------------------------------------------------*/
unsigned short CDC_GetSerialState(void) {
    //  unsigned short temp;

    //  ser_LineState (&temp);
    //
    //  if (temp & 0x8000)  CDC_SerialState |= CDC_SERIAL_STATE_RX_CARRIER;
    //  if (temp & 0x2000)  CDC_SerialState |= CDC_SERIAL_STATE_TX_CARRIER;
    //  if (temp & 0x0010)  CDC_SerialState |= CDC_SERIAL_STATE_BREAK;
    //  if (temp & 0x4000)  CDC_SerialState |= CDC_SERIAL_STATE_RING;
    //  if (temp & 0x0008)  CDC_SerialState |= CDC_SERIAL_STATE_FRAMING;
    //  if (temp & 0x0004)  CDC_SerialState |= CDC_SERIAL_STATE_PARITY;
    //  if (temp & 0x0002)  CDC_SerialState |= CDC_SERIAL_STATE_OVERRUN;

    return (0);
}

/**
 * Send the SERIAL_STATE notification as defined in usbcdc11.pdf, 6.3.5.
 * Called from the USB IRQ
 */
void CDC_NotificationIn(void) {
    static const uint8_t NotificationBuf[] = { 0xA1, // bmRequestType
            CDC_NOTIFICATION_SERIAL_STATE, // bNotification (SERIAL_STATE)
            0x00, // wValue
            0x00, 0x00, // wIndex (Interface #, LSB first)
            0x00, 0x02, // wLength (Data length = 2 bytes, LSB first)
            0x00, 0x00, // UART State Bitmap (16bits, LSB first)
            0x00 };
    USB_WriteEP(CDC_CEP_IN, NotificationBuf, 10); // send notification
}

uint32_t CDC_GetOutEpBuff(uint8_t *buff)
{
    uint32_t s = USB_EPBufState(CDC_DEP_OUT);
    uint32_t count = 0;
    // check the full/empty bit for data waiting in EP
    if ((s & 0x01) != 0) {
        count = USB_ReadEP(CDC_DEP_OUT, buff);
    }
    return count;
}

void CDC_PollOutEp()
{
    uint32_t s = USB_EPBufState(CDC_DEP_OUT);
    if ((s & 0x01) != 0) {
        CDCBlock_t *b = CDC_AllocBlock();
        if (b != NULL) {
            b->count = USB_ReadEP(CDC_DEP_OUT, b->data);
            if (b->count > 0) {
                CDC_AppendFullBlock(b);
            } else {
                CDC_FreeBlock(b);
            }
        }
    }
}

uint32_t CDC_WriteInEp(const uint8_t *data, uint32_t length) {
    uint32_t s = USB_EPBufState(CDC_DEP_IN);
    uint32_t cnt = 0;
    // check the full/empty bit for an empty buffer
    while ((s & 0x01) == 0 && cnt < length) {
        uint32_t n = USB_WriteEP(CDC_DEP_IN, data, length);
        cnt += n;
        data += n;
        s = USB_EPBufState(CDC_DEP_IN);
    }
    return cnt;
}

void CDC_BlockingWriteInEp(const uint8_t *data, uint32_t length) {
    uint32_t cnt = 0;
    // check the full/empty bit for an empty buffer
    while (cnt < length) {
        uint32_t s = USB_EPBufState(CDC_DEP_IN);
        if ((s & 0x01) == 0) {
            uint32_t n = USB_WriteEP(CDC_DEP_IN, data, length);
            cnt += n;
            data += n;
        }
    }
}
