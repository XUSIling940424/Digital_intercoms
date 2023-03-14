#include "my_netapp.h"

struct Net_Port_Msg ser_6667_recv;   /*6667�������˿ڻ���*/

static u8 Client_HandData(u8 *buf);
static uint8_t  server_rest(uint8_t * buf,u8 Number);

/*�����߳̿��ƿ�ָ��*/
static rt_thread_t my_net_thread = RT_NULL;	
/*��������*/
static void mu_net_thread_entry(void *parameter);

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
	u16  out_time_num=0;
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
    else{;}
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
      if(len>0)
      {
        recv(Number,recv_tcp_erver_buff,len);                         // ��������Client������
        recv_tcp_erver_buff[len]=0x00;                                   // ����ַ���������
				/*������Ӧ*/
				server_rest(recv_tcp_erver_buff,Number);			
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
static uint8_t  server_rest(uint8_t * buf,u8 Number)
{
	struct Net_Comm   net_comm;
  u8 server_back_buf[10]={0};
	
	net_comm.msg_from=buf[0];
	net_comm.msg_to=buf[1]; 
	net_comm.cmd=buf[2];
	net_comm.data=(buf[3]<<8) | buf[4];
	net_comm.zeros=buf[5];
	
	
	rt_kprintf("net_comm.cmd=%d\n",net_comm.cmd);
	switch(net_comm.cmd)
	{
		case AUX_Call_Com://�������л����ź�			
			break;
		
		case Com_Call_Aux://�������и����ź�
			break;

		case Aux_Call_DUT://��������ֵ�����ź�	
		 break;
		
		case Com_Call_Dut://��������ֵ����
		 break;

		case Dut_Call_Aux://ֵ���Һ��и���
			/*�����ͻ�����Ϣ*/
			server_back_buf[0]=net_comm.msg_to;
			server_back_buf[1]=net_comm.msg_from;
			server_back_buf[2]=net_comm.cmd;
			server_back_buf[3]=net_comm.data>>8;
			server_back_buf[4]=net_comm.data;
			server_back_buf[5]=net_comm.zeros;	
			send(Number,server_back_buf,6);	
			 /*ת������*/
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=net_comm.msg_from;	
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=net_comm.msg_to;	
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=net_comm.cmd;										
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=net_comm.data;
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.zeros=net_comm.zeros;		
			
			my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
			rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/			
		 break;	

		case Take_Down://�һ�����	
			/*�����ͻ�����Ϣ*/
			server_back_buf[0]=net_comm.msg_to;
			server_back_buf[1]=net_comm.msg_from;
			server_back_buf[2]=net_comm.cmd;
			server_back_buf[3]=net_comm.data>>8;
			server_back_buf[4]=net_comm.data;
			server_back_buf[5]=net_comm.zeros;	
			send(Number,server_back_buf,6);	
			 /*ת������*/
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=net_comm.msg_from;	
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=net_comm.msg_to;	
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=net_comm.cmd;										
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=net_comm.data;
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.zeros=net_comm.zeros;	
			
			my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
			rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/					
		 break;	

		case Rst_Comm://������ֹ	
			/*�����ͻ�����Ϣ*/
			server_back_buf[0]=net_comm.msg_to;
			server_back_buf[1]=net_comm.msg_from;
			server_back_buf[2]=net_comm.cmd;
			server_back_buf[3]=net_comm.data>>8;
			server_back_buf[4]=net_comm.data;
			server_back_buf[5]=net_comm.zeros;	
			send(Number,server_back_buf,6);	
			 /*ת������*/
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=net_comm.msg_from;	
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=net_comm.msg_to;	
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=net_comm.cmd;										
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=net_comm.data;
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.zeros=net_comm.zeros;		
			
			my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
			rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/			
		 break;
		
		case Check_Com://�ֶ����
			/*�����ͻ�����Ϣ*/
			net_comm.data=0x00 |( My_Attribute.com_line <<3) | ( My_Attribute.pit_line  <<2)|  (My_Attribute.lif_line <<1) |  My_Attribute.car_line;
			server_back_buf[0]=net_comm.msg_to;
			server_back_buf[1]=net_comm.msg_from;
			server_back_buf[2]=net_comm.cmd;
			server_back_buf[3]=net_comm.data>>8;
			server_back_buf[4]=net_comm.data;
			server_back_buf[5]=net_comm.zeros;	
			send(Number,server_back_buf,6);
      rt_kprintf("server_back_buf[4]=%x\n",server_back_buf[4]);		
		 break;
		
		case HearBeat://�������ϴ�
		 break;

		case ALA_FILT://�������������ϴ�
		 break;

		case Elec_DULER://���Ƿѹ
		 break;	

		case VOILD_Chang://��������
			/*�����ͻ�����Ϣ*/
			server_back_buf[0]=net_comm.msg_to;
			server_back_buf[1]=net_comm.msg_from;
			server_back_buf[2]=net_comm.cmd;
			server_back_buf[3]=net_comm.data>>8;
			server_back_buf[4]=net_comm.data;
			server_back_buf[5]=net_comm.zeros;	
			send(Number,server_back_buf,6);	
			 /*ת������*/
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=net_comm.msg_from;	
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=net_comm.msg_to;	
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=net_comm.cmd;										
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=net_comm.data;
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.zeros=net_comm.zeros;		
			
			my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
			rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/			
		 break;
		
		default:
			break;
	}		
}



/*�ͻ������ݴ�����*/
static u8 Client_HandData(u8 *buf)
{
  u8 ret=1;

	my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=buf[0];	
	my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=buf[1];	
	my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=buf[2];										
	my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=(buf[3]<<8) | buf[4];
	my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.zeros=buf[5];	
	my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
	rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/	

	return ret;
}
