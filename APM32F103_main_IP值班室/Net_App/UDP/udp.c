/**
******************************************************************************
* @file         udp.c
* @version      V1.0
* @date         2020-06-05
* @brief        udp数据回环测试
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

#include "udp.h"


uint8 		A_TX_buff[1514];
uint8     A_RX_buff[1514]; 

struct Net_Msg udp_7777_recv_buff[5]={0};

void do_udp(void)
{
  uint16 len=0;
	u32 ces,ses=0;
	uint16 w=0;
	int16_t Audio_data,Audio_datas,idx=0;
	uint16 lens=0;
	uint16 i=0;
	
  switch(getSn_SR(SOCK_UDPS))                                     // 获取socket的状态
  {
    case SOCK_CLOSED:                                             // socket处于关闭状态
      socket(SOCK_UDPS,Sn_MR_UDP,local_port,0);                   // 初始化socket
      break;
    case SOCK_UDP:                                                // socket初始化完成
      if(getSn_IR(SOCK_UDPS) & Sn_IR_RECV)
      {
        setSn_IR(SOCK_UDPS, Sn_IR_RECV);                          // 清接收中断
      }
	/*DAC*/	
			if((lens=getSn_RX_RSR(SOCK_UDPS))>0)
			{
        if(recvfrom(SOCK_UDPS,A_RX_buff, lens, remote_ip,&remote_port)) // 接收到数据 				
				{ 
          if(lens<=150){					
					A_RX_buff[lens-8]=0;
						rt_memset(udp_7777_recv_buff[A_RX_buff[0]].msg_buff[udp_7777_recv_buff[A_RX_buff[0]].msg_input],0,121);
						rt_memcpy(udp_7777_recv_buff[A_RX_buff[0]].msg_buff[udp_7777_recv_buff[A_RX_buff[0]].msg_input],&A_RX_buff[1],120);
						udp_7777_recv_buff[A_RX_buff[0]].msg_input=((udp_7777_recv_buff[A_RX_buff[0]].msg_input+1)%(MAX_NET_MSG_NUM));
            						
					}else {
					  lens=0;
					}						
				}	
			}		
/*ADC*/			
			if(my_usart_msg.msg_output!=my_usart_msg.msg_input){
				rt_memcpy(A_TX_buff,my_usart_msg.msg_buff[my_usart_msg.msg_output],120);
				sendto(SOCK_UDPS,A_TX_buff,120, remote_ip, remote_port);
				my_usart_msg.msg_output=((my_usart_msg.msg_output+1)%MAX_USART_MSG_NUM);
			}														
      break;
  }
}
