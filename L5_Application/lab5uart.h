/*
 * lab5uart.h
 *
 *  Created on: Mar 15, 2018
 *      Author: 1995v
 */

#ifndef LAB5UART_H_
#define LAB5UART_H_

#include "LPC17xx.h"

class Lab5_UART {
private:

public:
    void Init(void){
        // 1. Power PCUART2 pin 24
          LPC_SC->PCONP |=(1<<24);

          // 2. Peripherial Clock: PCLOCKSEL1 17:16
          LPC_SC->PCLKSEL1&= ~(3<<16);
          //48mhz cpu clock. 12mhz pclk for uart.

          // 3. Baude Rate: we want 9600 BPS, 625= (DLM+DLL)
          // Set DLAB = 1 - enables us to talk with DLL and DLM
          LPC_UART2->LCR |= (1<<7);


          //Set DLL
          LPC_UART2->DLL &= ~(255 << 0); // set DLL to 78 cause 78*16 = 1250. 12mhz/1250 = 9600bps.
          LPC_UART2->DLL |= (78 << 0); //set DLL to 78
          LPC_UART2->DLM &= ~(255 << 0); // clear DLM cause not necessary

          //THEN 8 bit data, 1 stop bit, no parity
          LPC_UART2->LCR |= (3 << 0); //8 bit data length
          LPC_UART2->LCR |= (0 << 2); // Stopbit is set to one
          LPC_UART2->LCR &= ~(1 << 3);//

          // 4. UART FIFO - Enabled FIFO, reset TX* RX buffers*
          LPC_UART2->FCR |= (1 << 0);
          LPC_UART2->FCR |= (1 << 1);
          LPC_UART2->FCR |= (1 << 2);

          // 5. Pins select UART pins through the PINSEL registers
          //TxD2 2[8]
          LPC_PINCON->PINSEL4 &= ~(3 << 16);
          LPC_PINCON->PINSEL4 |= (2 << 16);
          LPC_PINCON->PINMODE4 |= (2 << 16);
          //RxD2 2[9]
          LPC_PINCON->PINSEL4 &= ~(3 << 18);
          LPC_PINCON->PINSEL4 |= (2 << 18);
          LPC_PINCON->PINMODE4 |= (2 << 18);

          // 6. Interrupts: DLAB =0
          LPC_UART2->LCR &= ~(1<<7);
          //U2IER

          //Init UART Rx interrupt (TX interrupt is optional)
          LPC_UART2->IER |= (1 << 2);

          //isr_register(Uart2, my_uart2_rx_intr);

    };

    char RxChar(void)
    {
        char mychar;
        while(! (LPC_UART2->LSR & (1 << 0)) );
        mychar = LPC_UART2->RBR;
        printf("Returned: %c \n", mychar);
        return mychar;
    }

    void TxChar(char input){
        while(! (LPC_UART2->LSR & (1 << 5)) );
        LPC_UART2->THR = input;
    };

    void TxInt(int input){
        while(! (LPC_UART2->LSR & (1 << 5)) );
        LPC_UART2->THR = input;
    };

    void lcd_line_one ( char * buff) {
        TxInt(0xFE);
        TxInt(128);
        for (int i = 0 ; i < 16 ; i++) {
            TxChar(*buff);
            buff++;
        }

    }

    void lcd_line_two ( char  buff[100]) {
        TxInt(0xFE);
        TxInt(192);
        for (int i = 0 ; i < 16 ; i++) {
            TxChar(buff[i]);
        }
    }
};

#endif /* LAB5UART_H_ */
