#include "stm32f10x.h"
#include "Init.h"
#include "Song.h"
#include <stdio.h>
#include <string.h>

#pragma import(__use_no_semihosting)


typedef enum {
    SUNDAY = 0,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY
} Weekday;


typedef struct {
    uint8_t enable;         
    uint16_t minutes;        
    uint32_t startTick;       
} Alarm_TimeTypeDef;

struct __FILE { int handle;  };
FILE __stdout;
FILE __stdin;


#define RX_BUFFER_SIZE 64
char rxBuffer[RX_BUFFER_SIZE];
uint8_t rxIndex = 0;
uint8_t rxComplete = 0;

uint32_t systemTick = 0;   
Alarm_TimeTypeDef alarm;    

void _sys_exit(int x) {
    x = x; 
}


typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;
} RTC_TimeTypeDef;

RTC_TimeTypeDef currentTime;


Weekday GetWeekday(uint16_t year, uint8_t month, uint8_t day) {
    if (month < 3) {
        month += 12;
        year--;
    }
    uint16_t c = year / 100;
    year = year % 100;
    uint8_t w = (day + 2*month + 3*(month+1)/5 + year + year/4 - c/4) % 7;
    return (Weekday)w;
}


const char* WeekdayToString(Weekday weekday) {
    static const char* weekdays[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
    return weekdays[weekday];
}


void Alarm_Init(void) {
    alarm.enable = 0;
    alarm.minutes = 0;
    alarm.startTick = 0;
    printf("Alarm module initialized\r\n");
}


void Alarm_SetTime(uint16_t mins) {
    if (mins == 0) {          
        alarm.enable = 0;
        printf("Alarm disabled\r\n");
        return;
    }
    alarm.enable = 1;
    alarm.minutes = mins;
    alarm.startTick = systemTick;  
    printf("Alarm set to %d minutes\r\n", mins);
}


void Alarm_Check(void) {
    static uint8_t isPlaying = 0;
    
    if (alarm.enable) {
        uint32_t elapsedTicks = systemTick - alarm.startTick;
        if (elapsedTicks >= alarm.minutes * 60) {
            alarm.enable = 0;
            isPlaying = 1;
            PlayMusic(music1); 
        }
    }
 
    
}

void TIM3_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = 999; 
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM3, ENABLE);
}

void uart_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART1, ENABLE);
}

int fputc(int ch, FILE *f) {
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, (uint8_t)ch);
    return ch;
}

void RTC_SetTime(uint16_t year, uint8_t month, uint8_t day, 
                uint8_t hour, uint8_t minute, uint8_t second, uint16_t millisecond) {
    currentTime.year = year;
    currentTime.month = month;
    currentTime.day = day;
    currentTime.hour = hour;
    currentTime.minute = minute;
    currentTime.second = second;
    currentTime.millisecond = millisecond;
}

uint8_t IsLeapYear(uint16_t year) {
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

uint8_t GetMonthDays(uint16_t year, uint8_t month) {
    static uint8_t days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && IsLeapYear(year))
        return 29;
    return days[month - 1];
}


void ParseTimeCommand(char* command) {
    if (command[0] == '>') {  
        
        if (strlen(command) < 23) {
            printf("Invalid command format\r\n");
            return;
        }
        
        uint16_t year;
        uint8_t month, day, hour, minute, second;
        uint16_t millisecond;
        
        
        if (sscanf(command + 1, "%hu-%hhu-%hhu %hhu:%hhu:%hhu.%hu", 
                  &year, &month, &day, &hour, &minute, &second, &millisecond) != 7) {
            printf("Invalid time format\r\n");
            return;
        }
        
        
        if (month < 1 || month > 12 || 
            day < 1 || day > GetMonthDays(year, month) ||
            hour > 23 || minute > 59 || second > 59 ||
            millisecond > 999) {
            printf("Invalid date or time\r\n");
            return;
        }
        
        
        RTC_SetTime(year, month, day, hour, minute, second, millisecond);
        printf("Time set to: %04d-%02d-%02d %02d:%02d:%02d.%03d\r\n",
               year, month, day, hour, minute, second, millisecond);
    } 
    else if (strncmp(command, "ALARM=", 6) == 0) {  
        uint16_t mins;
        if (sscanf(command + 6, "%hu", &mins) == 1) {
            Alarm_SetTime(mins);
        } else {
            printf("Invalid alarm format! Use: ALARM=<minutes>\r\n");
        }
    }
    else {
        printf("Unknown command\r\n");
        printf("Supported commands:\r\n");
        printf(">yyyy-mm-dd hh:mm:ss.ms  - Set time\r\n");
        printf("ALARM=<minutes>          - Set alarm (0 to cancel)\r\n");
    }
}

void TIM3_IRQHandler(void) {
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        
        currentTime.millisecond++;
        if (currentTime.millisecond >= 1000) {
            currentTime.millisecond = 0;
            currentTime.second++;
            systemTick++;  
            
            if (currentTime.second >= 60) {
                currentTime.second = 0;
                currentTime.minute++;
                
                if (currentTime.minute >= 60) {
                    currentTime.minute = 0;
                    currentTime.hour++;
                    
                    if (currentTime.hour >= 24) {
                        currentTime.hour = 0;
                        currentTime.day++;
                        
                        if (currentTime.day > GetMonthDays(currentTime.year, currentTime.month)) {
                            currentTime.day = 1;
                            currentTime.month++;
                            
                            if (currentTime.month > 12) {
                                currentTime.month = 1;
                                currentTime.year++;
                            }
                        }
                    }
                }
            }
        }
    }
}

void USART1_IRQHandler(void) {
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        
        char ch = USART_ReceiveData(USART1);

        if (ch == '\r' || rxIndex >= RX_BUFFER_SIZE - 1) {
            
            rxBuffer[rxIndex] = '\0';
            rxComplete = 1;
            rxIndex = 0;
        } else if (ch != '\n') {
       
            rxBuffer[rxIndex++] = ch;
        }
    }
}

int main(void) {
    TIM3_Init();
    uart_init();
    Alarm_Init();  
    
    
    RTC_SetTime(2025, 5, 28, 16, 0, 1, 1);
    
    printf("Time & Alarm module initialized\r\n");
    printf("Supported commands:\r\n");
    printf(">yyyy-mm-dd hh:mm:ss.ms  - Set time\r\n");
    printf("ALARM=<minutes>          - Set alarm (0 to cancel)\r\n");
    
    while (1) {
     
        if (rxComplete) {
            rxComplete = 0;
            ParseTimeCommand(rxBuffer);
        }
        
        Weekday weekday = GetWeekday(currentTime.year, currentTime.month, currentTime.day);
        printf("%04d-%02d-%02d,%s,%02d:%02d:%02d.%03d\r\n",
               currentTime.year, currentTime.month, currentTime.day,
               WeekdayToString(weekday),
               currentTime.hour, currentTime.minute, currentTime.second,
               currentTime.millisecond);
        
       
        Alarm_Check();
        
     
        for(volatile int i = 0; i < 500000; i++);
    }
}