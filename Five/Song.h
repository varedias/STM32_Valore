#ifndef __Song_H
#define __Song_H

#include "stm32f10x.h"

// ??????(???????)
#define DOL 262
#define REL 294
#define MIL 330
#define FAL 349
#define SOL 392
#define LAL 440
#define SIL 494
//middle pitch
#define DOM 523
#define REM 587
#define MIM 659
#define FAM 698
#define SOM 784
#define LAM 880
#define SIM 988
//high pitch
#define DOH 1046
#define REH 1175
#define MIH 1318
#define FAH 1397
#define SOH 1568
#define LAH 1760
#define SIH 1976
//pause
#define STOPNS 0

// ??????(???????)
#define B41 2000000UL //4?
#define B31 1500000UL //3?
#define B21 1000000UL //2?
#define B32 750000UL  //1.5?
#define B11 500000UL  //1?
#define B34 375000UL  //3/4?
#define B12 250000UL  //1/2?
#define B14 125000UL  


void ASingle(u32 fr, u32 cyus);
void PlayMusic(u32 * p);


u32 music1[]={

    DOM, B12, STOPNS, B14, 
    LAL, B12, STOPNS, B14,
    LAM, B11, STOPNS, B14,
    LAL, B12, STOPNS, B14,
    FAM, B12, STOPNS, B14,
    MIM, B31, STOPNS, B11,
    
  
    STOPNS, 0
};

#endif