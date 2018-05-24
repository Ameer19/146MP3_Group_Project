# CMPE146 MP3 Player Project

This project uses the SJOne on board SD card to read MP3 audio files. This project's firmware is written from the ground-up and is implemented in C using FreeRTOS.

The system communicates to a external MP3 decoder and lets the user interact with the power, songs in the playlist, and change the volume. The LCD display shows a menu that denotes which function belongs to each button.

## Quickstart Instructions

To get started, have a look at L5_Application/main.cpp and the example tasks.

You can change a #define 0 to #define 1 for one example at a time and look
through the code of each example.

The general idea is to interact with the project through terminal commands.

See "ChangeLog" for the changes made to the sample project source code.

