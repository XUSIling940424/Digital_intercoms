#include "my_netapp.h"

/*�����߳̿��ƿ�ָ��*/
static rt_thread_t my_net_thread = RT_NULL;	
/*��������*/
static void mu_net_thread_entry(void *parameter);

struct Net_Port_Msg ser_6667_recv;   /*6667�������˿ڻ���*/
struct Net_Port_Msg ser_6666_recv;   /*6666�������˿ڻ���*/

static u8 Client_HandData(u8 *buf);
static uint8_t  server_rest(uint8_t * buf,u8 Number);

static uint8 getSn_DIPR(SOCKET s,u8* buf);

/*������������߳�*/
int build_my_net_thread(void)
{
		/*�����߳�*/
		my_net_thread=                              /* �߳̿��ƿ�ָ�� */
		rt_thread_create( "my_net",                 /* �߳����� */
											mu_net_thread_entry,      /* �߳���ں��� */
											RT_NULL,                /* �߳���ں������� */
											1024,                    /* �߳�ջ��С */
											2,                      /* �̵߳����ȼ� */
											500);                    /* �߳�ʱ��Ƭ */
		
		/*�����̣߳���������*/
		if(my_net_thread != RT_NULL){
			rt_kprintf("rt_thread_my_net_thread is success\n");
			rt_thread_startup(my_net_thread);
		}
		else{ 
			rt_kprintf("rt_thread_my_net_thread is faile\n");
			return -1;	
		}
}

static void mu_net_thread_entry(void *parameter)
{
	while(1){
   /*����������6667�˿�*/		
    tcp_server_list(SOCK_TCPS,6667);
   /*����������6666�˿�*/		
    tcp_server_list(SOCK_TCPS_2,6666);		
		/*�����������*/
		do_udp();		
		rt_thread_delay(10);  
	}
}

/*
�ͻ��˷��ͺ���
Number:w5500�˿ں�
client_poll:�ͻ��˶˿�
server_ip:���ӷ�������ַ
server_poll:���ӷ������˿ں�
sendbuf:��Ҫ���͵����ݵ�ַ
send_len:��Ҫ�������ݵĳ���
����ֵ��EXIT_FAILURE  
*/
uint8 tcp_client_send(u8 Number,u16 client_poll,uint8 * server_ip,u16 server_poll,u8 *sendbuf,u16 send_len)
{
	u8  out_time_num=0;
	uint16 len=0;	
	uint8_t tcp_client_buff[20];
	rt_err_t uwRet=RT_EOK;
	uint8_t ret=0;
	
	len = send_len;
 
  do{
		if(getSn_SR(Number)==SOCK_CLOSED)                                   // socket���ڹر�״̬
		{ 
			IINCHIP_WRITE(Sn_KPALVTR(Number),0x02);//���ӱ������
			socket(Number,Sn_MR_TCP,client_poll++,Sn_MR_ND);
		}
		else if(getSn_SR(Number)==SOCK_INIT)                                     // socket���ڳ�ʼ��״̬
		{ connect(Number,server_ip,server_poll);}
    else if(getSn_SR(Number)==SOCK_CLOSE_WAIT)                               // socket���ڵȴ��ر�״̬
			{
				disconnect(Number);                                             // �Ͽ���ǰTCP����
				close(Number);                                                  // �رյ�ǰ��ʹ��socket
        rt_thread_delay(100);				
			}				
    else{
		    ;
		}
		rt_thread_delay(10);
    out_time_num++;	
    if(out_time_num>=10){ 
			  out_time_num=0;
				disconnect(Number);                                             // �Ͽ���ǰTCP����
				close(Number);                                                  // �رյ�ǰ��ʹ��socket	
			  rt_thread_delay(100);
				return 0;
		}
	}
	while(getSn_SR(Number)!=SOCK_ESTABLISHED);                          //�ȴ�����״̬
	  out_time_num=0;
	  send(Number,sendbuf,len);                                         //��Server��������
	/*�ȴ�����������*/
	do{
	  if(getSn_IR(Number) & Sn_IR_CON)
     {
        setSn_IR(Number, Sn_IR_CON);                 // ��������жϱ�־λ
     }
		 len=getSn_RX_RSR(Number);
		 rt_thread_delay(10);
     out_time_num++;		 
	 if(out_time_num>=10){
		 out_time_num=0;
		 disconnect(Number);                                             // �Ͽ���ǰTCP����
		 close(Number);                                                  // �رյ�ǰ��ʹ��socket
     rt_thread_delay(100);		 
	   return 0;
		}
	}while(len<=0);
	  out_time_num=0;
	  recv(Number,tcp_client_buff,len);                       // ��������Server������
    tcp_client_buff[len]=0x00;                                 // ����ַ���������  	
		ret=Client_HandData(tcp_client_buff);
		disconnect(Number);                                             // �Ͽ���ǰTCP����
		close(Number);                                                  // �رյ�ǰ��ʹ��socket	
    rt_thread_delay(100);	
	  return ret;
}


/*
tcp�ͻ��˼�������
server_poll :��Ҫ�����Ķ˿�
Number :w5500�˿ں�
*/
void tcp_server_list(u8 Number,u16 server_poll)
{
  uint16 len=0;  
	uint8_t recv_tcp_erver_buff[320];
	rt_err_t uwRet=RT_EOK;
	uint8_t clietn_ip[4]={0};
	
  switch(getSn_SR(Number))                             // ��ȡsocket��״̬
  {
    case SOCK_CLOSED:                                     // socket���ڹر�״̬
			IINCHIP_WRITE(Sn_KPALVTR(Number),0x02);//���ӱ������
      socket(Number ,Sn_MR_TCP,server_poll,Sn_MR_ND);   // ��socket
      break;     
    
    case SOCK_INIT:                                       // socket�ѳ�ʼ��״̬
      listen(Number);                                  // socket��������
      break;
    
    case SOCK_ESTABLISHED:                                // socket�������ӽ���״̬		   
      if(getSn_IR(Number) & Sn_IR_CON)
      {
        setSn_IR(Number, Sn_IR_CON);                   // ��������жϱ�־λ
      }
      len=getSn_RX_RSR(Number);                        // ����lenΪ�ѽ������ݵĳ���
      if((len>0) && (len<=sizeof(recv_tcp_erver_buff)))
      {
        recv(Number,recv_tcp_erver_buff,len);                         // ��������Client������
        recv_tcp_erver_buff[len]=0x00;                                   // ����ַ���������
				/*��ȡ�ͻ���IP��ַ*/
				getSn_DIPR(Number,clietn_ip);
        rt_kprintf("clietn_ip=%d.%d.%d.%d\n",clietn_ip[0],clietn_ip[1],clietn_ip[2],clietn_ip[3]);				
				/*������Ӧ*/
				server_rest(recv_tcp_erver_buff,Number);	
				disconnect(Number);                              // �Ͽ���ǰTCP����
				close(Number);                                   // �رյ�ǰ��ʹ��socket				
      }
      break;
    
    case SOCK_CLOSE_WAIT:                                 // socket���ڵȴ��ر�״̬
			disconnect(Number);                              // �Ͽ���ǰTCP����
      close(Number);                                   // �رյ�ǰ��ʹ��socket
      break;
  }
}

/*
���ڷ��������յ����ݺ���Ӧ�����ź�
buf:���յ�������֡��ʼ��ַ
Number:���ͷ�����Ϣ�Ķ˿ں�
*/
static uint8_t server_rest(uint8_t * buf,u8 Number)
{
 struct Net_Comm   net_comm;
 u8 server_back_buf[10]={0};
 uint8_t clietn_ip[4]={0};
 
 getSn_DIPR(Number,clietn_ip);
 /**/
 net_comm.msg_from=buf[0];
 net_comm.msg_to=buf[1]; 
 net_comm.cmd=buf[2];
 net_comm.data=(buf[3]<<8) | buf[4];
 net_comm.zeros=buf[5];
 net_comm.client_ip[0]=clietn_ip[0];
 net_comm.client_ip[1]=clietn_ip[1];
 net_comm.client_ip[2]=clietn_ip[2];
 net_comm.client_ip[3]=clietn_ip[3];
 
// rt_kprintf("net_comm.msg_from=%x\n",net_comm.msg_from);
// rt_kprintf("net_comm.msg_to=%x\n",net_comm.msg_to);
// rt_kprintf("net_comm.cmd=%x\n",net_comm.cmd);
 
 switch(net_comm.cmd)
 {
		case AUX_Call_Com://�������л����ź�			
			break;
		
		case Com_Call_Aux://�������и����ź�
			break;
		
		case Aux_Call_DUT://��������ֵ�����ź�
			server_back_buf[0]=net_comm.msg_to;;
			server_back_buf[1]=net_comm.msg_from;
			server_back_buf[2]=net_comm.cmd;
			server_back_buf[3]=net_comm.data>>8;
			server_back_buf[4]=net_comm.data;
			server_back_buf[5]=net_comm.zeros;
			send(Number,server_back_buf,6);
			/*�����ṹ�嵽����*/
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].msg_from=net_comm.msg_from;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].msg_to=net_comm.msg_to;		
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].cmd=net_comm.cmd;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].data=net_comm.data;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].zeros=net_comm.zeros;
			rt_memcpy(ser_6667_recv.msg_buff[ser_6667_recv.msg_input].client_ip,net_comm.client_ip,4);
			ser_6667_recv.msg_input=(ser_6667_recv.msg_input+1)%MAX_NET_MSG_NUM; 		
			break;
		
		case Com_Call_Dut://��������ֵ����
			server_back_buf[0]=net_comm.msg_to;;
			server_back_buf[1]=net_comm.msg_from;
			server_back_buf[2]=net_comm.cmd;
			server_back_buf[3]=net_comm.data>>8;
			server_back_buf[4]=net_comm.data;
			server_back_buf[5]=net_comm.zeros;
			send(Number,server_back_buf,6);
			/*�����ṹ�嵽����*/
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].msg_from=net_comm.msg_from;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].msg_to=net_comm.msg_to;		
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].cmd=net_comm.cmd;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].data=net_comm.data;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].zeros=net_comm.zeros;
			rt_memcpy(ser_6667_recv.msg_buff[ser_6667_recv.msg_input].client_ip,net_comm.client_ip,4);
			ser_6667_recv.msg_input=(ser_6667_recv.msg_input+1)%MAX_NET_MSG_NUM; 			
			break;
		
		case Dut_Call_Aux://ֵ���Һ��и���
			break;
		
		case Take_Down://�һ�����				
			break;
		
		case Rst_Comm://������ֹ
			server_back_buf[0]=net_comm.msg_to;;
			server_back_buf[1]=net_comm.msg_from;
			server_back_buf[2]=net_comm.cmd;
			server_back_buf[3]=net_comm.data>>8;
			server_back_buf[4]=net_comm.data;
			server_back_buf[5]=net_comm.zeros;
			send(Number,server_back_buf,6);
			/*�����ṹ�嵽����*/
//		 rt_kprintf("net_comm.cmd=%d\n",net_comm.cmd);
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].msg_from=net_comm.msg_from;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].msg_to=net_comm.msg_to;		
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].cmd=Rst_Comm;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].data=net_comm.data;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].zeros=net_comm.zeros;
			rt_memcpy(ser_6667_recv.msg_buff[ser_6667_recv.msg_input].client_ip,net_comm.client_ip,4);
//		  rt_kprintf("ser_6667_recv.msg_buff[%d].cmd=%x\n",ser_6667_recv.msg_input,ser_6667_recv.msg_buff[ser_6667_recv.msg_input].cmd);
			ser_6667_recv.msg_input=(ser_6667_recv.msg_input+1)%MAX_NET_MSG_NUM; 
      		
			break;
		
		case Check_Com://�ֶ����
			break;
		
		case HearBeat://�������ϴ�
			rt_kprintf("recv HearBeat msg\n");
			server_back_buf[0]=net_comm.msg_to;;
			server_back_buf[1]=net_comm.msg_from;
			server_back_buf[2]=net_comm.cmd;
			server_back_buf[3]=net_comm.data>>8;
			server_back_buf[4]=net_comm.data;
			server_back_buf[5]=net_comm.zeros;
			send(Number,server_back_buf,6);
			/*�����ṹ�嵽����*/
			ser_6666_recv.msg_buff[ser_6666_recv.msg_input].msg_from=net_comm.msg_from;
			ser_6666_recv.msg_buff[ser_6666_recv.msg_input].msg_to=net_comm.msg_to;		
			ser_6666_recv.msg_buff[ser_6666_recv.msg_input].cmd=net_comm.cmd;
			ser_6666_recv.msg_buff[ser_6666_recv.msg_input].data=net_comm.data;
			ser_6666_recv.msg_buff[ser_6666_recv.msg_input].zeros=net_comm.zeros;
			rt_memcpy(ser_6666_recv.msg_buff[ser_6666_recv.msg_input].client_ip,net_comm.client_ip,4);
			ser_6666_recv.msg_input=(ser_6666_recv.msg_input+1)%MAX_NET_MSG_NUM; 							
			break;
		
		case ALA_FILT://�������������ϴ�
			server_back_buf[0]=net_comm.msg_to;;
			server_back_buf[1]=net_comm.msg_from;
			server_back_buf[2]=net_comm.cmd;
			server_back_buf[3]=net_comm.data>>8;
			server_back_buf[4]=net_comm.data;
			server_back_buf[5]=net_comm.zeros;
			send(Number,server_back_buf,6);
			/*�����ṹ�嵽����*/
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].msg_from=net_comm.msg_from;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].msg_to=net_comm.msg_to;		
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].cmd=net_comm.cmd;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].data=net_comm.data;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].zeros=net_comm.zeros;
			rt_memcpy(ser_6667_recv.msg_buff[ser_6667_recv.msg_input].client_ip,net_comm.client_ip,4);
			ser_6667_recv.msg_input=(ser_6667_recv.msg_input+1)%MAX_NET_MSG_NUM; 			
			break;
		
		case Elec_DULER://���Ƿѹ
			break;
		
		case VOILD_Chang://��������
			break;
		
		default:
			break;
 }
}

/*�ͻ������ݴ�����*/
static u8 Client_HandData(u8 *buf)
{
	u8 ret=0;
	struct Net_Comm   net_comm;
	struct Usart_Comm  my_usart3_comm;
	uint8_t usart_send_buff[20]={0};

	my_usart3_comm.Head[0]=0xff;  my_usart3_comm.Head[1]=0x55; my_usart3_comm.Head[2]=0x00;

  net_comm.msg_from=buf[0];
  net_comm.msg_to=buf[1];	
  net_comm.cmd=buf[2];
  net_comm.data=(buf[3]<<8) | buf[4];
  net_comm.zeros=buf[5];

 switch(net_comm.cmd)
 {
		case AUX_Call_Com://�������л����ź�	
      ret=AUX_Call_Com;		
			break;
		
		case Com_Call_Aux://�������и����ź�
			ret=Com_Call_Aux;		
			break;
		
		case Aux_Call_DUT://��������ֵ�����ź�
      ret=Aux_Call_DUT;					
			break;
		
		case Com_Call_Dut://��������ֵ����
      ret=Com_Call_Dut;			
			break;
		
		case Dut_Call_Aux://ֵ���Һ��и���
  		ret=Dut_Call_Aux;   rt_kprintf("recv Dut_Call_Aux msg\n");
			break;
		
		case Take_Down://�һ�����	
  		ret=Take_Down;		rt_kprintf("recv Take_Down msg\b");
			break;
		
		case Rst_Comm://������ֹ
		  ret=Rst_Comm;		rt_kprintf("recv Rst_Comm msg\b");		
			break;
		
		case Check_Com://�ֶ����
			if(net_comm.data & 0x08){   //����������Ϣ
				My_Attribute.com_line=1;
			}else{
			  My_Attribute.com_line=0;
			}
			if(net_comm.data & 0x04){   //�׿�������Ϣ
				My_Attribute.pit_line=1;
			}else{
			  My_Attribute.pit_line=0;
			}
			if(net_comm.data & 0x02){   //����������Ϣ
				My_Attribute.lif_line=1;
			}else{
			  My_Attribute.lif_line=0;
			}
			if(net_comm.data & 0x01){   //�ζ�������Ϣ
				My_Attribute.car_line=1;
			}else{
			  My_Attribute.car_line=0;
			}				
			ret=Check_Com;
			break;
		
		case HearBeat://�������ϴ�	
      ret=HearBeat;			
			break;
		
		case ALA_FILT://�������������ϴ�
			ret=ALA_FILT;
			break;
		
		case Elec_DULER://���Ƿѹ
			ret=Elec_DULER;
			break;
		
		case VOILD_Chang://��������
			ret=VOILD_Chang;
			break;
		
		default:
			break;
 }
#if 0	
  switch(net_comm.head)
  {
	  case 0x01:
		  switch(net_comm.cmd)
		  {
			  case Call_Comm| Rest_Comm:  //���з���
					my_usart3_comm.Comm=SCREE_CALL | USART3_BLACK;   	my_usart3_comm.DatLen=0x04;
        	my_usart3_comm.Data[0]=remote_ip[3];
          my_usart3_comm.Data[1]=net_comm.msg_from;
					my_usart3_comm.Data[2]=0x01; my_usart3_comm.Data[3]=0x00;
          usart3_send_comm(&my_usart3_comm);  ret=SCREE_CALL;
				  break;
				
			  case Take_down| Rest_Comm:  //�һ�����
					my_usart3_comm.Comm=SCREE_TAKE_DOWN | USART3_BLACK;   	my_usart3_comm.DatLen=0x04;
        	my_usart3_comm.Data[0]=remote_ip[3];
          my_usart3_comm.Data[1]=0x01;
					my_usart3_comm.Data[2]=0x01; my_usart3_comm.Data[3]=0x00;
          usart3_send_comm(&my_usart3_comm);  ret=SCREE_TAKE_DOWN;
				  break;

				case Rst_Comm | Rest_Comm: rt_kprintf("Rst_Comm\n");//��λ����
					my_usart3_comm.Comm=SCREE_REST | USART3_BLACK;   	my_usart3_comm.DatLen=0x04;
        	my_usart3_comm.Data[0]=remote_ip[3];
          my_usart3_comm.Data[1]=01;
					my_usart3_comm.Data[2]=0x01; my_usart3_comm.Data[3]=0x00;
          usart3_send_comm(&my_usart3_comm);	ret=SCREE_REST;		
					break;
				
				case RES_CALL | Rest_Comm:	rt_kprintf("RES_CALL\n");//����Ӧ��
					my_usart3_comm.Comm=SCREE_RES_CALL | USART3_BLACK;   	my_usart3_comm.DatLen=0x04;
        	my_usart3_comm.Data[0]=remote_ip[3];
          my_usart3_comm.Data[1]=01;
					my_usart3_comm.Data[2]=0x01; my_usart3_comm.Data[3]=0x00;
          usart3_send_comm(&my_usart3_comm);	ret=SCREE_RES_CALL;				  
					break;
		  }
		  break;
	 
	  case 0x00:
		  break;
	 
	  default:
		  break;
 }
#endif 
	return ret;
}

/*��ȡ�ͻ���IP��ַ*/
static uint8 getSn_DIPR(SOCKET s,u8* buf)
{
	buf[0] = IINCHIP_READ(Sn_DIPR0(s));
	buf[1] = IINCHIP_READ(Sn_DIPR1(s));
	buf[2] = IINCHIP_READ(Sn_DIPR2(s));
	buf[3] = IINCHIP_READ(Sn_DIPR3(s));
	
	return 1;
}

