/*!
 * @file        usbd_class_msc.c
 *
 * @brief       MSC class request handler
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

#include "usbd_class_msc.h"
#include "usbd_msc_bot.h"
#include "usbd_msc.h"

static uint8_t s_mscMaxLen = 0;

/*!
 * @brief       handler Class
 *
 * @param       reqData: USB device request data
 *
 * @retval      None
 */
void USBD_MSC_ClassHandler(USBD_DevReqData_T* reqData)
{
    uint16_t wValue = ((uint16_t)reqData->domain.wValue[1] << 8) | \
                                 reqData->domain.wValue[0];
    uint16_t wLength = ((uint16_t)reqData->domain.wLength[1] << 8) | \
                                 reqData->domain.wLength[0];

    switch (reqData->domain.bRequest)
    {
        case BOT_GET_MAX_LUN :
            if ((wValue == 0) && (wLength == 1) && \
                (reqData->domain.bmRequestType.bit.dir == 1))
            {
                s_mscMaxLen = STORAGE_MAX_LUN - 1;

                USBD_CtrlInData(&s_mscMaxLen, 1);
            }
            else
            {
                USB_OTG_SetStall(USB_EP_0);
            }
            break;
        case BOT_RESET :
            if ((wValue == 0) && (wLength == 0) && \
                (reqData->domain.bmRequestType.bit.dir == 0))
            {
                USBD_CtrlInData(NULL, 0);
                /** Reset */
                USBD_MSC_BOT_Reset();
            }
            else
            {
                USB_OTG_SetStall(USB_EP_0);
            }

            break;

        default:
            USB_OTG_SetStall(USB_EP_0);
            break;
    }
}
