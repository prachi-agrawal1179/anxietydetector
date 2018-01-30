/*
 * GccApplication2.c
 *
 * Created: 26-01-2018 15:23:47
 * Author : Rajdeep
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/sfr_defs.h>
#include "USART_128.h"


//void USART_Init();
void GSM_Begin();
void GSM_Send_Msg(char *, char *);
void GSM_Msg_Delete(unsigned int);
//void USART_TxChar(char);
//void USART_SendString(char *);

volatile int buffer_pointer;
char Mobile_no[14];
char buff[160];


int main(void)
{
	//let the output from PPG be given to A port
	DDRA = 0b00000000;
	PORTA = 0b00000000;
	USART_Init(51,1);
    GSM_Begin();
	strcpy(Mobile_no,"+91xxxxxxxxxx");
	char sms[] = "High pulse rate detected.";
	while (1)
	{
		if (bit_is_set(PINA,0)) //Set the pattern for high pulse rate
		{
			GSM_Send_Msg(Mobile_no ,sms);
		}
	}
}

/*void USART_Init(){
	
	//Enable receiver and transmitter
	UCSRB = (1<<RXEN) | (1<<TXEN);
	
}*/

void GSM_Begin(){
	while (1)
	{
		//USART_SendString("ATE0\r"); //send ATE0 to check module is ready or not
		USART_TransmitString("ATE0\r",1);
		_delay_ms(500);
		if(strstr(buff,"OK"))
		{
			memset(buff,0,160);
			break;
		}
		
	}
	_delay_ms(1000);
	//Message format is "Text"
	USART_TransmitString("AT+CMGF=1\r",1);
	_delay_ms(1000);
	
}
void GSM_Send_Msg(char *num, char *sms){
	char sms_buffer[35];
	buffer_pointer=0;
	sprintf(sms_buffer,"AT+CMGS=\"%s\"\r",num); // storing AT+CMGS="****7676" in sms_buffer
	USART_TransmitString(sms_buffer,1); //Give that command
	_delay_ms(200);
	while (1)
	{
		if (buff[buffer_pointer] == 0x3D) //wait for  '>' character so that message can be entered
		{
			buffer_pointer = 0;
			memset(buff,0,strlen(buff));
			/* When that sign is received message is sent and ended using ctrl+z*/
			USART_TransmitString(sms,1);
			USART_Transmitchar(0x1A,1); //0x1a is hex code for ctrl+z
			break;
		}
		buffer_pointer++;
	}
	_delay_ms(300);
	buffer_pointer = 0;
	memset(buff,0,strlen(buff)); //free up 
	memset(sms_buffer,0,strlen(sms_buffer));
}

void GSM_Msg_Delete(unsigned int position){
	buffer_pointer = 0;
	char del_cmd[20];
	sprintf(del_cmd, "AT+CMGD=%d\r", position);
	USART_TransmitString(del_cmd,1);
}
 /*void USART_TxChar(char data){
	 while(!(UCSRA & (1<<UDRE)));
	 UDR = data;
 }*/
 
 /*void USART_SendString(char *str){
	 int i = 0;
	 while (str[i] != '\0')
	 {
		 USART_TxChar(str[i]);
		 i++;
	 }
 }*/