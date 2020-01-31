/*
 * LMR34 E LDR
 *
 * Created: 30/01/2020 10:20:13
 * Author : Uendel
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

char result[9]; 

void reverse(char* str, int len)
{
	int i = 0, j = len - 1, temp;
	while (i < j) {
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++;
		j--;
	}
}

int intToStr(int x, char str[], int d)
{
	int i = 0;
	while (x) {
		str[i++] = (x % 10) + '0';
		x = x / 10;
	}
	
	// If number of digits required is more, then
	// add 0s at the beginning
	while (i < d)
	str[i++] = '0';
	
	reverse(str, i);
	str[i] = '\0';
	return i;
}

// Converts a floating-point/double number to a string.
void ftoa(float n, char* res, int afterpoint)
{
	// Extract integer part
	int ipart = (int)n;
	
	// Extract floating part
	float fpart = n - (float)ipart;
	
	// convert integer part to string
	int i = intToStr(ipart, res, 0);
	
	// check for display option after point
	if (afterpoint != 0) {
		res[i] = ','; // add dot
		
		// Get the value of fraction part upto given no.
		// of points after dot. The third parameter
		// is needed to handle cases like 233.007
		fpart = fpart * pow(10, afterpoint);
		
		intToStr((int)fpart, res + i + 1, afterpoint);
	}
}

void configUsart()  {
  UBRR0H = 0x00;
  UBRR0L = 0x67;
  
  UCSR0B = 1 << TXEN0;
}

void configADC(){
//  ADMUX = 0x00; // Canal 0 do ADC
  
  ADCSRA = 0x87; // Liga o ADC, não inicia a conversão
  ADCSRB = 0x00; // Uma conversão é iniciada sempre que o bit ADSC for setado
}

int readLDR() {
  //ADMUX = 0x02;
  ADCSRA = ADCSRA | 0x40; //Inicia a conversão ADCS = 1
  
  while((ADCSRA & 0x10) != 0x10){} //Espera a conversão ser finalizada - ADIF = 1
  unsigned char datal = ADCL;
  unsigned char datah = ADCH;
  //_delay_ms(200);
  //unsigned int data = ADC;
  ADCSRA = ADCSRA | 0x10; //Zera o flag

  return ((datah << 8) | (datal)); //Inteiro sem sinal 16 bits - 0 a 1023
}


int readLM35(){
  //ADMUX = 0x00;
  ADCSRA = ADCSRA | 0x40; //Inicia a conversão ADCS = 1
  
  while((ADCSRA & 0x10) != 0x10){} //Espera a conversão ser finalizada - ADIF=1
  unsigned char datal = ADCL;
  unsigned char datah = ADCH;
  ///_delay_ms(200);
  //unsigned int data = ADC;
  ADCSRA = ADCSRA | 0x10; //Zera o Flag
  
  return ((datah << 8) | (datal)); //Inteiro sem sinal de 16 bits - 0 a 1023
}


void convertToCelsiusStr(int data){
	float mv = ( data/1024.0)*5000;
	float cel = mv/10;
	ftoa(cel, result, 2);
}



void convertToPercentage(int data) {
  double lum = 100 * (100.0 / 1023) * data;

  int inteira = (int) lum / 100;
  int decimal = ((int) lum) % 100;
  result[0] = inteira/1000 ? inteira/1000 + 48 : ' ';
  result[1] = (inteira % 1000)/100 ? (inteira % 1000)/100 + 48 : ' ';
  result[2] = (inteira % 100)/10 + 48;
  result[3] = inteira % 10 + 48;
  result[4] = ',';
  result[5] = decimal/10 + 48;
  result[6] = '%';
  result[7] = '\n';
  result[8] = '\0';
}

void send_(char * mensagem) {
  int i = 0;
  while(mensagem[i]) {
    while(!(UCSR0A & (1 << UDRE0))){}
    UDR0 = (unsigned char)mensagem[i];
    UCSR0A = 0x40; 
    i++;
  }
}

int main(void)
{
  configADC();
  configUsart();
  int temperatura;
  //int luminosidade;
  
    while (1) 
    {
      _delay_ms(1000);
      temperatura = readLM35();
      convertToCelsiusStr(temperatura);
      send_(result);
//       _delay_ms(1000);
      //luminosidade = readLDR();
      //convertToPercentage(luminosidade);
      //send_(result);
    }
}