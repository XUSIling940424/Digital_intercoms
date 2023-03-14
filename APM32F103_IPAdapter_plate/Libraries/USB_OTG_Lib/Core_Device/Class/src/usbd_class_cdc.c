/*!
 * @file        usbd_class_cdc.c
 *
 * @brief       CDC class source file
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

#include "usbd_class_cdc.h"
#include "usbd_init.h"
#include "usb_user.h"
#include "usbd_descriptor.h"

static uint8_t cmdBuf[8] = {0};

/*!
 * @brief       Class Request handler
 *
 * @param       reqData : point to setup request
 *
 * @retval      None
 */
void USBD_ClassHandler(USBD_DevReqData_T* reqData)
{
    uint16_t length = ((uint16_t)reqData->domain.wLength[1] << 8) | \
                     reqData->domain.wLength[0] ;

    if (length)
    {
        if (reqData->domain.bmRequestType.bit.dir != 0)
        {
            USBD_CtrlInData(cmdBuf, length);
        }
        /** Host to Device */
        else
        {
            USBD_CtrlOutData(cmdBuf, length);
        }
    }
    else
    {
        USBD_CtrlTxStatus();
    }
}

