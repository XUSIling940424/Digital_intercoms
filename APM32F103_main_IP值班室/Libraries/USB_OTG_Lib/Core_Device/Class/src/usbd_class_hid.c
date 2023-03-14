/*!
 * @file        usbd_class_hid.c
 *
 * @brief       HID Class source file
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

#include "usbd_class_hid.h"
#include "usbd_Init.h"
#include "usbd_descriptor.h"

static uint8_t s_hidIdleState;
static uint8_t s_hidProtocol;

/*!
 * @brief       USB HID Class request handler
 *
 * @param       reqData : point to USBD_DevReqData_T structure
 *
 * @retval      None
 */
void USBD_ClassHandler(USBD_DevReqData_T* reqData)
{
    switch (reqData->domain.bRequest)
    {
        case HID_CLASS_REQ_SET_IDLE:
            s_hidIdleState = reqData->domain.wValue[1];
            USBD_CtrlTxStatus();
            break;

        case HID_CLASS_REQ_GET_IDLE:
            USBD_CtrlInData(&s_hidIdleState, 1);
            break;

        case HID_CLASS_REQ_SET_PROTOCOL:
            s_hidProtocol = reqData->domain.wValue[0];
            USBD_CtrlTxStatus();
            break;

        case HID_CLASS_REQ_GET_PROTOCOL:
            USBD_CtrlInData(&s_hidProtocol, 1);
            break;

        default:
            USB_OTG_SetStall(USB_EP_0);
            break;
    }
}
