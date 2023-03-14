/*!
 * @file        usbd_init.h
 *
 * @brief       USB device initialization
 *
 * @version     V1.0.1
 *
 * @date        2022-05-25
 *
 * @attention
 *
 *  Copyright (C) 2021-2022 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be usefull and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */
#ifndef __USBD_INIT_H_
#define __USBD_INIT_H_

#include "usbd_core.h"


void USBD_Init(USBD_InitParam_T *param);
void USBD_InitParamStructInit(USBD_InitParam_T *param);

void USBD_OpenOutEP(uint8_t ep, USB_EP_TYPE_T epType, uint32_t packSize);
void USBD_OpenInEP(uint8_t ep, USB_EP_TYPE_T epType, uint32_t packSize);
void USBD_CloseOutEP(uint8_t ep);
void USBD_CloseInEP(uint8_t ep);

void USB_CoreSoftReset(void);

#endif
