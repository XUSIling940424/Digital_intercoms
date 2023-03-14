#include "main.h"

/*�����߳̿��ƿ�ָ��*/
static rt_thread_t msg_thread = RT_NULL;
	
/*��������*/
static void msg_thread_entry(void *parameter);
static  void  remote_msg_handle(struct Remote_Msg * msg);
static unsigned char voice_chang1[5]={0xef,0xfa,0x04,0x01,0x06};  /*0-13��*/
static unsigned char voice_chang2[5]={0xef,0xfa,0x05,0x01,0x20};  /*0-63��*/

int x1_lin_time=0;
int x2_lin_time=0;
int k1_lin_time=0;
char k1_lin_temp=0;


uint8  local_ips[4]={0};
	

int main(void)
{	
	uint8_t client_send_buf[10]={0};	  uint8_t ret=0;   uint16_t   time_cnt=0;
	
	client_send_buf[0]=0x00;client_send_buf[1]=0x05;client_send_buf[2]=HearBeat;
	client_send_buf[3]=0x00;
	client_send_buf[4]=0x00;
	client_send_buf[5]=0;
	/*��ȡ��ַ��Ϣ*/
	local_ip[3]=Read_Addr_Msg();	
	/*�����ʼ��*/
	 reset_w5500(); 		                    // w5500Ӳ����λ 
	 PHY_check();                          // ���߼�����	
	 set_w5500_mac();                      // ����w5500MAC��ַ
	 set_w5500_ip();                      // ����w5500�������
	 setRTR(500);                         //���ó�ʱʱ�� 100us*time_out  100us*500=50ms
	 setRCR(3);                           //�����ش�����	
	 socket_buf_init(txsize, rxsize);      // ��ʼ��4��Socket�ķ��ͽ��ջ����С	
	 GPIO_WriteBitValue(GPIOB,GPIO_PIN_7,1);
	 init_data();
	 build_can_recv_thread();
	 build_can_send_thread();
	 build_my_net_thread();

		/*�����߳�*/
		msg_thread=                              /* �߳̿��ƿ�ָ�� */
		rt_thread_create( "msg_send",                 /* �߳����� */
											msg_thread_entry,      /* �߳���ں��� */
											RT_NULL,                /* �߳���ں������� */
											1024,                    /* �߳�ջ��С */
											3,                      /* �̵߳����ȼ� */
											200);                    /* �߳�ʱ��Ƭ */
		
		/*�����̣߳���������*/
		if(msg_thread != RT_NULL){
			rt_kprintf("rt_thread_create is success\n");
			rt_thread_startup(msg_thread);
		}
		else{ 
			rt_kprintf("rt_thread_create is faile\n");
			return -1;	
		}	
    while (1)
    {
			GPIO_WriteBitValue(GPIOB,GPIO_PIN_9,!GPIO_ReadOutputBit(GPIOB,GPIO_PIN_9));	

			if((ret==0) && (time_cnt>=100)){
				client_send_buf[4]=0x00 |( My_Attribute.com_line <<3) | ( My_Attribute.pit_line  <<2)|  (My_Attribute.lif_line <<1) |  My_Attribute.car_line;
			  ret=tcp_client_send(SOCK_TCPC_2,6669,remote_ip,6666,client_send_buf,6);
				time_cnt=0;
				if(ret==0)  GPIO_WriteBitValue(GPIOB,GPIO_PIN_7,1);
				else GPIO_WriteBitValue(GPIOB,GPIO_PIN_7,0);
			}
			if((ret==1) && (time_cnt>=200)){
				client_send_buf[4]=0x00 |( My_Attribute.com_line <<3) | ( My_Attribute.pit_line  <<2)|  (My_Attribute.lif_line <<1) |  My_Attribute.car_line;
			  ret=tcp_client_send(SOCK_TCPC_2,6669,remote_ip,6666,client_send_buf,6);
				time_cnt=0;
				if(ret==0)  GPIO_WriteBitValue(GPIOB,GPIO_PIN_7,1);
				else GPIO_WriteBitValue(GPIOB,GPIO_PIN_7,0);
			}
			
			rt_thread_delay(100);   time_cnt++;
    }	
}


/*�̶߳���*/
static void msg_thread_entry(void *paramter)
{
	int   i=0;
	int 	ret=0;
	
  
	Can1_Init();
	while(1){	
		
    /*���can����*/
		if(my_remote_msg.Msg_Out!=my_remote_msg.Msg_In){
			remote_msg_handle(&my_remote_msg);  
			my_remote_msg.Msg_Out=((my_remote_msg.Msg_Out+1)%MAX_MSG_NUM);
		}
		/*�����������*/
		if(udp_7777_recv_buff.msg_input!=udp_7777_recv_buff.msg_output){
		  rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/
		}
		rt_thread_delay(10);
	}
}


/*�������*/

static  void  remote_msg_handle(struct Remote_Msg * msg)
{
	uint8_t client_send_buf[10]={0};
  uint8_t ret=0;	
	
	rt_kprintf("msg->Can_Msg[msg->Msg_Out].msg.msg_from=%d\n",msg->Can_Msg[msg->Msg_Out].msg.msg_from);
	rt_kprintf("msg->Can_Msg[msg->Msg_Out].msg.msg_to=%d\n",msg->Can_Msg[msg->Msg_Out].msg.msg_to);
	rt_kprintf("msg->Can_Msg[msg->Msg_Out].msg.cmd=%d\n",msg->Can_Msg[msg->Msg_Out].msg.cmd);
	
	switch(msg->Can_Msg[msg->Msg_Out].msg.cmd)
	{
		case AUX_Call_Com://�������л����ź�			
			break;
		
		case Com_Call_Aux://�������и����ź�
			break;
		
		case Aux_Call_DUT://��������ֵ�����ź�	
			client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
			client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
			client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
		  client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.data>>8;
      client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
			tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
			rt_kprintf("recv Com_Room rest  back\n");			
			break;
		
		case Com_Call_Dut://��������ֵ����
			client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
			client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
			client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
		  client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.data>>8;
      client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
			tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
			rt_kprintf("recv Com_Call_Dut\n");				
			break;
		
		case Dut_Call_Aux://ֵ���Һ��и���
			rt_kprintf("recv Dut_Call_Aux rest  back\n");
			break;
		
		case Take_Down://�һ�����		
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room){  //���Ի����һ�
				client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
				client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
				client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
				client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.data>>8;
				client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
				tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
			}else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Car_roof){  //���Խζ��һ�
				client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
				client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
				client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
				client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.data>>8;
				client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
				tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
			}	else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Car_roof){  //���Ե׿ӹһ�
				client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
				client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
				client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
				client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.data>>8;
				client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
				tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
			}
      rt_kprintf("recv Take_Down\n");			
			break;
		
		case Rst_Comm://������ֹ
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room){  //���Ի�����λ����
				client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
				client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
				client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
				client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.data>>8;
				client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
				tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
				rt_kprintf("recv Com_Room rest  back\n");
			}else{
			 ;
			}				
			break;
		
		case Check_Com://�ֶ����			
			break;
		
		case HearBeat://�������ϴ�
			rt_kprintf("msg->Can_Msg[msg->Msg_Out].msg.data=%x\n",msg->Can_Msg[msg->Msg_Out].msg.data);
		  if(msg->Can_Msg[msg->Msg_Out].msg.data & 0x01){  //�ζ�����
				GPIO_WriteBitValue(GPIOB,GPIO_PIN_6,0);
				My_Attribute.car_line=1;
			}else{
				My_Attribute.car_line=0;
			  GPIO_WriteBitValue(GPIOB,GPIO_PIN_6,1);
			}if(msg->Can_Msg[msg->Msg_Out].msg.data & 0x02){  //��������
				My_Attribute.lif_line=1;
				GPIO_WriteBitValue(GPIOB,GPIO_PIN_8,0);			 
			}else{
				My_Attribute.lif_line=0;
				GPIO_WriteBitValue(GPIOB,GPIO_PIN_8,1);	
			}if(msg->Can_Msg[msg->Msg_Out].msg.data & 0x04){  //�׿�����
			  My_Attribute.pit_line=1;
			}else{
			  My_Attribute.pit_line=0;
			}if(msg->Can_Msg[msg->Msg_Out].msg.data & 0x08){  //��������
			   My_Attribute.com_line=1;
			}else{
			   My_Attribute.com_line=0;
			} 				
			break;
		
		case ALA_FILT://�������������ϴ�
			client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
			client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
			client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
		  client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.data>>8;
      client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
			tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
			rt_kprintf("recv ALA_FILT\n");				
			break;
		
		case Elec_DULER://���Ƿѹ
			break;
		
		case VOILD_Chang://��������
//			client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
//			client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
//			client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
//		  client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.data>>8;
//      client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
//			tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
//			rt_kprintf("recv Com_Room rest  back\n");					
//			break;
		
		default:
			break;
	}	

#if 0
	
	switch(msg->Can_Msg[msg->Msg_Out].msg.head)
	{
		case 0x01:    //������Ϣ
			if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Call_Comm|Rest_Comm))  //���з���
			{
				if(msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room)
				{
					rt_kprintf("recv Duty_Room call back\n");
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Take_down|Rest_Comm))  //�һ�����
			{
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Rst_Comm|Rest_Comm))  //������λ����
			{
				if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Lift_Addr) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room))
				{
					rt_kprintf("recv Com_Room rest  back\n");
				}else if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room))
				{
					client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.head;
					client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
					client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
					client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
					client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
					client_send_buf[5]=msg->Can_Msg[msg->Msg_Out].msg.zeros;
					tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
					rt_kprintf("recv Com_Room rest  back\n");
				}				
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Check_Com|Rest_Comm))  //�ֶ���ⷴ��
			{
			if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Lift_Addr) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room))
				{
					client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.head;
					client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
					client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
					client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
					client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
					client_send_buf[5]=msg->Can_Msg[msg->Msg_Out].msg.zeros;
					tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
					rt_kprintf("recv Com_Room check  back\n");
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(RES_CALL|Rest_Comm))  //����Ӧ����
			{
				if(msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room)
				{
					rt_kprintf("recv Duty_Room call back\n");
				}
			}
			break;
		
		case 0x00:    //������Ϣ
			if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Call_Comm))  //����
			{
				if((msg->Can_Msg[msg->Msg_Out].msg.msg_from | (Com_Room | Lift_Addr | Car_roof | Pit_Addr)) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room))
				{
					rt_kprintf("recv call from Com_Room\n");
					client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.head;
					client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
					client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
					client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
					client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
					client_send_buf[5]=msg->Can_Msg[msg->Msg_Out].msg.zeros;
					tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);				
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Take_down))  //�һ�
			{
				if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room))
				{							
					client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.head;
					client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
					client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
					client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
					client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
					client_send_buf[5]=msg->Can_Msg[msg->Msg_Out].msg.zeros;
					tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);				
					rt_kprintf("recv end tall from Com_Room\n");
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Rst_Comm))  //������λ
			{
				client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.head;
				client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
				client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
				client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
				client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
				client_send_buf[5]=msg->Can_Msg[msg->Msg_Out].msg.zeros;
				tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);				
				rt_kprintf("recv end tall from Com_Room\n");
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Check_Com))  //�ֶ����
			{
				rt_kprintf("recv end tall from Com_Room\n");
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(ADD_TALK))		 //����ͨ��		
			{
				client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.head;
				client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
				client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
				client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
				client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
				client_send_buf[5]=msg->Can_Msg[msg->Msg_Out].msg.zeros;
				tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
				rt_kprintf("recv add tall\n");
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(TAKE_UP))		 //����ժ���ź�	
			{
				client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.head;
				client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
				client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
				client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
				client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
				client_send_buf[5]=msg->Can_Msg[msg->Msg_Out].msg.zeros;
				tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
				rt_kprintf("recv take up \n");
			}					
			break;
		
		default:
			break;
	}
#endif	
}


int fputc(int ch, FILE* f)
{
    /* send a byte of data to the serial port */
    USART_TxData(USART1, (uint8_t)ch);
    /* wait for the data to be send  */
    while (USART_ReadStatusFlag(USART1, USART_FLAG_TXBE) == RESET);

    return (ch);
}