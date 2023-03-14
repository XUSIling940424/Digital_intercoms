/**
******************************************************************************
* @file         udp.h
* @version      V1.0
* @date         2020-06-05
* @brief        udp.c的头文件
*
* @company      深圳炜世科技有限公司
* @information  WIZnet W5500官方代理商，全程技术支持，价格优势大！
* @website      www.wisioe.com
* @forum        www.w5500.cn
* @qqGroup      579842114
* @Support      QQ:2571856470；Mob:18998931307
* @Email        support@wisioe.com
******************************************************************************
*/
#ifndef _UDP_H_
#define _UDP_H_

#include <stdint.h>
#include "socket.h"
#include "w5500.h"
#include "w5500_conf.h"
#include <stdio.h>
#include "my_netapp.h"


/* DATA_BUF_SIZE define for Loopback example */
#ifndef DATA_BUF_SIZE
#define DATA_BUF_SIZE  2048
#endif


extern struct Net_Msg udp_7777_recv_buff[5];

/* UDP Loopback test example */
void do_udp(void);

#endif
