/*!
 * @file        usb_fifo.c
 *
 * @brief       USB fifo handler
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

#include "usb_fifo.h"

/*!
 * @brief       Read the RxFIFO packet to buffer
 *
 * @param       rBuf:    Buffer pointer
 *
 * @param       rLen:    Buffer length
 *
 * @retval      None
 */
void USB_FIFO_ReadRxFifoPacket(uint8_t *rBuf, uint32_t rLen)
{
    uint32_t i = 0;
    __IO uint32_t temp = 0;

    for( i = 0; i < rLen / 4; i++)
    {
        *(__packed uint32_t *)rBuf = USB_OTG_ReadRxFifoData();
        rBuf += 4;
    }

    if (rLen & 0x3)
    {
        temp = USB_OTG_ReadRxFifoData();
        for (i = 0; i < (rLen & 0x3); i++)
        {
            rBuf[i] = temp >> (i << 3);
        }
    }
}

/*!
 * @brief       Write a buffer of data to a selected endpoint
 *
 * @param       ep:     Endpoint number
 *
 * @param       wBuf:   The pointer to the buffer of data to be written to the endpoint
 *
 * @param       wLen:   Number of data to be written (in bytes)
 *
 * @retval      None
 */
void USB_FIFO_WriteFifoPacket(uint8_t ep, uint8_t *wBuf, uint32_t wlen)
{
    uint32_t wordCnt = 0 , i = 0;

    wordCnt =  (wlen + 3) / 4;

    for (i = 0; i < wordCnt; i++)
    {
        USB_OTG_WriteTxFifoData(ep, *((__packed uint32_t *)wBuf) );
        wBuf+=4;
    }

}
