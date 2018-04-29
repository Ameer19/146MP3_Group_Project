/*
 * MP3DecoderDriver.hpp
 *
 *  Created on: Apr 23, 2018
 *      Author: Ameer
 */

#include "LABSPI.h"
#ifndef MP3DECODERDRIVER_HPP_
#define MP3DECODERDRIVER_HPP_
class MP3Decoder
{
public:
        //Set Pin 1.19- RST
        //    Pin 1.20- DCS
        //    Pin 1.22- CS
        //    Pin 1.23- DREQ
        void init();
        void write_to_decoder(uint8_t addr, uint16_t data_out);
        uint16_t read_from_decoder(uint8_t addr);   //For checking purposes
        void playMP3Track(uint8_t buffer[]);
        LabSPI audio;
private:
     //LabSPI audio;    //Is this Safe? Using SSP0 for decoder

};


#endif /* MP3DECODERDRIVER_HPP_ */
