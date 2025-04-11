#include "stm32f10x.h"
#include "Delay.h"


volatile int State1=0;
volatile int State2=0;
volatile int State3=0;
void Init()
{

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitTypeDef g1;
    g1.GPIO_Pin = GPIO_Pin_8;
    g1.GPIO_Mode = GPIO_Mode_Out_PP;
    g1.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &g1);
    
		g1.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &g1);
    
    g1.GPIO_Pin = GPIO_Pin_5;
    g1.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &g1);
    

    g1.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOA, &g1);
    

    g1.GPIO_Pin = GPIO_Pin_0;
    g1.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOA, &g1);
}




int GetState()
{
    
    if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == 0)//key0
    {
        Delay_ms(10);
        return 1;
    } 
    else if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == 0)//key1
    {
        Delay_ms(10);
        return 2;
    }
    else if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)//wk
    {
        Delay_ms(10);
        return 3;
    }
    return 0;

}


void Torggle(int State)
{
		if(State==1)
		{	
			Delay_ms(10);
			GPIOA->BRR=(1<<8);
			while(1){
			if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)==1)
				break;
			}
		Delay_ms(10);
			GPIOA->BSRR=(1<<8);   
		}
		
		if(State==2)
		{	
			Delay_ms(10);
			GPIOD->BRR=(1<<2);
			while(1){
			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)==1)
				break;
			}
		Delay_ms(10);
			GPIOD->BSRR=(1<<2);   
		}
		
		if(State==3)
			{	
				Delay_ms(10);
				GPIOD->BRR=(1<<2);
				GPIOA->BRR=(1<<8);
			while(1){
			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==0)
				break;
			}
		Delay_ms(10);
			GPIOD->BSRR=(1<<2);   
			GPIOA->BSRR=(1<<8);
		}
			
	
}


void ADDShow(State)
{
		if(State==1)
		{
		    State1++;
			if(State1%2==0)
				GPIOA->BRR=(1<<8);
			else
				GPIOA->BSRR=(1<<8);
			 while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == 0){};
            Delay_ms(20); 
		}
		if(State==2)
		{
			 State2++;
			if(State2%2==0)
				GPIOD->BRR=(1<<2);
			else
				GPIOD->BSRR=(1<<2);
			while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == 0){};
				Delay_ms(20);
		}
		
		if(State==3)
		{
				 State3++;
			if(State3%2==0){
				GPIOD->BRR=(1<<2);
			GPIOA->BRR=(1<<8);}
			else{
				GPIOD->BSRR=(1<<2);
					GPIOA->BSRR=(1<<8);}
			
			while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1){};
				Delay_ms(20);
		}
	
	
}





int main(void)
{
	Init();
	GPIOA->BSRR=(1<<8);
	GPIOD->BSRR=(1<<2);
	int State=0;
	while(1)
	{
		State=GetState();
		ADDShow(State);
	
	}
	
	
	
}
