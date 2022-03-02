#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
unsigned char g_num =0;
unsigned char second=0;
unsigned char minute=0;
unsigned char hour=0;
void Timer1_Init(void)
{
	/*Configure the timer mode
	 * Non PWM-Mode
	 * CTC Mode
	 * Enable compare match in Timer1
	 * Enable I-bite (Global interrupt mode )
	 * \*/
	TCCR1A|=(1<<FOC1A)|(1<<FOC1B);
	TCCR1B|=(1<<WGM12)|(1<<CS10)|(1<<CS11);
	TCNT1=0;
	OCR1A=15625;
	SREG|=(1<<7);
	 TIMSK |=(1<<OCIE1A);
}
ISR (TIMER1_COMPA_vect)
{
	second ++;
	if (second ==60)
	{
		second=0;
		minute++;
	}
	if (minute==60)
	{
		second=0;
		minute=0;
		hour++;
	}
	if (hour==24)
	{
		second=0;
		minute=0;
		hour=0;
	}
}
void INT0_Init_Reset(void )
{
	/* Reset the 7-segment */
	DDRD&=~(1<<PD2);//make the pin D is input
	PORTD|=(1<<PD2); //internal pull-up
    MCUCR |=(1<<ISC01);
    GICR|=(1<<INT0);
    SREG|=(1<<7);
}
ISR (INT0_vect)
{
	second=0;
	minute=0;
	hour=0;
}
void INT1_Init_Pause(void)
{
	DDRD&=~(1<<PD3);//make PD3 input pin
	MCUCR|=(1<<ISC11)|(1<<ISC10);//rising edge
	GICR|=(1<<INT1);
	SREG|=(1<<7);
}
ISR (INT1_vect)
{
	/*close the clock of Timer1 */
	TCCR1B&=~(1<<CS10)&~(1<<CS11)&~(1<<CS12);
}
void INT2_Init_Resume(void )
{
	DDRB &=~ (1<<PB2);//make the PB2 input pin
	PORTB |= (1<<PB2);//internal pull-up
	MCUCR&=~(1<<ISC2);//falling edge
	GICR |=(1<<INT2);
	SREG|=(1<<7);
}
ISR (INT2_vect)
{
	/*return the clock of Timer1 */
TCCR1B|=(1<<WGM12)|(1<<CS10)|(1<<CS11);
}
int main (void )
{

DDRA=0XFF; //make the pins of 7-segment as input
PORTA=0XFF;
DDRC=0X0F;// make the 7-segment as output
PORTC=0X00; //turn off the 7-segment
INT0_Init_Reset();//calling the function of reset
INT1_Init_Pause();//calling the function of pause
INT2_Init_Resume();//calling the function of resume
Timer1_Init();// calling the function of Timer1
while (1)
{

	PORTA =(1<<PA5);//enable the PA5 to print the count
	PORTC =second %10 ;
	_delay_ms(5);
	PORTA =(1<<PA4);
	PORTC =second /10;
	_delay_ms(5);
	PORTA =(1<<PA3);
	PORTC =minute %10;
    _delay_ms(5);
	PORTA =(1<<PA2);
	PORTC =minute /10 ;
	_delay_ms(5);
	PORTA =(1<<PA1);
	PORTC =hour %10 ;
	_delay_ms(5);
	PORTA =(1<<PA0);
	PORTC =hour /10 ;
	_delay_ms(5);
}

}
