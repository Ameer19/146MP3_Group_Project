#include <FreeRTOS.h>
#include <stdio.h>
#include <string.h>
#include "semphr.h"
#include <task.h>
#include <queue.h>
#include "uart0_min.h"
#include <stdint.h>
#include "LPC17xx.h"
#include "tasks.hpp"
#include "examples/examples.hpp"
#include "io.hpp"
#include "storage.hpp"
#include "lpc_sys.h"
#include "soft_timer.hpp"
#include "event_groups.h"
#include "LabGPIO.hpp"

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <stdio.h>
#include "scheduler_task.hpp"
#include "MP3DecoderDriver.hpp"
#include "lab5uart.h"

//Test Comment
//Test Comment 2

QueueHandle_t MP3Queue;
MP3Decoder MP3Player;
LabGPIO Play_Pause(1,9); //s0
LabGPIO Volume_Up_Button(1,10); //s1
LabGPIO Volume_Down_Button(1,14); //s2
LabGPIO Previous_Button(1,15); //s3
LabGPIO Next_Button(1,28);//^external switch
TaskHandle_t MP3PlayPause = NULL;
char playlist [20][100] = {0};
char* currenttrack;
Lab5_UART UART;
int arrayindex = 0;
int lcdchangetrack = 0;
bool playlistgenerationflag = true;


FRESULT Storage :: readallMP3Files()//const char* pFilename,  void* pData, unsigned int bytesToRead, unsigned int offset
{

   if (playlistgenerationflag){
       arrayindex = 0;
       DIR Dir;
       FILINFO Finfo;
       FRESULT returnCode = FR_OK;

       #if _USE_LFN
           char Lfname[_MAX_LFN];
       #endif

       const char *dirPath = "1:";
       if (FR_OK != (returnCode = f_opendir(&Dir, dirPath))) {
           printf("Invalid directory: |%s| (Error %i)\n", dirPath, returnCode);
       }

       while (returnCode == FR_OK)
       {

           #if _USE_LFN
               Finfo.lfname = Lfname;
               Finfo.lfsize = sizeof(Lfname);
           #endif

           returnCode = f_readdir(&Dir, &Finfo);
           if ( (FR_OK != returnCode) || !Finfo.fname[0]) {
               break;
           }

           int checksysvolinfo = strcmp (Lfname, "System Volume Information");
           int checknewline = strcmp (Lfname, "\0");
           if (checksysvolinfo != 0 && checknewline != 0){
               char mystring [100] = {0};
               strcat(mystring, dirPath);
               strcat(mystring, Lfname);
               strcpy(&playlist[arrayindex][0], mystring);
               arrayindex++;
           }


       }
       playlistgenerationflag = false;
   }
   for (int i = 0 ; i < arrayindex+1 ; i++){
       printf ("%s\n",playlist[i]);
   }

   //Got all file names in directory!



    FRESULT mountstatus;
    FRESULT findstatus;
    FRESULT readstatus;
    int feofstatus;
    static FIL mp3file;
    FATFS myFileSystem; //my work area (filesystem object).
    BYTE buffer[512];
    UINT filereadbytescount;//dont know what this does yet.

    for (int j = 0 ; j < arrayindex+1 ; j++){
        mountstatus = f_mount((FATFS*)&myFileSystem, (const char*)"1:", 1);//force mount the drive.
        findstatus = f_open (&mp3file, playlist[j], FA_OPEN_EXISTING | FA_READ);
        readstatus = f_read(&mp3file, buffer, 512, &filereadbytescount);
        feofstatus = f_eof(&mp3file);
        currenttrack = playlist[j];
        while (findstatus == FR_OK && mountstatus == FR_OK && feofstatus != 1){
            feofstatus = f_eof(&mp3file);
            if (readstatus == FR_OK){
                    //printf ("%i\n", readstatus);
                    //printf ("%i\n", filereadbytescount);
                    //printf ("%s\n", buffer);
                    if (xQueueSend(MP3Queue, buffer, portMAX_DELAY)){
                        //printf ("Data sent.\n");
                    }
                    readstatus = f_read(&mp3file, buffer, 512, &filereadbytescount);
                if (Previous_Button.getLevel()){
                    //go to previous track.
                    vTaskDelay(100);
                    j-=2;
                    //u0_dbg_printf("previous track selected.\n");
                    break;

                }
                if (Next_Button.getLevel()){
                    vTaskDelay(100);
                    //u0_dbg_printf("next track selected.\n");
                    break;
                }
            }
            //TODO: Insert code to switch between tracks. A GPIO button should change the current i to i++/i-- depending on next/prev track respectively
        }
    }   //This for loop will constantly send data from the SD card to MP3 Decoder based on file paths in playlist array.

    return readstatus;
}

void LCDTask (void *p){
    UART.Init();
    // Line 1
    // Line 2
    char *temp = currenttrack;
    char menu1[17] = " P V+ V- Pr ^Ne ";
    UART.lcd_line_two(menu1);
    while(1){

        /* Line 1 scrolling */
        UART.lcd_line_one(temp);
        if(*(temp + 16) >= 32) {
            temp++;
        }
        else
            temp = currenttrack;

        /* Line 2 menuselect */

        vTaskDelay(1000);
        /*
        for (int i = 0; i < 20; i++){
            UART.lcd_line_one(playlist[i]);
            vTaskDelay(1000);
        }
        */
    }
}

void MP3FileReadTask(void *p){
    while(1){
        //call readMP3FILE when suitable.
        Storage::readallMP3Files();
        vTaskDelay(1000);
    }
}

void MP3DecoderReadDataTask(void *p){
    BYTE decoderbuffer[512];
    MP3Player.init();
    while(1){
        //u0_dbg_printf("afterwhile\n");
        if (xQueueReceive(MP3Queue, decoderbuffer, 1000)){
            //u0_dbg_printf("beforeplaymp3\n");
            MP3Player.playMP3Track(decoderbuffer);
            //u0_dbg_printf("afterplaymp3\n");
            //vTaskDelay(1);
            //delay_ms(1000);
        }
    }
}

//void testTask(void *p)
//{
//    MP3Player.init();
//    while(1)
//    {
//        uint16_t data = MP3Player.read_from_decoder(0x0B);
//        //u0_dbg_printf("%x\n", data);
//        vTaskDelay(1000);
//    }
//}

//Helper tasks to suspend/resume tasks at will. Not used for MP3 Project, will be cleaned.
TaskHandle_t producer_task_handle;
TaskHandle_t consumer_task_handle;

CMD_HANDLER_FUNC(taskSuspendHandler){
    char bufferforsuspendorresume [10];
    char bufferfortasknametosuspend [25];
    cmdParams.scanf("%s %s", bufferforsuspendorresume, bufferfortasknametosuspend);
    if (strcmp (bufferfortasknametosuspend, "producertask") == 0){
        vTaskSuspend(producer_task_handle);
    }
    if (strcmp (bufferfortasknametosuspend, "consumertask") == 0){
        vTaskSuspend(consumer_task_handle);
    }
    return true;
}

CMD_HANDLER_FUNC(taskResumeHandler){
    char bufferforsuspendorresume [10];
    char bufferfortasknametoresume [25];
    cmdParams.scanf("%s %s", bufferforsuspendorresume, bufferfortasknametoresume);
    if (strcmp (bufferfortasknametoresume, "producertask") == 0){
        vTaskResume(producer_task_handle);
    }

    if (strcmp (bufferfortasknametoresume, "consumertask") == 0){
        vTaskResume(consumer_task_handle);
    }

    return true;
}
// End task suspend/resume


//CMD_HANDLER_FUNC(MP3Playback)
//{
//    //cmdParams.scanf("%s");
//    if(cmdParams == "play")
//    {
//        vTaskResume(MP3PlayPause);
//    }
//    if(cmdParams == "pause")
//    {
//        vTaskSuspend(MP3PlayPause);
//    }
//    return true;
//}

void Play_Pause_Button(void *p)
{
    bool play_status = false;
    uint8_t alternate_status = 1;
    while(1)
    {
        vTaskDelay(100);
        if(Play_Pause.getLevel())
        {
            play_status = true;
        }
        else
        {
            play_status = false;
        }

        if(play_status)
        {
            if(alternate_status)
            {
                vTaskResume(MP3PlayPause);
                alternate_status--;
            }
            else
            {
                vTaskSuspend(MP3PlayPause);
                alternate_status++;
            }
        }
        vTaskDelay(1);
    }
}

void Volume_Control(void *p)
{
    bool left_vol_status = false;
    bool right_vol_status = false;
    while(1)
    {
        vTaskDelay(100);
        if(Volume_Up_Button.getLevel())
        {
            left_vol_status = true;
        }
        else if(Volume_Down_Button.getLevel())
        {
            right_vol_status = true;
        }

        if(left_vol_status)
        {
            MP3Player.volumeControl(true, false);
            left_vol_status = false;
        }
        else if(right_vol_status)
        {
            MP3Player.volumeControl(false, false);
            right_vol_status = false;
        }

    }
}

int main(int argc, char const *argv[])
{
    //TaskHandle_t sw_watchdog_task_handle;//Relocated my task handles here so I can use them in the handlers.cpp file and suspend my tasks that are running through the terminal tasks.
    //myEGH = xEventGroupCreate();//returns the id of the event group to task_watchdog
    //Sensor_queue = xQueueCreate(100, sizeof(float));
    MP3Queue = xQueueCreate(3, 512); // a Queue of depth 3 that can store 512.
    Play_Pause.setDirection(false);
    Volume_Up_Button.setDirection(false);
    Volume_Down_Button.setDirection(false);

    xTaskCreate(MP3FileReadTask, (const char*) "MP3FileReader", 1024, NULL, 2, NULL);
    xTaskCreate(MP3DecoderReadDataTask, (const char*) "Play MP3", 1024, NULL, 2, &MP3PlayPause);
    xTaskCreate(Play_Pause_Button, (const char*) "Play/Pause", 1024, NULL, 2, NULL);
    xTaskCreate(Volume_Control, (const char*) "Volume Control", 1024, NULL, 2, NULL);
	xTaskCreate(LCDTask, (const char*) "LCD Task", 1024, NULL, 2, NULL);
    //scheduler_add_task(new terminalTask(PRIORITY_HIGH));
    //scheduler_start();

    vTaskStartScheduler();
}
