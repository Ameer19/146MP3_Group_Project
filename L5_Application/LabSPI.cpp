/*
 * LabSPI.cpp
 *
 *  Created on: Mar 7, 2018
 *      Author: Ryan
 */

#include <LabSPI.h>
#include <LPC17xx.h>

LabSPI::LabSPI()
{
}

bool LabSPI::init(Peripheral peripheral, uint8_t data_size_select, FrameModes format, uint8_t divide)
{
    if(peripheral == SSP0)
    {
        LPC_SC->PCONP |= (1 << 21);
        LPC_SC->PCLKSEL1 &= ~(3 << 10);
        LPC_SC->PCLKSEL1 |= (1 << 10);
        //LPC_PINCON->PINSEL0 &= ~(3 << 6);
        LPC_PINCON->PINSEL1 |= (2 << 4 | 2 << 2);
        LPC_PINCON->PINSEL0 |= (2 << 30);
        //LPC_GPIO0->FIODIR |= (1 << 3);
        //LPC_GPIO0->FIOSET = (1 << 3);
        LPC_SSP0->CR0 |= ((data_size_select - 1) << 0);
        if(format == SPI)
        {
            LPC_SSP0->CR0 &= format;
        }
        else
        {
            LPC_SSP0->CR0 |= format;
        }

        //LPC_SSP0->CR0 &= ~(3 << 6);
        //LPC_SSP0->CR0 &= ~(255 < 8);
        //LPC_SSP0->CR1 &= ~(1 << 1);
        //LPC_SSP0->CR1 &= ~(1 << 2);
        LPC_SSP0->CR1 |= (1 << 1);

        LPC_SSP0->CPSR |= (divide << 0);
    }

    if(peripheral == SSP1)
    {
        LPC_SC->PCONP |= (1 << 10);
        LPC_SC->PCLKSEL0 &= ~(3 << 20);
        LPC_SC->PCLKSEL0 |= (1 << 20);
        LPC_PINCON->PINSEL0 &= ~(3 << 12);
        LPC_PINCON->PINSEL0 |= (2 << 14 | 2 << 16 | 2 << 18);
        LPC_PINCON->PINMODE0 |= (2 << 12 | 2 << 14 | 2 << 16 | 2 << 18);
        LPC_GPIO0->FIODIR |= (1 << 6);
        LPC_GPIO0->FIOSET = (1 << 6);
        LPC_SSP1->CR0 |= ((data_size_select - 1) << 0);
        if(format == SPI)
        {
            LPC_SSP1->CR0 &= format;
        }
        else
        {
            LPC_SSP1->CR0 |= format;
        }

        //LPC_SSP1->CR0 &= ~(3 << 6);
        //LPC_SSP1->CR0 &= ~(255 < 8);
        //LPC_SSP1->CR1 &= ~(1 << 1);
        //LPC_SSP1->CR1 &= ~(1 << 2);
        LPC_SSP1->CR1 |= (1 << 1);

        LPC_SSP1->CPSR |= (divide << 0);
    }
    return true;
}

uint8_t LabSPI::transfer(uint8_t send)
{
    LPC_SSP0->DR = send;
    while(LPC_SSP0->SR & (1 << 4));
    return LPC_SSP0->DR & (0xFFFF);
}

void LabSPI::spi_cs()
{
    LPC_GPIO0->FIOCLR = (1 << 6);
}

void LabSPI::spi_ds()
{
    LPC_GPIO0->FIOSET = (1 << 6);
}

LabSPI::~LabSPI()
{
}

