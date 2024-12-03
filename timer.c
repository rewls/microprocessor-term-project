#include "device_registers.h"            /* include peripheral declarations S32K144 */
#include "clocks_and_modes.h"
int lpit0_ch0_flag_counter = 0; /*< LPIT0 timeout counter */
int lpit0_ch1_flag_counter = 0; /*< LPIT0 timeout counter */

unsigned int num0,num1,num2,num3 =0;
int min, sec;

unsigned int j=0; /*FND select pin index */

unsigned int FND_DATA[10]={0x7E, 0x0C,0xB6,0x9E,0xCC,0xDA,0xFA,0x4E,0xFE,0xCE};// 0~9 number
unsigned int Delaytime = 0; /* Delay Time Setting Variable*/

unsigned int FND_SEL[4]={0x0100,0x0200,0x0400,0x0800};

void NVIC_init_IRQs(void)
{
	/*LPIT ch0 overflow set*/
	S32_NVIC->ICPR[1] = 1 << (48 % 32);
	S32_NVIC->ISER[1] = 1 << (48 % 32);
	S32_NVIC->IP[48] = 0x00;
	/*LPIT ch1 overflow set*/
	S32_NVIC->ICPR[1] = 1 << (49 % 32);
	S32_NVIC->ISER[1] = 1 << (49 % 32);
	S32_NVIC->IP[49] = 0x0B;
}

void PORT_init (void)
{
	/*
	 * ===============PORT-D SEGMENT=====================
	 */
	PCC-> PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock for PORT D */

	PTD->PDDR |= (1<<1);   /* Port D1: Data Direction= input (default) */
	PTD->PDDR |= (1<<2);   /* Port D2: Data Direction= input (default) */
	PTD->PDDR |= (1<<3);   /* Port D3: Data Direction= input (default) */
	PTD->PDDR |= (1<<4);   /* Port D4: Data Direction= input (default) */
	PTD->PDDR |= (1<<5);   /* Port D5: Data Direction= input (default) */
	PTD->PDDR |= (1<<6);   /* Port D6: Data Direction= input (default) */
	PTD->PDDR |= (1<<7);   /* Port D7: Data Direction= input (default) */

	PORTD->PCR[1] = PORT_PCR_MUX(1); /* Port D1: MUX = GPIO */
	PORTD->PCR[2] = PORT_PCR_MUX(1); /* Port D2: MUX = GPIO */
	PORTD->PCR[3] = PORT_PCR_MUX(1); /* Port D3: MUX = GPIO */
	PORTD->PCR[4] = PORT_PCR_MUX(1); /* Port D4: MUX = GPIO */
	PORTD->PCR[5] = PORT_PCR_MUX(1); /* Port D5: MUX = GPIO */
	PORTD->PCR[6] = PORT_PCR_MUX(1); /* Port D6: MUX = GPIO */
	PORTD->PCR[7] = PORT_PCR_MUX(1); /* Port D7: MUX = GPIO */

	PTD->PDDR |= 1<<8|1<<9|1<<10|1<<11;
	PORTD->PCR[8] = PORT_PCR_MUX(1); /* Port D8: MUX = GPIO */
	PORTD->PCR[9] = PORT_PCR_MUX(1); /* Port D9: MUX = GPIO */
	PORTD->PCR[10] = PORT_PCR_MUX(1); /* Port D10: MUX = GPIO */
	PORTD->PCR[11] = PORT_PCR_MUX(1); /* Port D11: MUX = GPIO */
}

void WDOG_disable (void)
{
	WDOG->CNT=0xD928C520;     /* Unlock watchdog       */
	WDOG->TOVAL=0x0000FFFF;   /* Maximum timeout value    */
	WDOG->CS = 0x00002100;    /* Disable watchdog       */
}

void LPIT0_init (uint32_t Dtime)
{
	/*!
	 * LPIT Clocking:
	 * ==============================
	 */
	PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);    /* Clock Src = 6 (SPLL2_DIV2_CLK)*/
	PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clk to LPIT0 regs 		*/
	/*!
	 * LPIT Initialization:
	 */
	LPIT0->MCR = 0x00000001;  /* DBG_EN-0: Timer chans stop in Debug mode */
	/* DOZE_EN=0: Timer chans are stopped in DOZE mode */
	/* SW_RST=0: SW reset does not reset timer chans, regs */
	/* M_CEN=1: enable module clk (allows writing other LPIT0 regs) */
	LPIT0->MIER = 0x07;  /* TIE0=1: Timer Interrupt Enabled fot Chan 0,1,2 */

	LPIT0->TMR[0].TVAL = 40000000;      /* Chan 0 Timeout period: 40M clocks */
	LPIT0->TMR[0].TCTRL = 0x00000001;
	/* T_EN=1: Timer channel is enabled */
	/* CHAIN=0: channel chaining is disabled */
	/* MODE=0: 32 periodic counter mode */
	/* TSOT=0: Timer decrements immediately based on restart */
	/* TSOI=0: Timer does not stop after timeout */
	/* TROT=0 Timer will not reload on trigger */
	/* TRG_SRC=0: External trigger soruce */
	/* TRG_SEL=0: Timer chan 0 trigger source is selected*/

	LPIT0->TMR[1].TVAL = Dtime* 40000;      /* Chan 1 Timeout period: 40M clocks */
	LPIT0->TMR[1].TCTRL = 0x00000001;
	/* T_EN=1: Timer channel is enabled */
	/* CHAIN=0: channel chaining is disabled */
	/* MODE=0: 32 periodic counter mode */
	/* TSOT=0: Timer decrements immediately based on restart */
	/* TSOI=0: Timer does not stop after timeout */
	/* TROT=0 Timer will not reload on trigger */
	/* TRG_SRC=0: External trigger soruce */
	/* TRG_SEL=0: Timer chan 0 trigger source is selected*/

	LPIT0->TMR[2].TVAL = Dtime* 40000;      /* Chan 1 Timeout period: 40M clocks */
	LPIT0->TMR[2].TCTRL = 0x00000001;
}

void LPIT0_init_delay(uint32_t Delay_time)
{
	LPIT0->TMR[2].TVAL = Delay_time* 40000;      /* Chan 1 Timeout period: 40M clocks */
	LPIT0->TMR[2].TCTRL = 0x00000001;
}

void delay_ms (volatile int us)
{
	LPIT0_init_delay(us);           /* Initialize PIT0 for 1 second timeout  */
	while (0 == (LPIT0->MSR &   LPIT_MSR_TIF2_MASK))
		lpit0_ch1_flag_counter++;         /* Increment LPIT0 timeout counter */
	LPIT0->MSR |= LPIT_MSR_TIF2_MASK;;//............LPIT_MSR_TIF0_MASK; /* Clear LPIT0 timer flag 0 */
}

int Seg_out(int min, int sec)
{
	Delaytime = 1;

	num3 = (min / 10) % 10;
	num2 = min % 10;
	num1 = (sec / 10) % 10;
	num0 = sec % 10;

	if (num1 > 5)
		return 1;

	PTD->PSOR = FND_SEL[j];
	PTD->PCOR =0x7f;
	PTD->PSOR = FND_DATA[num3];
	delay_ms(Delaytime);
	PTD->PCOR = 0xfff;
	j++;

	PTD->PSOR = FND_SEL[j];
	PTD->PCOR =0x7f;
	PTD->PSOR = FND_DATA[num2];
	delay_ms(Delaytime);
	PTD->PCOR = 0xfff;
	j++;

	PTD->PSOR = FND_SEL[j];
	PTD->PCOR =0x7f;
	PTD->PSOR = FND_DATA[num1];
	delay_ms(Delaytime);
	PTD->PCOR = 0xfff;
	j++;

	PTD->PSOR = FND_SEL[j];
	PTD->PCOR =0x7f;
	PTD->PSOR = FND_DATA[num0];
	delay_ms(Delaytime);
	PTD->PCOR = 0xfff;
	j=0;
	return 0;
}

/* delay counter */
void LPIT0_Ch1_IRQHandler (void)
{
	lpit0_ch1_flag_counter++;         /* Increment LPIT1 timeout counter */
	LPIT0->MSR |= LPIT_MSR_TIF1_MASK;  /* Clear LPIT0 timer flag 0 */
}

void LPIT0_Ch0_IRQHandler (void)
{
	lpit0_ch0_flag_counter++;         /* Increment LPIT0 timeout counter */
	if (min == 0 && sec ==0) {
		LPIT0->MIER &= ~(1 << 0);
		return;
	}
	if (sec == 0) {
		min--;
		sec = 59;
	}
	sec--;
	LPIT0->MSR |= LPIT_MSR_TIF0_MASK;  /* Clear LPIT0 timer flag 0 */
}

void stopwatch(int m, int s)
{
	LPIT0->MIER |= 0x01;  /* TIE0=1: Timer Interrupt Enabled fot Chan 0,1,2 */
	min = m;
	sec = s;
	while (!(min == 0 && sec == 0))
		Seg_out(min, sec);
	LPIT0->TMR[0].TVAL = 40000000;      /* Chan 0 Timeout period: 40M clocks */
	LPIT0->TMR[0].TCTRL = 0x00000001;
}

int main(void)
{
	WDOG_disable();/* Disable Watchdog in case it is not done in startup code */
	PORT_init();            /* Configure ports */
	SOSC_init_8MHz();       /* Initialize system oscilator for 8 MHz xtal */
	SPLL_init_160MHz();     /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz();  /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
	NVIC_init_IRQs();       /* Enable desired interrupts and priorities */
	LPIT0_init(1);

	stopwatch(0, 10);
}

