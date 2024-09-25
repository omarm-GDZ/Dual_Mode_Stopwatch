#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
unsigned char sec=0,min=0,hrs=0,countdown_flag=0,flag=0,pause_flag=0;
void Timer1_Init(void);
void INT_Init(void);
void Display_Time(void);
ISR(TIMER1_COMPA_vect){
	if(countdown_flag==0)
		sec++;
	else if(countdown_flag==1)
	{
		if(hrs==0 && min==0 && sec==0)
		{
			PORTD|=(1<<PD0);
			TCCR1B &=~(1<<CS12) &~(1<<CS10);
		}
		else sec--;
	}
}
ISR(INT0_vect){
	sec=0;
	min=0;
	hrs=0;
	countdown_flag=0;
}
ISR(INT1_vect){
	TCCR1B &=~(1<<CS12) &~(1<<CS10);
	pause_flag=1;
	PORTD&=~(1<<PD0);
}
ISR(INT2_vect){
	Timer1_Init();
	pause_flag=0;
}
int main(void){
	DDRC |=0x0F;
	DDRA =0xFF;
	DDRB=0x00;
	DDRD|=(1<<PD0) |(1<<PD4) |(1<<PD5);
	DDRD &=~(1<<PD2) &~(1<<PD3);
	PORTC&=0xF0;
	PORTB=0xFF;
	PORTD|=(1<<PD2);
	SREG|=(1<<7);
	Timer1_Init();
	INT_Init();
	while(1){
		if(countdown_flag==0){
			PORTD|=(1<<PD4);
			PORTD&=~(1<<PD5);
			Display_Time();
			if(sec==60 && pause_flag==0)
			{
				sec=0;
				min++;
				Display_Time();
			}
			if(min==60 && pause_flag==0)
			{
				min=0;
				hrs++;
				Display_Time();
			}
			if(hrs==24 && pause_flag==0)
			{
				hrs=0;
				Display_Time();
			}
		}
		else if(countdown_flag==1)
		{
			PORTD|=(1<<PD5);
			PORTD&=~(1<<PD4);
			Display_Time();
			if(sec>59 && min>0)
			{
				sec=59;
				min--;
				Display_Time();
			}
			else if(min==0 && sec>60 && hrs>0){

				sec=59;
				min=59;
				hrs--;
				Display_Time();
			}
		}
		if(!(PINB & (1<<PB7))){
			_delay_ms(30);
			if(!(PINB & (1<<PB7))){
				if(flag==0){
					flag=1;
					if(countdown_flag==0)
						countdown_flag=1;
					else {
						countdown_flag=0;
						PORTD&=~(1<<PD0);
						if(pause_flag==0)
							Timer1_Init();
					}
				}
			}else flag=0;
		}
		// decrement hours
		if(!(PINB &(1<<PB0)))
		{
			_delay_ms(30);
			if(!(PINB &(1<<PB0)))
			{
				if(flag==0)
				{
					flag=1;
					if(hrs>0)
						hrs--;
					else hrs=24;
				}
			}else flag=0;
		}

		// decrement minutes
		if(!(PINB &(1<<PB3)))
		{
			_delay_ms(30);
			if(!(PINB &(1<<PB3)))
			{
				if(flag==0)
				{
					flag=1;
					min--;
					if(min>60)
						min=59;
				}
			}else flag=0;
		}

		// decrement seconds
		if(!(PINB &(1<<PB5)))
		{
			_delay_ms(30);
			if(!(PINB &(1<<PB5)))
			{
				if(flag==0)
				{
					flag=1;
					sec--;
					if(sec>60)
						sec=59;
				}
			}else flag=0;
		}

		// increment hrs
		if(!(PINB &(1<<PB1)))
		{
			_delay_ms(30);
			if(!(PINB &(1<<PB1)))
			{
				if(flag==0)
				{
					flag=1;
					hrs++;
					if(hrs==25)
						hrs=0;
				}
			}else flag=0;
		}

		// increment mins
		if(!(PINB &(1<<PB4)))
		{
			_delay_ms(30);
			if(!(PINB &(1<<PB4)))
			{
				if(flag==0)
				{
					flag=1;
					min++;
					if(min==60)
						min=0;
				}
			}else flag=0;
		}

		// increment sec
		if(!(PINB &(1<<PB6)))
		{
			_delay_ms(30);
			if(!(PINB &(1<<PB6)))
			{
				if(flag==0)
				{
					flag=1;
					sec++;
					if(sec==60)
						sec=0;
				}
			}else flag=0;
		}
	}//while brace
}
void Timer1_Init(void){
	TCCR1A =(1<<FOC1A );
	TCCR1B = (1<<WGM12) | (1<<CS12) |(1<<CS10);
	TCCR1B &=~(1<<CS11);
	TCNT1=0;
	OCR1A=15625;
	TIMSK|=(1<<OCIE1A);
}
void INT_Init(void){
	MCUCR|=(1<<ISC01) |(1<<ISC11)|(1<<ISC10);
	MCUCSR &=~(1<<ISC2);
	GICR |=(1<<INT0) |(1<<INT1) |(1<<INT2);
}
void Display_Time(void){
	PORTA|=(1<<PA0);
	PORTC=(PORTC&0)|(sec%10);
	_delay_ms(2);
	PORTA&=~(1<<PA0);

	PORTA|=(1<<PA1);
	PORTC=(PORTC&0)|(sec/10);
	_delay_ms(2);
	PORTA&=~(1<<PA1);

	PORTA|=(1<<PA2);
	PORTC=(PORTC&0)|(min%10);
	_delay_ms(2);
	PORTA&=~(1<<PA2);

	PORTA|=(1<<PA3);
	PORTC=(PORTC&0)|(min/10);
	_delay_ms(2);
	PORTA&=~(1<<PA3);

	PORTA|=(1<<PA4);
	PORTC=(PORTC&0)|(hrs%10);
	_delay_ms(2);
	PORTA&=~(1<<PA4);

	PORTA|=(1<<PA5);
	PORTC=(PORTC&0)|(hrs/10);
	_delay_ms(2);
	PORTA&=~(1<<PA5);
}
