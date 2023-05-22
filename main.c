#include<stm32f4xx.h>
#include <stdio.h>
/*NRPU
1:Relay Module Configuration
2:LCD Configuration
3:Automatic Mode 

*/

char mode[1];
int sec=40;
int min=59;
int hour=17;

//Relay
void configure(void)
{
	int pin = 0;
	// Initialize clock for Part C
	RCC ->AHB1ENR |= 0x04;
	
	for(pin=4;pin<=7;pin++)//PC4-PC7 as Output for Relay
	{
		GPIOC ->MODER &= ~(0x3 << (pin * 2));
		GPIOC ->MODER |= (0x1 << (pin * 2));
	}
}

void us(volatile int x)
{
	int i;
	for(i=0; i<(3*x); i++)
	{
		;
	}
}
// delay in micro second
void delay_us(volatile unsigned y)
{
  volatile unsigned int num;
  for (num = 0; num < y; num++)
  {}
	for (num = 0; num < y/2; num++)
  {}
}
// delay in milli second
void delay_ms(volatile unsigned a)
{
  volatile unsigned int num;
  for (num = 0; num < a; num++)
  {
    delay_us(1000);
  }
}
void delay_msi(volatile unsigned a)
{
  volatile unsigned int num;
  for (num = 0; num < a; num++)
  {
    delay_us(100000);
  }
}

//LCD OUTPUT
void configure_LCD(void)
{
	int pin;
	RCC->AHB1ENR |=0x1; //enable clock for port A
	RCC->AHB1ENR |=0x10;     //enable clock for port E

	for( pin=0; pin<=7; pin++)
	{
		GPIOA->MODER &= (~(0x3<<(pin*2)));
		GPIOA->MODER |=(0x01<<(pin*2));      //making PA0 to PA7 as output
	}
	for( pin=1; pin<=3; pin++)
	{
		GPIOE->MODER &= (~(0x3<<(pin*2)));
		GPIOE->MODER |=(0x01<<(pin*2));      //making PE1 to PE3 as output for RS, R/W, and EN respectively
	}
}
void lcd_write_command(unsigned char cmd)
{
	GPIOE->ODR &= ~(0x2);  //PE1
	GPIOE->ODR &= ~(0x4);   //PE2
	GPIOE->ODR |= (0x8);  //PE3
	us(2);
	GPIOA->ODR &= ~(0xFF);
	GPIOA->ODR |= cmd;
	us(2);
	GPIOE->ODR &= ~(0x8);
	us(2);
}
void lcd_write_data(unsigned char cmd)
{
	GPIOE->ODR |=(0x2);  //PE1
	GPIOE->ODR &= ~(0x4);   //PE2
	GPIOE->ODR |= (0x8);  //PE3

	GPIOA->ODR &= ~(0xFF);
	GPIOA->ODR |= cmd;
	us(2);
	GPIOE->ODR &= ~(0x8);
	us(2);
}

void lcd_init()
{
	lcd_write_command(0x38); //pin row configuration
	lcd_write_command(0x0C); //clear all

	lcd_write_command(0x06);  //Cursor shift
	lcd_write_command(0x01);  //Return home

	lcd_write_command(0x80);  //Turn on Display
}


void lcd_clear(void)
{
	lcd_write_command(0x01);//clear disp
	delay_ms(2);
	lcd_write_command(0x02);//move cursor to home
	delay_ms(2);
}

void lcd_write_string(unsigned char *ptr)
{
	int j;
	for(j=0;ptr[j]!=0;j++)
	{
		delay_ms(2);
		lcd_write_data(ptr[j]);
		delay_ms(2);
	}
	if (strlen(ptr)<=16)
	{
		while(*ptr!=0)
		{
			lcd_write_data(*ptr);
			ptr++;
		}
	}
	else if (strlen(ptr)>16)
	{
		for (j=0;j<16;j++)
		{
			lcd_write_data(*ptr);
			ptr++;
		}
		lcd_write_command(0xC0);
		while(*ptr)
		{
			lcd_write_data(*ptr);
			ptr++;
		}
	}
}

void lcd_write_num(int print_val)
{
	int i=0,buff[20];
	int temp=print_val;

	if (print_val<0)
	{
		lcd_write_data('-');
		print_val*=(-1);
	}
	else if(print_val==0)
	{
		lcd_write_data(48);
	}
	else
	{
		for(i=0;print_val!=0;i++)
		{
			buff[i]=temp%10;
			print_val=print_val/10;
			temp=print_val;
		}
		for(;i!=0;--i)
		{
			lcd_write_data(buff[i-1]+48);
		}
	}
}

void Relay_Status(int choice)
{
	//Relay(1);Light 1
	//Relay(2);Light 2
	

	switch (choice)
	{
		case -1:
			break;
		case 0x01:
			lcd_write_string("Relay Num 1= OFF");
			delay_ms(1000);
			lcd_clear();
			break;
		case 0x02:
			lcd_write_string("Relay Num 2= OFF");
			delay_ms(1000);
			lcd_clear();
			break;
			case 0x10:
			lcd_write_string("Relay Num 1= ONN");
			delay_ms(1000);
			lcd_clear();
			break;
		case 0x20:
			lcd_write_string("Relay Num 2= ONN");
			delay_ms(1000);
			lcd_clear();
			break;
		
	}
}

void Relay_On(int choice)
{
	//Relay(1);Light 1
	//Relay(2);Light 2
	

	switch (choice)
	{
		case -1:
			break;
		case 1:
			GPIOC->ODR &= ~0x10; //PC4 = Relay 1
			Relay_Status(0x10);
			break;
		case 2:
			GPIOC->ODR &= ~0x20; //PC5 = Relay 2
			Relay_Status(0x20);
			break;
		default:{}
	}
}
void Relay_Off(int choice)
{
	//Relay(1);Light 1
	//Relay(2);Light 2
	//Relay(3);Light 3
	//Relay(4);Light 4

	switch (choice)
	{
		case -1:
			break;
		case 1:
			GPIOC->ODR |= 0x10; //PC4 = Relay 1
			Relay_Status(0x01);
			break;
		case 2:
			GPIOC->ODR |= 0x20; //PC5 = Relay 2
			Relay_Status(0x02);
			break;
		default:{}
	}
}



void CLOCK(void)
	{ for (int n=0;n<10;n++){
lcd_write_command(0xC0);
lcd_write_string("It's  Day  Time.");
		delay_ms(1000);
lcd_write_string(" PV Power ON    ");
		delay_ms(5000);
		lcd_clear();
		delay_ms(1000);
			Relay_On(1);
		delay_ms(1000);
		Relay_Off(2);
			delay_ms(10000);
lcd_write_command(0xC0);
lcd_write_string(" It's Night Time");
		delay_ms(1000);
lcd_write_string("-Gasf Power On--");
		delay_ms(5000);
		lcd_clear();
				Relay_Off(1);
		delay_ms(1000);
		Relay_On(2);
		delay_ms(10000);
		
			
			

}
}

void umar (void)
{
	if (delay_msi==100000){
		Relay_On(1);
				Relay_Off(2);}
		else {	Relay_Off(1);
			Relay_On(2);}}
void Intro(void)
{//Prints Intro Messsage
	unsigned char text1[] = {"------NRPU------"};
	unsigned char text2[] = {"--286 253 260---"};
	int i;
	for (i=0; i<16; i++)
	{
		delay_ms(100);
		lcd_write_data(text1[i]);
	}
	delay_ms(100);
	lcd_write_command(0xC0);
	delay_ms(10);
	for (i=0; i<16; i++)
	{
		delay_ms(100);
		lcd_write_data(text2[i]);
	}
	lcd_write_command(0x0C);//Cursor OFF
	lcd_write_command(0x02);//Return Home
	delay_ms(200);
}
/*void LCD_OUT(void)
{
	int i;
	lcd_clear();
	lcd_write_string(" Hour:          ");
	delay_ms(1000);
	lcd_write_command(0xC0);
	lcd_write_string(" Mins:          ");
	delay_ms(1000);
	lcd_clear();
	for(i=0;i<60;i=i+59)
	{
		if (i==59)
		{
			lcd_write_num(hour+3);
			delay_ms(1000);
			lcd_write_command(0xC0);
			lcd_write_num(min-(min-1));
			delay_ms(1000);
			lcd_clear();
			delay_ms(10);
		}
		else
		{
			lcd_write_num(hour);
			delay_ms(1000);
			lcd_write_command(0xC0);
			lcd_write_num(min);
			delay_ms(1000);
			lcd_clear();
			delay_ms(1000);
		}
		lcd_write_command(0x80);
		delay_ms(10);
	}
	hour=hour+1;
	min=1;
	for(hour=hour+1;hour>=6;hour=hour-10)
	{
		if((hour<=7)||(hour>=18))//Night Time
		{
			lcd_write_string("It's  Day  Time.");
			lcd_write_string("--PV  Power ON--");
			delay_ms(5000);
			lcd_clear();
			Relay_On(1);
			delay_ms(10000);
			lcd_write_string("It's  Day  Time.");
			lcd_write_string("--Rel  Power ON--");
				
			Relay_Off(2);
			lcd_clear();
		}
		else
		{
			lcd_write_string("It is Night Time");
			lcd_write_string("Gasfier Power On");
			delay_ms(5000);
			lcd_clear();
			Relay_Off(1);
				delay_ms(10000);
			lcd_write_string("It is Night Time");
			lcd_write_string(" So Gasefir On  ");
			Relay_On(2);
			delay_ms(1000);
			lcd_write_string("Thank Youuu!!!  ");
			delay_ms(5000);
			lcd_clear();
		}
	}

	delay_ms(1000);
}*/




int main(void)
{
	
	Relay_Off(1);
	lcd_clear();
	Relay_Off(2);
	int i;
	configure();
	//configure_uart();
	configure_LCD();
	lcd_init();
	lcd_clear();
	delay_ms(1000);

	
	Intro();
	delay_ms(1000);
	lcd_clear();
	
	//AUTOMATIC MODE
	lcd_write_string("------Mode------");
	delay_ms(1000);
	lcd_write_command(0xC0);
	lcd_write_string("Aut Powe Control");
	delay_ms(1000);
	lcd_clear();
	lcd_write_string(" Automatic Mode ");
	delay_ms(1000);
	lcd_write_command(0xC0);
	lcd_write_string("Activated !!!   ");
	delay_ms(1000);
	lcd_clear();
	
		delay_ms(1000);
	//Relay_Off(1);
	CLOCK();
	lcd_clear();
//	Relay_Off(2);
	lcd_clear();
	
		
	return 0;
}
