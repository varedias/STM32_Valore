#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Song.h"



void GP_init()
{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA,ENABLE);
		GPIO_InitTypeDef G;
		G.GPIO_Mode=GPIO_Mode_Out_PP;
		G.GPIO_Pin=GPIO_Pin_6;
		G.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOC,&G);
	
    G.GPIO_Pin = GPIO_Pin_5;
    G.GPIO_Mode = GPIO_Mode_IPU;
    G.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &G);

  
    G.GPIO_Mode = GPIO_Mode_IPU;
    G.GPIO_Pin = GPIO_Pin_15;
    G.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &G);

    G.GPIO_Pin = GPIO_Pin_0; 
    G.GPIO_Mode = GPIO_Mode_IPD; 
    G.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &G);
		GPIOC->ODR &=~(1<<6);

}

int GetState()
{
    
    if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == 0)
    {
        Delay_ms(10);
        return 1;
    } 
    else if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == 0)
    {
        Delay_ms(10);
        return 2;
    }
    else if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)
    {
        Delay_ms(10);
        return 3;
    }
    return 0;

}

void ASingle(u32 fr, u32 cyus)
{
		u32 frus ;
		u32 beatNS;
		if(fr==0)
		{
			GPIOC->BRR = 1<<6;
			Delay_us(cyus);
			return ;
		}
		frus =1000000UL/fr;
		beatNS=cyus/frus;
		while(beatNS)
		{
			GPIOC->BSRR = 1<<6;
			Delay_us(frus/2);
			GPIOC->BRR =1 <<6;
		  Delay_us(frus/2);
			beatNS--;
		}
		
}


void PlayMusic(u32 * p)
{
	u32 i,j;
	while(1)
	{
			i=*p++;
			j=*p++;
		if(i==0&&j==0)break;
		ASingle(i,j);
	
	}
		
}

void Gun()
{
	short t =325;
		for(t=170;t<325;t++)
	{
	 GPIOC->ODR ^= 1<<6;
			Delay_us(t);
	}
	GPIOC->ODR&=~(1<<6) ;
}


void Ambulance()
{
	short i;
	for(i=0;i<1200;i++)
	{
		GPIOC->ODR ^=1<<6;
		Delay_us(250);
	}
	for(i=0;i<900;i++)
	{
		GPIOC->ODR ^=1<<6;
			Delay_us(333);
	}
	GPIOC->BRR = 1<<6;
}


void fire()
{
		short i,t;
		for(t=555;t<833;t++)
	{
		for(i=0;i<(10000/t);i++)
		{
			GPIOC->ODR ^=1<<6;
			Delay_us(t);
		}
	}
		for(t=833;t>555;t--)
		{
			for(i=0;i<(10000/t);i++)
			{
				GPIOC->ODR ^= 1<<6;
				Delay_us(t);
			}
		}
	GPIOC->BRR = 1<<6;
}
	
	


int main()
{
	short i ,j;
	GP_init();

	while(1)
	{
		if(GetState()==1)
		{
				fire();
		}
		if(GetState()==2)
		{
				Gun();
		}
		if(GetState()==3)
		{
			Ambulance();
		}
		
	}


}
