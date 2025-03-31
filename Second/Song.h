/************************
??(??)??,??Hz
*************************/
//low pitch
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

/*********************************
拍数，节拍常数，单位 us/每分钟120拍
**********************************/
#define B41 2000000UL 
//4拍
#define B31 1500000UL 
//3拍
#define B21 1000000UL 
//2拍
#define B32 750000UL 
//1.5拍
#define B11 500000UL 
//1拍
#define B34 375000UL 
//4分之3拍
#define B12 250000UL 
//2分之1拍
#define B14 125000UL 
//4分之1拍

u32 music1[]=
{
LAM,B11,LAM,B11,LAM,B11,LAM,B14,DOH,B14,REH,B14,MIH,B14,
LAM,B11,LAM,B11,LAM,B12,SOM,B12,SOM,B12,LAM,B12,
LAM,B11,LAM,B11,LAM,B11,LAM,B14,DOH,B14,REH,B14,MIH,B14,
LAM,B11,LAM,B11,LAM,B12,FAH,B12,FAH,B12,MIH,B12,
LAM,B11,LAM,B11,LAM,B11,LAM,B14,DOH,B14,REH,B14,MIH,B14,
LAM,B11,LAM,B11,LAM,B12,LAM,B12,SOM,B12,LAM,B12,
LAM,B11,LAM,B11,LAM,B14,LAM,B14,DOH,B14,REH,B14,MIH,B14,
LAM,B32,SOM,B12,SOM,B12,LAM,B12,LAM,B11,
STOPNS,B32,
LAM,B12,LAM,B12,SOM,B14,SOM,B14,SOM,B12,LAM,B12,
SOM,B12,MIM,B12,MIM,B11,SOM,B14,MIM,B21,
STOPNS,B32,
LAM,B12,LAM,B12,SOM,B12,LAM,B12,SIM,B12,
DOH,B11,SIM,B11,LAM,B12,SIM,B14,LAM,B14,SOM,B11,0,0
};

u32 see_you_again[] = {

    DOH, B12, REH, B12, MIH, B11, DOH, B12,
    REH, B12, MIH, B12, FAH, B11, MIH, B12,
    DOH, B12, REH, B12, MIH, B11, DOH, B12,
    SIL, B12, DOH, B12, REH, B11,


    MIH, B11, MIH, B12, FAH, B12, SOH, B11,
    LAH, B11, SOH, B12, FAH, B12, MIH, B11,
    DOH, B11, DOH, B12, REH, B12, MIH, B11,
    FAH, B11, MIH, B12, REH, B12, DOH, B11,


    DOH, B12, REH, B12, MIH, B11, FAH, B12,
    SOH, B11, LAH, B12, SOH, B12, FAH, B11,
    MIH, B11, FAH, B12, SOH, B12, LAH, B11,
    SOH, B11, FAH, B12, MIH, B12, REH, B11,


    DOH, B12, REH, B12, MIH, B11, DOH, B12,
    REH, B12, MIH, B12, FAH, B11, SOH, B12,
    LAH, B11, SIH, B12, LAH, B12, SOH, B11,
    FAH, B11, MIH, B12, REH, B12, DOH, B11,


    MIH, B32, STOPNS, B32,
    0, 0  
};


