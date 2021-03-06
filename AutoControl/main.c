#include "stm32f4xx.h"
#include "stm32f4xx_rtc.h"

#include "initPA1.h"
#include "initButton.h"
#include "initRTC.h"
#include "customChar.h"

#include "tm_lib/tm_stm32f4_delay.h"
#include "tm_lib/tm_stm32f4_hd44780.h"

#include "DHT11/dht11a.h"

enum Stan {Rozpoczecie,UstawianieGodziny, UstawianieMinuty,UstawianieRoku, UstawianieMiesiaca, UstawianieDnia, ZapisCzasu, Normalny, Obroty, Zdarzenie };
uint8_t nrZbocza = 0;
int czasImpulsu = 0;




typedef struct Layer{
	char buffor[2][16];
	int height;
	int x,y;
};

typedef struct{
	char LCDbuffor[2][16];
	struct Layer layer[3];
}Ekran;

void appendLayer(Ekran *e){
	int i;
	for(i=0;i < 3;i++){
		strcpy(e->LCDbuffor[0],e->layer[i].buffor[0]);
		strcpy(e->LCDbuffor[1],e->layer[i].buffor[1]);
	}
}

typedef struct{
	enum Stan aktualnyStan;
	enum Stan ostatniStan;
	uint16_t obroty;
	uint8_t tempeIn, tempOut;
	uint8_t humidity;
	uint8_t ChkSum;
    RTC_TimeTypeDef time;
    RTC_DateTypeDef data;
    uint8_t hour, minute;
    uint16_t year;
    uint8_t month, date;
    uint8_t leweDrzwi, praweDrzwi, bagaznik;
}CarKomputer;

CarKomputer komp;
Ekran ekran;

void initKomputer(CarKomputer * komp){
	komp->year=15;
	komp->aktualnyStan=Rozpoczecie;
}

void show(CarKomputer * komp){
	int i,j;
	char buffer[5];
	char buffer1[10];
	char buffer2[10];
	char buffer3[16];
	char buffer4[10];
	char buffer5[10];
	char buffer6[10];
	char bufferTempOut[5];
	char logoBuf[16] = "AutoControl v1";
	sprintf(buffer,"%d",komp->obroty);
	sprintf(buffer1,"%d %d %d",komp->data.RTC_Year+2000, komp->data.RTC_Month,komp->data.RTC_Date);
	sprintf(buffer2,"%02d:%02d",komp->time.RTC_Hours,komp->time.RTC_Minutes);
	sprintf(bufferTempOut,"%d",komp->tempOut);

	TM_HD44780_Clear();

	switch(komp->aktualnyStan){
	case Rozpoczecie:
		for(j=1;j<=strlen(logoBuf);j++){
			strncpy(buffer3,logoBuf,j);
			Delayms(25);
			TM_HD44780_Puts(0,0,buffer3);

		}
		break;
	case UstawianieGodziny:
		sprintf(buffer4,"Godzina: %d",komp->hour);
		TM_HD44780_Puts(0,0,buffer4);
		break;
	case UstawianieMinuty:
		sprintf(buffer5,"Minuty: %d",komp->minute);
		TM_HD44780_Puts(0,0,buffer5);
		break;
	case ZapisCzasu:
		TM_HD44780_Puts(0,0,"Ustawiono date");
		break;
	case UstawianieRoku:
		sprintf(buffer6,"%d %d %d",komp->year,komp->month,komp->date);
		TM_HD44780_Puts(0,0,buffer6);
		TM_HD44780_Puts(0,1,"Ustaw date");
		break;
	case UstawianieMiesiaca:
		sprintf(buffer6,"%d %d %d",komp->year,komp->month,komp->date);
		TM_HD44780_Puts(0,0,buffer6);
		TM_HD44780_Puts(0,1,"Ustaw date");
		break;
	case UstawianieDnia:
		sprintf(buffer6,"%d %d %d",komp->year,komp->month,komp->date);
		TM_HD44780_Puts(0,0,buffer6);
		TM_HD44780_Puts(0,1,"Ustaw date");
		break;
	case Normalny:
		TM_HD44780_Puts(0,0,buffer2);
		TM_HD44780_Puts(6,0,buffer1);
		TM_HD44780_Puts(1,1,bufferTempOut);
		TM_HD44780_PutCustom(0,1,0);
		TM_HD44780_PutCustom(4,1,1);

		break;
	case Obroty:
		TM_HD44780_Puts(0,0,buffer);
		TM_HD44780_Puts(6,0,"RPM");
		for(i= 0; i < komp->obroty/1000 ;i++){
			TM_HD44780_CursorSet(i,1);
			TM_HD44780_Data(0xFF);
		}
		break;
	case Zdarzenie:
		TM_HD44780_PutCustom(0,0,2);
		TM_HD44780_PutCustom(0,1,3);
		TM_HD44780_PutCustom(1,0,4);
		TM_HD44780_PutCustom(1,1,5);
		if(komp->leweDrzwi == 1){
			TM_HD44780_PutCustom(0,0,6);
			TM_HD44780_PutCustom(0,1,7);
			TM_HD44780_Puts(4,0,"Lewe");
			TM_HD44780_Puts(4,1,"drzwi");
		}
		if(komp->praweDrzwi == 1){
			//TM_HD44780_PutCustom(1,0,17);
			//TM_HD44780_PutCustom(1,1,18);
			TM_HD44780_Puts(4,0,"Prawe");
			TM_HD44780_Puts(4,1,"drzwi");
		}
		if(komp->bagaznik == 1){
			TM_HD44780_Puts(4,0,"Bagaznik");
			TM_HD44780_Puts(4,1,"Otwarty");
		}
		break;
	}


}


int getRPM(int czasImpulsu){
	float frequency = 64000/czasImpulsu;
	return frequency*60;
}





/* Set interrupt handlers */
/* Handle PD0 interrupt */
void EXTI1_IRQHandler(void) {
    /* Make sure that interrupt flag is set */
    if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
        /* Do your stuff when PD0 is changed */

    	if(nrZbocza==0){
    		TIM2->CNT = 0;
    		TIM_Cmd(TIM2, ENABLE);
    		nrZbocza=1;
    	}else{
    		czasImpulsu= TIM2->CNT;
    		TIM_Cmd(TIM2, DISABLE);
    		nrZbocza=0;
    	}

        /* Clear interrupt flag */
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

void EXTI0_IRQHandler(void) {
    /* Make sure that interrupt flag is set */
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
        /* Do your stuff when PD0 is changed */



    		switch(komp.aktualnyStan){
    		case Rozpoczecie:
    			komp.aktualnyStan = UstawianieGodziny;
    			break;
    		case UstawianieGodziny:
    			komp.aktualnyStan = UstawianieMinuty;
    			break;
    		case UstawianieMinuty:
    			komp.aktualnyStan = UstawianieRoku;
    			break;
    		case UstawianieRoku:
    			komp.aktualnyStan = UstawianieMiesiaca;
    			break;
    		case UstawianieMiesiaca:
    			komp.aktualnyStan = UstawianieDnia;
    			break;
    		case UstawianieDnia:
    			komp.aktualnyStan = ZapisCzasu;
    			break;
    		case ZapisCzasu:
    			komp.aktualnyStan = Normalny;
    			setTime(komp.hour,komp.minute);
    			setDate(komp.year,komp.month,komp.date);
    			break;
    		case Normalny:
    			komp.aktualnyStan = Obroty;
    			break;
    		case Obroty:
    			komp.aktualnyStan = Normalny;
    			break;
    		case Zdarzenie:
    			komp.aktualnyStan = komp.ostatniStan;
    			break;
    		}

        /* Clear interrupt flag */
        EXTI_ClearITPendingBit(EXTI_Line0);
    }

}


void EXTI3_IRQHandler(void) {
    /* Make sure that interrupt flag is set */
    if (EXTI_GetITStatus(EXTI_Line3) != RESET) {
        /* Do your stuff when PD0 is changed */

    	if(komp.aktualnyStan == UstawianieGodziny){
    		if(komp.hour <24)
    			komp.hour++;
    		else komp.hour = 0;
    	}

    	if(komp.aktualnyStan == UstawianieMinuty){
    		if(komp.minute < 60)
    			komp.minute++;
    		else komp.minute = 0;
    	}

    	if(komp.aktualnyStan == UstawianieRoku){
    		komp.year++;

    	}

    	if(komp.aktualnyStan == UstawianieMiesiaca){
    		if(komp.month < 12)
    			komp.month++;
    		else komp.month = 0;
    	}
    	if(komp.aktualnyStan == UstawianieDnia){
    		if(komp.date < 31)
    			komp.date++;
    		else komp.date = 0;
    	}
    }

    /* Clear interrupt flag */
    EXTI_ClearITPendingBit(EXTI_Line3);

}

//------------przerwania od drzwi i bagaznika

void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line6) != RESET)
    {
		if(komp.leweDrzwi == 0){
			komp.leweDrzwi=1;
			komp.ostatniStan = komp.aktualnyStan;
			komp.aktualnyStan = Zdarzenie;
		}
		else {
			komp.leweDrzwi=0;
			komp.aktualnyStan = komp.ostatniStan;
		}

		EXTI_ClearITPendingBit(EXTI_Line6);

    }

	if(EXTI_GetITStatus(EXTI_Line7) != RESET)
    {
		if(komp.praweDrzwi == 0){
			komp.praweDrzwi=1;
			komp.ostatniStan = komp.aktualnyStan;
			komp.aktualnyStan = Zdarzenie;
		}
		else {
			komp.praweDrzwi=0;
			komp.aktualnyStan = komp.ostatniStan;
		}

		EXTI_ClearITPendingBit(EXTI_Line7);

    }

	if(EXTI_GetITStatus(EXTI_Line8) != RESET)
    {
		if(komp.bagaznik == 0){
			komp.bagaznik=1;
			komp.ostatniStan = komp.aktualnyStan;
			komp.aktualnyStan = Zdarzenie;
		}
		else {
			komp.bagaznik=0;
			komp.aktualnyStan = komp.ostatniStan;
		}

		EXTI_ClearITPendingBit(EXTI_Line8);

    }
}




int main(void)
{
	SystemInit();
	Configure_PA1();
	initButton();
	initButtonPE3();
	initButtonPC6();
	initButtonPC7();
	initButtonPC8();
	RTC_Configuration();
	initTIM2();
	dhtTim3Init();
	initKomputer(&komp);

	TM_HD44780_Init(16,2);

	TM_HD44780_CreateChar(0,&charTermo[0]);
	TM_HD44780_CreateChar(1,&charCelc[0]);
	TM_HD44780_CreateChar(2,&carL0[0]);
	TM_HD44780_CreateChar(3,&carL1[0]);
	TM_HD44780_CreateChar(4,&carR0[0]);
	TM_HD44780_CreateChar(5,&carR1[0]);
	TM_HD44780_CreateChar(6,&carLeft0Open[0]);
	TM_HD44780_CreateChar(7,&carLeft1Open[0]);
	//TM_HD44780_CreateChar(17,&carR0Open[0]);
	//TM_HD44780_CreateChar(18,&carR1Open[0]);

	komp.aktualnyStan = Rozpoczecie;
	show(&komp);
	Delayms(500);
	komp.aktualnyStan = UstawianieGodziny;

	int i = 500;
	show(&komp);
    while(1)
    {

    	komp.obroty = getRPM(czasImpulsu);
    	RTC_GetDate(RTC_Format_BIN,&komp.data);
    	RTC_GetTime(RTC_Format_BIN,&komp.time);
    	if(i>500){
    		dhtRead(&komp.humidity,&komp.tempOut,&komp.ChkSum);
    		i=0;
    	}
    	i++;
    	show(&komp);
    	Delayms(10);
    }

}
