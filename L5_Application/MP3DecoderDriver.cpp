/*
 * MP3DecoderDriver.cpp
 *
 *  Created on: Apr 23, 2018
 *      Author: Ameer
 */
#include "MP3DecoderDriver.hpp"
#include "printf_lib.h"
#include "utilities.h"
#include "tasks.hpp"

#define SCI_MODE 0x00
#define SCI_STATUS 0x01
#define SCI_BASS 0x02
#define SCI_CLOCKF 0x03
#define SCI_DECODE_TIME 0x04
#define SCI_AUDATA 0x05
#define SCI_WRAM 0x06
#define SCI_WRAMADDR 0x07
#define SCI_HDAT0 0x08
#define SCI_HDAT1 0x09
#define SCI_AIADDR 0x0A
#define SCI_VOL 0x0B
#define SCI_AICTRL0 0x0C
#define SCI_AICTRL1 0x0D
#define SCI_AICTRL2 0x0E
#define SCI_AICTRL3 0x0F

uint8_t volume_level = 5;

void MP3Decoder::init()
{
    audio.init(audio.SSP0,0x08,audio.SPI,8);    //Have SPP0 take care of communications between decoder

    LPC_GPIO1->FIODIR |= (1<<19);   //set pin P1.19 as output for RESET signal
    LPC_PINCON->PINSEL3 &= ~(3<<6);

    LPC_GPIO1->FIODIR |= (1<<20);   //set pin P1.20 as output for DCS signal
    LPC_PINCON->PINSEL3 &= ~(3<<8);

    LPC_GPIO1->FIODIR |= (1<<22);   //set pin P1.22 as output for CS signal
    LPC_PINCON->PINSEL3 &= ~(3<<12);

    LPC_GPIO1->FIOSET = (1<<19);    //set pin P1.19 high initially
    LPC_GPIO1->FIOSET = (1<<20);    //set pin P1.20 high initially
    LPC_GPIO1->FIOSET = (1<<22);    //set pin P1.22 high initially

    LPC_GPIO1->FIODIR &= ~(1<<23);  //set pin P1.23 as input for DREQ
    LPC_PINCON->PINSEL3 &= ~(3<<14);

    write_to_decoder(SCI_MODE, 0x0800);         //Set up MODE register
    //write_to_decoder();                         //Set up Bass/Treble register
    write_to_decoder(SCI_CLOCKF, 0x2000);       //Set up Clock register
    write_to_decoder(SCI_AUDATA, 0xAC45);       //Set up AudioData register               `
    //write_to_decoder(SCI_VOL, 0x2424);          //Set up Volume register
    volumeControl(true, true);
    //uint16_t data = read_from_decoder(SCI_VOL);
    //u0_dbg_printf("mode data = %x\n", data);

}
void MP3Decoder::write_to_decoder(uint8_t addr, uint16_t data_out)
{
    while(!(LPC_GPIO1->FIOPIN & (1 << 23)))
    {
        //vTaskDelay(1);
    }

    LPC_GPIO1->FIOCLR = (1 << 22);
    audio.transfer(0x02);
    audio.transfer(addr);

    uint8_t highByte = (data_out>>8) & 0xFF;
    //u0_dbg_printf("highByte = %x\n", highByte);
    audio.transfer(highByte);

    uint8_t lowByte = data_out & 0xFF;
    //u0_dbg_printf("lowByte = %x\n", lowByte);
    audio.transfer(lowByte);

    while(!(LPC_GPIO1->FIOPIN & (1 << 23)))
    {
        //vTaskDelay(1);
    }

    LPC_GPIO1->FIOSET = (1 << 22);
}

uint16_t MP3Decoder::read_from_decoder(uint8_t addr)
{
    while(!(LPC_GPIO1->FIOPIN & (1 << 23)));
    {
        //vTaskDelay(1);
    }

    LPC_GPIO1->FIOCLR = (1 << 22);
    audio.transfer(0x03);
    audio.transfer(addr);
    uint8_t highByte = audio.transfer(0xFF);

    while(!(LPC_GPIO1->FIOPIN & (1 << 23)))
    {
        //vTaskDelay(1);
    }

    uint8_t lowByte = audio.transfer(0xFF);

    while(!(LPC_GPIO1->FIOPIN & (1 << 23)))
    {
        //vTaskDelay(1);
    }

    LPC_GPIO1->FIOSET = (1 << 22);

    uint16_t dataRead = highByte << 8;
    dataRead |= lowByte;
    //u0_dbg_printf("dataRead: %x\n", dataRead);

    return dataRead;
}

void MP3Decoder::playMP3Track(uint8_t buffer[])
{
    for(uint8_t byteIndex = 0; byteIndex < 16; byteIndex++)         //Iterate through all 16 32 byte chunks
    {
        //u0_dbg_printf("byteIndex = %d\n", byteIndex);
        while(!(LPC_GPIO1->FIOPIN & (1 << 23)))                     //Wait until DREQ is set to high
        {
            //vTaskDelay(1);
        }

        LPC_GPIO1->FIOCLR = (1 << 20);                              //Select SDI CS
        for(uint16_t i = 32 * byteIndex; i < (32 * byteIndex) + 32; i++)
        {
            //u0_dbg_printf("i = %d, audio transferring.\n", i);
            MP3Decoder::audio.transfer(buffer[i]);           //Send next 32 bytes to decoder
            //delay_ms(100);
        }

        while(!(LPC_GPIO1->FIOPIN & (1 << 23)))                      //Wait until DREQ is set to high
        {
            //vTaskDelay(1);
        }

        LPC_GPIO1->FIOSET = (1 << 20);                              //Deselect SDI CS
    }
}

void MP3Decoder::volumeControl(bool higher, bool init)
{
    if(higher && volume_level < 8 && !init)
    {
        volume_level++;
    }
    else if(!higher && volume_level > 1 && !init){
        volume_level--;
    }

    if(volume_level == 8)
    {
        write_to_decoder(SCI_VOL, 0x1010);
        u0_dbg_printf("volume level = %i", volume_level);
    }
    else if(volume_level == 7)
    {
        write_to_decoder(SCI_VOL, 0x3232);
        u0_dbg_printf("volume level = %i", volume_level);
    }
    else if(volume_level == 6)
    {
        write_to_decoder(SCI_VOL, 0x5454);
        u0_dbg_printf("volume level = %i", volume_level);
    }
    else if(volume_level == 5)
    {
        write_to_decoder(SCI_VOL, 0x7676);
        u0_dbg_printf("volume level = %i", volume_level);
    }
    else if(volume_level == 4)
    {
        write_to_decoder(SCI_VOL, 0x9898);
        u0_dbg_printf("volume level = %i", volume_level);
    }
    else if(volume_level == 3)
    {
        write_to_decoder(SCI_VOL, 0xBABA);
        u0_dbg_printf("volume level = %i", volume_level);
    }
    else if(volume_level == 2)
    {
        write_to_decoder(SCI_VOL, 0xDCDC);
        u0_dbg_printf("volume level = %i", volume_level);
    }
    else if(volume_level == 1)
    {
        write_to_decoder(SCI_VOL, 0xFEFE);
        u0_dbg_printf("volume level = %i", volume_level);
    }
    vTaskDelay(1000);
}

