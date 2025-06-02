#include "stm32f10x.h"
#include "Delay.h"





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

