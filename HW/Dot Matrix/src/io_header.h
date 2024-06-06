#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#define A_High PORTD |= 0x04	// A
#define A_Low PORTD &= ~0x04	// Arduino Uno 1번핀
#define B_High PORTD |= 0x08	// B
#define B_Low PORTD &= ~0x08	// Arduino Uno 2번핀
#define C_High PORTD |= 0x10	// C
#define C_Low PORTD &= ~0x10	// Arduino Uno 3번핀
#define D_High PORTD |= 0x20	// C
#define D_Low PORTD &= ~0x20	// Arduino Uno 3번핀
#define R1_ON PORTD |= 0x40	// Red1
#define R1_OFF PORTD &= ~0x40	// Arduino Uno 5번핀
#define G1_ON PORTD |= 0x80	// Green1
#define G1_OFF PORTD &= ~0x80	// Arduino Uno 6번핀
#define B1_ON PORTB |= 0x01	// Blue1
#define B1_OFF PORTB &= ~0x01	// Arduino Uno 8번핀
#define R2_ON PORTB |= 0x02	// Red2
#define R2_OFF PORTB &= ~0x02	// Arduino Uno 8번핀
#define G2_ON PORTB |= 0x04	// Green2
#define G2_OFF PORTB &= ~0x04	// Arduino Uno 9번핀
#define B2_ON PORTB |= 0x08	// Blue2
#define B2_OFF PORTB &= ~0x08	// Arduino Uno 10번핀
#define Clk_enable PORTB |= 0x10	// Clk
#define Clk_disable PORTB &= ~0x10	// Arduino Uno 11번핀
#define OE_enable PORTB &= ~0x20	// Output_Enable(OE)
#define OE_disable PORTB |= 0x20	// Arduino Uno 12번핀
#define LE_enable PORTC |= 0x08	// Latch_Enable(LE)
#define LE_disable PORTC &= ~0x08	// Arduino Uno 13번핀 
#define Red 1	//폰트 컬러 적색
#define Green 2	//폰트 컬러 녹색 
#define Blue 3	//폰트 컬러 청색 

void ActivePulse()	// 스트로브 신호
{
	LE_enable;	// 래치 신호 비활성화   
	LE_disable;	// 래치 신호 활성화   
          OE_enable;	// 출력 신호 활성화
	delayMicroseconds(700);	// 700uS 딜레이
     	OE_disable;	// 출력 신호 비활성화

}

void Shift_Register(unsigned char High, unsigned char Low, unsigned char cnt_row, unsigned char FontColor)
{
	/*점등시키고자 하는 LED 16bit를 High(8bit), Low(8bit)로 나누어 cnt_row 인자값에 따라 상위 8비트, 하위 8비트 출력 */
	static unsigned char clk = 0;
	static unsigned int Temp = 0;
	Temp = High << 8;	// 상위 1바이트 좌측으로 8번 시프트 
	Temp |= Low;	// 상위 1바이트, 하위 8bit 결합
	for (clk = 0; clk < 16; clk++)
	{
		if (Temp &(1 << clk))	// 16비트 데이터를 1비트씩 시프트레지스터에 입력
		{
			switch (FontColor)
			{
				case 1:
					if (cnt_row < 8)
					{
					 			// 상위 8비트 ON, 하위 8비트 OFF 
						R1_ON;
						G1_OFF;
						B1_OFF;
						R2_OFF;
						B2_OFF;
						G2_OFF;
					}
					else
					{
					 			// 하위 8비트 ON, 상위 8비트 OFF
						R2_ON;
						G2_OFF;
						B2_OFF;
						R1_OFF;
						B1_OFF;
						G1_OFF;
					}

					break;
				case 2:
					if (cnt_row < 8)
					{
					 			// 상위 8비트 ON, 하위 8비트 OFF 
						R1_OFF;
						G1_ON;
						B1_OFF;
						R2_OFF;
						B2_OFF;
						G2_OFF;
					}
					else
					{
					 			// 하위 8비트 ON, 상위 8비트 OFF
						R2_OFF;
						G2_ON;
						B2_OFF;
						R1_OFF;
						B1_OFF;
						G1_OFF;
					}

					break;
				case 3:
					if (cnt_row < 8)
					{
					 			// 상위 8비트 ON, 하위 8비트 OFF 
						R1_OFF;
						G1_OFF;
						B1_ON;
						R2_OFF;
						B2_OFF;
						G2_OFF;
					}
					else
					{
					 			// 하위 8비트 ON, 상위 8비트 OFF
						R2_OFF;
						G2_OFF;
						B2_ON;
						R1_OFF;
						B1_OFF;
						G1_OFF;
					}

					break;
				default:
					if (cnt_row < 8)
					{
					 			// 상위 8비트 ON, 하위 8비트 OFF 
						R1_OFF;
						G1_OFF;
						B1_OFF;
						R2_OFF;
						B2_OFF;
						G2_OFF;
					}
					else
					{
					 			// 하위 8비트 ON, 상위 8비트 OFF
						R2_OFF;
						G2_OFF;
						B2_OFF;
						R1_OFF;
						B1_OFF;
						G1_OFF;
					}

					break;
			}
		}
		else
		{
			R2_OFF;
			G2_OFF;
			B2_OFF;
			R1_OFF;
			B1_OFF;
			G1_OFF;
		}

		Clk_enable;	// CLK 신호 활성화
		Clk_disable;
	}
}

void row_dynamic(unsigned char i)
{
	switch (i)	// 행 이동
	{
		case 0:
			A_Low;
			B_Low;
			C_Low;
			break;	// 1행 LED 
		case 1:
			A_High;
			B_Low;
			C_Low;
			break;	// 2행 LED 
		case 2:
			A_Low;
			B_High;
			C_Low;
			break;	// 3행 LED 
		case 3:
			A_High;
			B_High;
			C_Low;
			break;	// 4행 LED 
		case 4:
			A_Low;
			B_Low;
			C_High;
			break;	// 5행 LED 
		case 5:
			A_High;
			B_Low;
			C_High;
			break;	// 6행 LED 
		case 6:
			A_Low;
			B_High;
			C_High;
			break;	// 7행 LED 
		case 7:
			A_High;
			B_High;
			C_High;
			break;	// 8행 LED
		case 8:
			A_Low;
			B_Low;
			C_Low;
			break;	// 9행 LED 
		case 9:
			A_High;
			B_Low;
			C_Low;
			break;	// 10행 LED 
		case 10:
			A_Low;
			B_High;
			C_Low;
			break;	// 11행 LED 
		case 11:
			A_High;
			B_High;
			C_Low;
			break;	// 12행 LED 
		case 12:
			A_Low;
			B_Low;
			C_High;
			break;	// 13행 LED 
		case 13:
			A_High;
			B_Low;
			C_High;
			break;	// 14행 LED 
		case 14:
			A_Low;
			B_High;
			C_High;
			break;	// 15행 LED 
		case 15:
			A_High;
			B_High;
			C_High;
			break;	// 16행 LED 
	}
}