/*
 * LabGPIO.cpp
 *
 *  Created on: Feb 13, 2018
 *      Author: Ryan
 */
#include <LabGPIO.hpp>
#include "LPC17xx.h"

LabGPIO::LabGPIO(uint8_t port, uint8_t pin)
{
    Port = port;
    Pin = pin;
}

void LabGPIO::selectPin()
{
    if(Port == 0 && ((Pin >= 0 && Pin <= 11) || Pin == 15))
    {
        LPC_PINCON->PINSEL0 &= ~(3 << (Pin * 2));
    }
    if(Port == 0 && ((Pin >= 15 && Pin <= 30)))
    {
        LPC_PINCON->PINSEL1 &= ~(3 << ((Pin - 16) * 2));
    }
    if(Port == 2 && (Pin >= 0 && Pin <= 13))
    {
        LPC_PINCON->PINSEL4 &= ~(3 << (Pin * 2));
    }
}

/**
 * Should alter the hardware registers to set the pin as an input
 */

void LabGPIO::setAsInput()
{
    if(Port == 0)
    {
        LPC_GPIO0->FIODIR &= ~(1 << Pin);
    }

    else if(Port == 1)
    {
        LPC_GPIO1->FIODIR &= ~(1 << Pin);
    }

    else
    {
        LPC_GPIO2->FIODIR &= ~(1 << Pin);
    }
}

/**
 * Should alter the hardware registers to set the pin as an output
 */
void LabGPIO::setAsOutput()
{
    if(Port == 0)
    {
        LPC_GPIO0->FIODIR |= (1 << Pin);
    }

    else if(Port == 1)
    {
        LPC_GPIO1->FIODIR |= (1 << Pin);
    }

    else
    {
        LPC_GPIO2->FIODIR |= (1 << Pin);
    }
}

/**
 * Should alter the set the direction output or input depending on the input.
 *
 * @param {bool} output - true => output, false => set pin to input
 */
void LabGPIO::setDirection(bool output)
{
    if(output)
    {
        setAsOutput();
    }

    else
    {
        setAsInput();
    }
}

/**
 * Should alter the hardware registers to set the pin as high
 */
void LabGPIO::setHigh()
{
    if(Port == 0)
    {
        LPC_GPIO0->FIOSET = (1 << Pin);
    }

    else if(Port == 1)
    {
        LPC_GPIO1->FIOSET = (1 << Pin);
    }

    else
    {
        LPC_GPIO2->FIOSET = (1 << Pin);
    }
}

/**
 * Should alter the hardware registers to set the pin as low
 */
void LabGPIO::setLow()
{
    if(Port == 0)
    {
        LPC_GPIO0->FIOCLR = (1 << Pin);
    }

    else if(Port == 1)
    {
        LPC_GPIO1->FIOCLR = (1 << Pin);
    }

    else
    {
        LPC_GPIO2->FIOCLR = (1 << Pin);
    }
}

/**
 * Should alter the hardware registers to set the pin as low
 *
 * @param {bool} high - true => pin high, false => pin low
 */
void LabGPIO::set(bool high)
{
    if(high)
    {
        setHigh();
    }

    else
    {
        setLow();
    }
}

/**
 * Should return the state of the pin (input or output, doesn't matter)
 *
 * @return {bool} level of pin high => true, low => false
 */
bool LabGPIO::getLevel()
{
    bool pin_state;

    if(Port == 0)
    {
        if(LPC_GPIO0->FIOPIN & (1 << Pin))
        {
            pin_state = true;
        }

        else
        {
            pin_state = false;
        }
    }
    else if(Port == 1)
    {
        if(LPC_GPIO1->FIOPIN & (1 << Pin))
        {
            pin_state = true;
        }

        else
        {
            pin_state = false;
        }
    }
    else
    {
        if(LPC_GPIO2->FIOPIN & (1 << Pin))
        {
            pin_state = true;
        }

        else
        {
            pin_state = false;
        }
    }

    return pin_state;
}

LabGPIO::~LabGPIO()
{

}


