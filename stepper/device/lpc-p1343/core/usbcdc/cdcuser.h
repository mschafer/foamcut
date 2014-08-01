/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    cdcuser.h
 *      Purpose: USB Communication Device Class User module Definitions
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

#ifndef __CDCUSER_H__
#define __CDCUSER_H__

#include <stdint.h>

/* CDC buffer handling */
extern int CDC_RdOutBuf(char *buffer, const int *length);
extern int CDC_WrOutBuf(const char *buffer, int *length);
extern int CDC_OutBufAvailChar(int *availChar);

/* CDC Data In/Out Endpoint Address */
#define CDC_DEP_IN       0x83
#define CDC_DEP_OUT      0x03

/* CDC Communication In Endpoint Address */
#define CDC_CEP_IN       0x81

/* CDC Requests Callback Functions */
extern uint32_t CDC_SendEncapsulatedCommand(void);
extern uint32_t CDC_GetEncapsulatedResponse(void);
extern uint32_t CDC_SetCommFeature(unsigned short wFeatureSelector);
extern uint32_t CDC_GetCommFeature(unsigned short wFeatureSelector);
extern uint32_t CDC_ClearCommFeature(unsigned short wFeatureSelector);
extern uint32_t CDC_GetLineCoding(void);
extern uint32_t CDC_SetLineCoding(void);
extern uint32_t CDC_SetControlLineState(unsigned short wControlSignalBitmap);
extern uint32_t CDC_SendBreak(unsigned short wDurationOfBreak);

/* CDC Bulk Callback Functions */
extern void CDC_BulkIn(void);
extern void CDC_BulkOut(void);

/* CDC Notification Callback Function */
extern void CDC_NotificationIn(void);

/* CDC Initialization Function */
extern void CDC_Init(void);

/* CDC prepare the SERAIAL_STATE */
extern unsigned short CDC_GetSerialState(void);

/* flow control */
extern unsigned short CDC_DepInEmpty; // DataEndPoint IN empty

/**
 * Transfer data from the Out endpoint RAM to the out buffer.
 * \param buffer Must be at least 64 bytes long.
 * \return The number of bytes read from the endpoint, >= 0 && <= 64.
 */
uint32_t CDC_GetOutEpBuff(uint8_t *buff);

/**
 * Poll the out endpoint and transfer data into buffer pool if any is available.
 * Data may be read one byte at a time using \sa CDC_ReadByte.
 */
void CDC_PollOutEp(void);

/**
 * Reads a single byte from the CDC buffer and puts it into rxByte.
 * \param rxByte A pointer to the address that will hold the byte that is read.
 * \return 0 on success, -1 if no byte is available.
 */
uint8_t CDC_ReadByte(uint8_t *rxByte);

/**
 * Write data to the In endpoint.
 * Does not block.  Sends as many bytes as possible and returns.
 * \return Number of bytes written.
 */
uint32_t CDC_WriteInEp(const uint8_t *data, uint32_t length);

/**
 * Write data to the In endpoint.
 * Blocks until all bytes have been sent.
 */
void CDC_BlockingWriteInEp(const uint8_t *data, uint32_t length);

#endif  /* __CDCUSER_H__ */

