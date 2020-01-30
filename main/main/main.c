/*
 * LMR34 E LDR
 *
 * Created: 30/01/2020 10:20:13
 * Author : jzrhard05
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

char result[9]; 

void configUsart()  {
  UBRR0H = 0x00;
  UBRR0L = 0x67;
  
  UCSR0B = 1 << TXEN0;
}

void configADC(){
//  ADMUX = 0x00;
  
  ADCSRA = 0x87;
  ADCSRB = 0x00;
}

int readLDR() {
  ADMUX = 0x02;
  ADCSRA = ADCSRA | 0x40;
  
  while((ADCSRA & 0x10) != 0x10){}
  unsigned char datah = ADCH;
  unsigned char datal = ADCL;
  _delay_ms(200);
  //unsigned int data = ADC;
  ADCSRA = ADCSRA | 0x10;

  return ((datah << 8) | (datal));
}


int readLM35(){
  ADMUX = 0x00;
  ADCSRA = ADCSRA | 0x40;
  
  while((ADCSRA & 0x10) != 0x10){}
  unsigned char datah = ADCH;
  unsigned char datal = ADCL;
  _delay_ms(200);
  //unsigned int data = ADC;
  ADCSRA = ADCSRA | 0x10;
  
  return ((datah << 8) | (datal));
}


void convertToCelsiusStr(int data){
  long int celsius = data*489;

  int inteira = celsius / 1000;
  int decimal = celsius % 1000;
  result[0] = inteira/100 + 48;
  result[1] = (inteira % 100)/10 + 48;
  result[2] = inteira % 10 + 48;
  result[3] = ',';
  result[4] = decimal/100 + 48;
  result[5] = (decimal%100)/10 + 48;
  result[6] = decimal%10 + 48;
  result[7] = '\n';
  result[8] = '\0';
}

void convertToPercentage(int data) {
  double lum = data; // 100 * (100.0 / 1023) * data;

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
      convertToPercentage(temperatura);
//      convertToCelsiusStr(temperatura);
      send_(result);
//       _delay_ms(1000);
//      luminosidade = readLDR();
//      convertToPercentage(luminosidade);
//      send_(result);
    }
}