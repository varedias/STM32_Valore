#include "stm32f10x.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define BUFFER_SIZE 64

char rxBuffer[BUFFER_SIZE];
uint8_t bufferIndex = 0;
uint8_t expressionComplete = 0;

void AllInit()
{

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);


    GPIO_InitTypeDef g1;
    g1.GPIO_Pin = GPIO_Pin_9;           
    g1.GPIO_Speed = GPIO_Speed_50MHz;
    g1.GPIO_Mode = GPIO_Mode_AF_PP;      
    GPIO_Init(GPIOA, &g1);

    g1.GPIO_Pin = GPIO_Pin_10;           
    g1.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &g1);

   
    USART_InitTypeDef g2;
    USART_StructInit(&g2);             
    g2.USART_BaudRate = 115200;
    g2.USART_StopBits = USART_StopBits_1;
    g2.USART_Parity = USART_Parity_No;
    g2.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &g2);

    USART_Cmd(USART1, ENABLE);         

   
    NVIC_InitTypeDef g3;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
    g3.NVIC_IRQChannel = USART1_IRQn;
    g3.NVIC_IRQChannelPreemptionPriority = 3;
    g3.NVIC_IRQChannelSubPriority = 3;
    g3.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&g3);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 
}


void USART_SendString(USART_TypeDef* USARTx, char *str)
{
    while(*str)
    {
        USART_SendData(USARTx, *str++);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    }
}


void CalculateExpression(char *expr)
{
    int num1 = 0, num2 = 0, result = 0;
    char op = 0;
    char *ptr = expr;

    while(*ptr && !isdigit(*ptr)) ptr++; 
    num1 = atoi(ptr);
    
  
    while(*ptr && isdigit(*ptr)) ptr++;
    op = *ptr;
    ptr++;
    
    num2 = atoi(ptr);

    switch(op)
    {
        case '+':
            result = num1 + num2;
            break;
        case '-':
            result = num1 - num2;
            break;
        case '*':
            result = num1 * num2;
            break;
        case '/':
            if(num2 != 0)
                result = num1 / num2;
            else
                USART_SendString(USART1, "Error: Division by zero\r\n");
            break;
        case '%':
            result = num1 % num2;
            break;
        case '&':
            result = num1 & num2;
            break;
        case '|':
            result = num1 | num2;
            break;
        default:
            USART_SendString(USART1, "Error: Invalid operator\r\n");
            return;
    }
    
    // ??????????
    char response[32];
    sprintf(response, "%d%c%d=%d\r\n", num1, op, num2, result);
    USART_SendString(USART1, response);
}


void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        uint8_t receivedData = USART_ReceiveData(USART1);
        

        if(receivedData == '\r' || receivedData == '\n')
        {
            if(bufferIndex > 0) 
            {
                rxBuffer[bufferIndex] = '\0'; 
                expressionComplete = 1;
                bufferIndex = 0;
            }
        }
        else if(bufferIndex < BUFFER_SIZE - 1) 
        {
            rxBuffer[bufferIndex++] = receivedData;
        }
    }
}

int main()
{
    AllInit();
    
    while(1)
    {
        if(expressionComplete)
        {

            char *questionMark = strstr(rxBuffer, "?");
            if(questionMark != NULL)
            {
                *questionMark = '\0'; 
            }
            
            CalculateExpression(rxBuffer);
            expressionComplete = 0;
        }
    }
}