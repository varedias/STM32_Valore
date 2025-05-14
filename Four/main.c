
#include "stm32f10x.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define BUFFER_SIZE 64
#define MAX_RESPONSE_SIZE 32

char rxBuffer[BUFFER_SIZE];
volatile uint8_t bufferIndex = 0;
volatile bool expressionComplete = false;

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

void USART_SendString(USART_TypeDef* USARTx, const char *str)
{
    while(*str)
    {
        USART_SendData(USARTx, *str++);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    }
}

bool ParseExpression(const char *expr, int *num1, char *op, int *num2)
{
    const char *ptr = expr;
    
    while(*ptr && isspace(*ptr)) ptr++;

    if(!isdigit(*ptr)) return false;
    *num1 = atoi(ptr);
    
    while(*ptr && isdigit(*ptr)) ptr++;
    
    
    while(*ptr && isspace(*ptr)) ptr++;
    
    
    if(!(*ptr == '+' || *ptr == '-' || *ptr == '*' || 
         *ptr == '/' || *ptr == '%' || *ptr == '&' || *ptr == '|'))
        return false;
    *op = *ptr++;
    
    
    while(*ptr && isspace(*ptr)) ptr++;
    
    
    if(!isdigit(*ptr)) return false;
    *num2 = atoi(ptr);
    
    return true;
}

void CalculateExpression(const char *expr)
{
    int num1 = 0, num2 = 0, result = 0;
    char op = 0;
    char response[MAX_RESPONSE_SIZE];
    
    if(!ParseExpression(expr, &num1, &op, &num2))
    {
        USART_SendString(USART1, "Error: Invalid expression format\r\n");
        return;
    }
    
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
            if(num2 == 0)
            {
                USART_SendString(USART1, "Error: Division by zero\r\n");
                return;
            }
            result = num1 / num2;
            break;
        case '%':
            if(num2 == 0)
            {
                USART_SendString(USART1, "Error: Modulo by zero\r\n");
                return;
            }
            result = num1 % num2;
            break;
        case '&':
            result = num1 & num2;
            break;
        case '|':
            result = num1 | num2;
            break;
        default:
            USART_SendString(USART1, "Error: Unsupported operator\r\n");
            return;
    }
    
    snprintf(response, MAX_RESPONSE_SIZE, "%d %c %d = %d\r\n", num1, op, num2, result);
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
                expressionComplete = true;
                bufferIndex = 0;
            }
        }

        else if(receivedData == '\b' || receivedData == 0x7F)
        {
            if(bufferIndex > 0)
            {
                bufferIndex--;
            }
        }

        else if(bufferIndex < BUFFER_SIZE - 1 && isprint(receivedData)) 
        {
            rxBuffer[bufferIndex++] = receivedData;
        }

    }
}

int main(void)
{
    AllInit();
    
    USART_SendString(USART1, "Calculator ready. Enter expressions like '12+34' or '5 * 6'.\r\n");
    
    while(1)
    {
        if(expressionComplete)
        {
            char *questionMark = strchr(rxBuffer, '?');
            if(questionMark != NULL)
            {
                *questionMark = '\0';
            }
            
            CalculateExpression(rxBuffer);
            expressionComplete = false;
        }
    }
}

