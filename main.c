// Include main header

#include "stm32f4xx.h"
#include "fonts.h"
#include "time.h"

// Include BSP Headers

#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_sdram.h"
#include "stm32f429i_discovery_ioe.h"


//Les énumerations

	 typedef enum etatMaf_Ascenseur etatMaf_Ascenseur;

	 enum etatMaf_Ascenseur {attente_ascenseur,a0,a1,p26};

	 typedef enum etatMaf_Tapis etatMaf_Tapis;

	 enum etatMaf_Tapis{attente,a3,a31};

	 typedef enum etatMaf_Verin etatMaf_Verin;

	 enum  etatMaf_Verin{p2,a4,a41};

	 typedef enum etatMaf_barriere etatMaf_barriere;

	 enum  etatMaf_barriere{attente_barriere,a2 };

	 typedef enum etatMaf_evacuer etatMaf_evacuer;

	 enum  etatMaf_evacuer{attente3,a6,a7,a5,p1,a61};







int main(void)
{

//Les synchronisations et les nano-mémoires

 uint8_t syn_verin_ferme=0;
 uint8_t syn_trap_retour=0;
 uint8_t syn_ferme_barriere=0;
 int sync2=0;
 int sync=0;
 int sync3=0;
 int sync_carton=0;
 int sync_ouverture=0;
 int p27=1;
 int p28=0;
 int milieu=1;
 int bas=0;
 uint8_t A0,A1,A2,A3,A4,A5,A6,A7;

 //Les port A,B et C

 uint8_t Port_C;
 uint8_t Port_A;
 uint8_t Port_B;
 uint16_t sensors=0;

#define c0  1
#define c1  2
#define c2  4
#define c3  8
#define c4  16
#define c5  32
#define c6  64
#define c7  128
#define c8  256
#define c9  512
#define c10 1024




 // Turn on Green LED

 	STM_EVAL_LEDInit(LED3);
 	STM_EVAL_LEDOn(LED3);

// Display welcome message

	LCD_Init();					  	// Start & Init LCD
	LCD_LayerInit();
	LTDC_Cmd(ENABLE);
	LCD_SetLayer(LCD_FOREGROUND_LAYER);

	LCD_Clear(0xFFFF);
	LCD_SetTextColor(0x0000);
	LCD_SetFont(&Font16x24);
	LCD_DisplayStringLine(50, (uint8_t*) " Hello World !");

	LCD_SetFont(&Font12x12);
	LCD_SetTextColor(0xF000);
	LCD_DisplayStringLine(100, (uint8_t*) "  Welcome to the");
	LCD_DisplayStringLine(115, (uint8_t*) "  STM32F429");
	LCD_DisplayStringLine(130, (uint8_t*) "  Discovery Board");
	etatMaf_Ascenseur etat=a0;
	etatMaf_Tapis etat1=attente;
	etatMaf_Verin etat2=p2;
	etatMaf_barriere etat3=attente_barriere;
	etatMaf_evacuer etat4=attente3;


	while(1)

    {
// LECTURE DS CAPTEURS

	// Lire les Ports A et C


		sensors = I2C_ReadPCFRegister(PCF_C_READ_ADDR);
				sensors = sensors<<8;
				sensors = sensors + I2C_ReadPCFRegister(PCF_A_READ_ADDR);



// EVOLUTION DES RDP

    //Evolution de l'ascenseur
	switch(etat){

	case a0:
		if ((sensors & c0) == c0) { etat = a1; }
		break;
	case a1:
		if ((sensors & c1) == c1) { etat = p26; }
		break;
	case p26:
		if(p27 && (sensors & c2) == c2) {etat=a0;
		p27=0;
		p28=1;
		}

		else if(p28)
		{etat=attente_ascenseur;
		sync_ouverture=1;
		p27=1;
		p28=0;
		}
		break;

	case attente_ascenseur:

			if(sync_carton)
				{etat=a0;
				sync_carton=0;}
				break;

	}



	//Evolution du tapis

	switch(etat1)
	{



	case attente:
		if(((sensors & c2) == c2) && sync_ouverture==1) {
			etat1=a3;
			sync=1;

		sync_ouverture=0;}
		break;
	case a3:
		if((sensors & c4) == c4)
			{etat1=a31;

		syn_verin_ferme=1;}
		break;
	case a31:
		if(syn_trap_retour)
			{etat1=attente;
		syn_trap_retour=0;
		syn_ferme_barriere=1;}
		break;
	}


	//Evolution verin
	switch(etat2){
	case p2:
		if(syn_verin_ferme)
		{ etat2=a4;
		syn_verin_ferme=0;}
		break;
	case a4:
		if ((sensors & c5) == c5) {
			etat2 = a41;
			syn_trap_retour = 1;

		}
		break;
	case a41:
		if ((sensors & c3) == c3 && sync3) {
			etat2 = p2;
			sync_carton = 1;
		}
		break;

	}


	   //Evolution barriere

	switch(etat3){
	case attente_barriere:
		if(((sensors & c2) == c2) && sync==1){ etat3=a2;
		sync=0;}
		break;
	case a2:
		if(syn_ferme_barriere)
		{etat3=attente_barriere;
		sync2 = 1;
		syn_ferme_barriere=0;}
		break;

	}

     //	Evolution de l'evacuation

  switch(etat4){
	case attente3:
		if (sync2)
		{ etat4 = a6;
		sync2 = 0; }
		break;
	case a6:
		if ((sensors & c6) == c6) { etat4 = a7; }
		break;
	case a7:
		if ((sensors & c10) == c10) { etat4 = a5; }
		break;
	case a5:
		if((sensors & c7) == c7)
		{etat4=a61;
		sync3=1;}
		break;
	case p1:
		if(sync2) {etat4=a61;}
		break;
	case a61:
		if(((sensors & c8) == c8) && milieu==1)

		{
		sync3=1;
		milieu=0;
		bas=1;
		etat4=p1;}

		else if(((sensors & c9) == c9) && bas==1)

		{etat4=attente3;
		milieu=1;
		bas=0;
		sync3=1;
		sync2=0;}

		break;

	}


// ECRITURE DES ACTIONNEURS

	// calculer les actions Ai
A0=(etat==a0);
A1=(etat==a1);
A2=(etat3==a2);
A3=(etat1==a3 || etat1==a31);
A4=(etat2==a4 || etat2==a41);
A5=(etat4==a5);
A6=(etat4==a6 || etat4==a61);
A7 = (etat4 == a7);
Port_B=A0*1+A1*2+A2*4+A3*8+A4*16+A5*32+A6*64+A7*128;

	// ecrire sur le PortB
I2C_WritePCFRegister(PCF_B_WRITE_ADDR,Port_B);


    }
}
