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

struct Net_Msg   udp_7777_recv_buff;

void do_udp(void)
{
  uint16 len=0;
	u32 ces,ses=0;
	uint16 w=0;
	int16_t Audio_data,Audio_datas,idx=0;
	uint16 lens=0;
	uint8  i=0;
	
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
				GPIO_WriteBitValue(GPIOB,GPIO_PIN_6,!GPIO_ReadOutputBit(GPIOB,GPIO_PIN_6));
        if(recvfrom(SOCK_UDPS,A_RX_buff, lens, remote_ip,&remote_port)) // 接收到数据 				
				{  				
					for(i=0;i<8;i++)
           {
					   rt_memcpy(&udp_7777_recv_buff.msg_buff[udp_7777_recv_buff.msg_input][i*10],&A_RX_buff[i*15+5],10);
					 }	
          udp_7777_recv_buff.msg_input=((udp_7777_recv_buff.msg_input+1)%(MAX_NET_MSG_NUM));				 
				}	
			}else{
				  ;
			}
/*ADC*/			
			/*can语音消息检测*/
//			if((Recv_Can_buff[0].msg_output!=Recv_Can_buff[0].msg_input)| (Recv_Can_buff[1].msg_output!=Recv_Can_buff[1].msg_input) |\
//  			(Recv_Can_buff[2].msg_output!=Recv_Can_buff[2].msg_input)| (Recv_Can_buff[3].msg_output!=Recv_Can_buff[3].msg_input))
//			   GPIO_WriteBitValue(GPIOB,GPIO_PIN_8,!GPIO_ReadOutputBit(GPIOB,GPIO_PIN_8));
//			else
//				GPIO_WriteBitValue(GPIOB,GPIO_PIN_8,1);
			
			if(Recv_Can_buff[0].msg_output!=Recv_Can_buff[0].msg_input){  //第一路
				rt_memset(&A_TX_buff[0],0,sizeof(A_TX_buff));
				A_TX_buff[0]=0x01;
				for(i=0;i<8;i++){
					A_TX_buff[i*15+1]=0xef;    A_TX_buff[i*15+2]=0xfa;
					A_TX_buff[i*15+3]=0x08;  A_TX_buff[i*15+4]=0x0b;  A_TX_buff[i*15+5]=0x01;		
					rt_memcpy(&A_TX_buff[i*15+6],&Recv_Can_buff[0].msg_buff[Recv_Can_buff[0].msg_output][i*10],10);					
				}
				Recv_Can_buff[0].msg_output=((Recv_Can_buff[0].msg_output+1)%MAX_MSG_NUM);
				sendto(SOCK_UDPS,A_TX_buff,121, remote_ip, remote_port);     // W5500把接收到的数据发送给Remote, remote_port);
//        rt_kprintf("send is num 1\n");				
			}
			 if(Recv_Can_buff[1].msg_output!=Recv_Can_buff[1].msg_input){  //第二路
				rt_memset(&A_TX_buff[0],0,sizeof(A_TX_buff));
				A_TX_buff[0]=0x02;
				for(i=0;i<8;i++){
					A_TX_buff[i*15+1]=0xef;    A_TX_buff[i*15+2]=0xfa;
					A_TX_buff[i*15+3]=0x08;  A_TX_buff[i*15+4]=0x0b;  A_TX_buff[i*15+5]=0x02;		
					rt_memcpy(&A_TX_buff[i*15+6],&Recv_Can_buff[1].msg_buff[Recv_Can_buff[1].msg_output][i*10],10);					
				}
				Recv_Can_buff[1].msg_output=((Recv_Can_buff[1].msg_output+1)%MAX_MSG_NUM);
				sendto(SOCK_UDPS,A_TX_buff,121, remote_ip, remote_port);     // W5500把接收到的数据发送给Remote, remote_port);				
//				rt_kprintf("send is num 2\n");
			}
			if(Recv_Can_buff[2].msg_output!=Recv_Can_buff[2].msg_input){  //第三路
				rt_memset(&A_TX_buff[0],0,sizeof(A_TX_buff));
				A_TX_buff[0]=0x03;
				for(i=0;i<8;i++){
					A_TX_buff[i*15+1]=0xef;    A_TX_buff[i*15+2]=0xfa;
					A_TX_buff[i*15+3]=0x08;  A_TX_buff[i*15+4]=0x0b;  A_TX_buff[i*15+5]=0x03;		
					rt_memcpy(&A_TX_buff[i*15+6],&Recv_Can_buff[2].msg_buff[Recv_Can_buff[2].msg_output][i*10],10);					
				}
				Recv_Can_buff[2].msg_output=((Recv_Can_buff[2].msg_output+1)%MAX_MSG_NUM);
				sendto(SOCK_UDPS,A_TX_buff,121, remote_ip, remote_port);     // W5500把接收到的数据发送给Remote, remote_port);				
//				rt_kprintf("send is num 3\n");
			}
			if(Recv_Can_buff[3].msg_output!=Recv_Can_buff[3].msg_input){  //第四路
				rt_memset(&A_TX_buff[0],0,sizeof(A_TX_buff));
				A_TX_buff[0]=0x04;
				for(i=0;i<8;i++){
					A_TX_buff[i*15+1]=0xef;    A_TX_buff[i*15+2]=0xfa;
					A_TX_buff[i*15+3]=0x08;  A_TX_buff[i*15+4]=0x0b;  A_TX_buff[i*15+5]=0x04;		
					rt_memcpy(&A_TX_buff[i*15+6],&Recv_Can_buff[3].msg_buff[Recv_Can_buff[3].msg_output][i*10],10);					
				}
				Recv_Can_buff[3].msg_output=((Recv_Can_buff[3].msg_output+1)%MAX_MSG_NUM);
				sendto(SOCK_UDPS,A_TX_buff,121, remote_ip, remote_port);     // W5500把接收到的数据发送给Remote, remote_port);      				
//				rt_kprintf("send is num 4\n");
			}				
      break;
  }
}
