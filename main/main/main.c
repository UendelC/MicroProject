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

float power(int a, int b){
	int cont = a;
	while(b != 1){
		cont *= a;
		b--;
	}
	
	return(cont);
}

//Retirado de tutorialspoint.dev/language/c/convert-floating-point-number-string
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

//Retirado de tutorialspoint.dev/language/c/convert-floating-point-number-string
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
//Retirado de tutorialspoint.dev/language/c/convert-floating-point-number-string
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
		fpart = fpart * power(10, afterpoint);
		
		intToStr((int)fpart, res + i + 1, afterpoint);
	}
}

void configUsart()  {
  UBRR0H = 0x00; // Define Baud Rate
  UBRR0L = 0x08; // 115200
  
  UCSR0B = 1 << TXEN0;
  UCSR0C = 0b00000110; // Desabilita paridade, Seleciona um bit de stop
}

void configADC(){
  ADCSRA = 0x87; // Liga o ADC, não inicia a conversão
  ADCSRB = 0x00; // Uma conversão é iniciada sempre que o bit ADSC for setado
}

int analogRead(int ADMUX_){
	ADMUX = ADMUX_;
	ADCSRA = ADCSRA | 0x40; //Inicia a conversão ADCS = 1
	
	while((ADCSRA & 0x10) != 0x10){} //Espera a conversão ser finalizada - ADIF=1
	unsigned char datal = ADCL;
	unsigned char datah = ADCH;
	ADCSRA = ADCSRA | 0x10; //Zera o Flag
	
	return ((datah << 8) | (datal)); //Inteiro sem sinal de 16 bits - 0 a 1023
}

void convertToCelsiusStr(int data){
	float mv = ( data/1024.0)*5000;
	float cel = mv/10;
	ftoa(cel, result, 2);
	result[4] = ' ';
	result[5] = 'º';
	result[6] = 'C';
	result[7] = '\n';
	result[8] = '\0';
}


void convertToPercentage(int data) {
  float lum = (100.0 / 1023) * data;
  
  ftoa(lum, result, 2);

  result[4] = ' ';
  result[5] = '%';
  result[6] = '\n';
  result[7] = '\0';
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
  int cont =0;
  int temperatura;
  int luminosidade;
  
    while (1) 
    { 
	  for(cont = 0; cont<6; cont++) {         
		_delay_ms(5000);
		  switch(cont){
			case 0:
				temperatura = analogRead(0x00);
				convertToCelsiusStr(temperatura);
				send_(result);
				luminosidade = analogRead(0x02);
				convertToPercentage(luminosidade);
				send_(result);
				break;
			case 2:
				luminosidade = analogRead(0x02);
				convertToPercentage(luminosidade);
				send_(result);
				break;
			case 3:
				temperatura = analogRead(0x00);
				convertToCelsiusStr(temperatura);
				send_(result);
				break;
			case 4:
				luminosidade = analogRead(0x02);
				convertToPercentage(luminosidade);
				send_(result);
				break;
		  }
	  }
	  
	  
    }
}