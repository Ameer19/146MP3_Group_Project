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

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <stdio.h>
#include "scheduler_task.hpp"
#include "MP3DecoderDriver.hpp"


QueueHandle_t MP3Queue;
MP3Decoder MP3Player;

FRESULT Storage :: readMP3File()//const char* pFilename,  void* pData, unsigned int bytesToRead, unsigned int offset
{
    FRESULT filestatus = FR_INT_ERR; //just an initial value. Should be overwritten.
    FRESULT mountstatus;
    FRESULT readstatus;
    static FIL mp3file;
    FATFS myFileSystem; //my work area (filesystem object).
    BYTE buffer[512];
    UINT filereadbytescount;//dont know what this does yet.

    mountstatus = f_mount((FATFS*)&myFileSystem, (const char*)"1:", 1);//force mount the drive.
    filestatus = f_open (&mp3file, "1:mymp3.mp3", FA_OPEN_EXISTING | FA_READ);
    readstatus = f_read(&mp3file, buffer, 512, &filereadbytescount);

    while (filestatus == FR_OK && mountstatus == FR_OK){
        if (readstatus == FR_OK){
//            printf ("%i\n", readstatus);
//            printf ("%i\n", filereadbytescount);
//            printf ("%s\n", buffer);
            if (xQueueSend(MP3Queue, buffer, portMAX_DELAY)){
//                printf ("Data sent.\n");
            }
            readstatus = f_read(&mp3file, buffer, 512, &filereadbytescount);
        }
    }
    return readstatus;
}

void MP3FileReadTask(void *p){
    while(1){
        //call readMP3FILE when suitable.
        Storage::readMP3File();
    }
}

void SendDataToMP3DecoderTask(void *p){
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

void testTask(void *p)
{
    MP3Player.init();
    while(1)
    {
        uint16_t data = MP3Player.read_from_decoder(0x0B);
        //u0_dbg_printf("%x\n", data);
        vTaskDelay(1000);
    }
}

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


int main(int argc, char const *argv[])
{
    //TaskHandle_t sw_watchdog_task_handle;//Relocated my task handles here so I can use them in the handlers.cpp file and suspend my tasks that are running through the terminal tasks.
    //myEGH = xEventGroupCreate();//returns the id of the event group to task_watchdog
    //Sensor_queue = xQueueCreate(100, sizeof(float));
    MP3Queue = xQueueCreate(3, 512); // a Queue of depth 3 that can store 512.

    xTaskCreate(MP3FileReadTask, (const char*) "MP3FileReader", 1024, NULL, 2, NULL);
    xTaskCreate(SendDataToMP3DecoderTask, (const char*) "Play MP3", 1024, NULL, 3, NULL);
    //xTaskCreate(testTask, (const char*) "testTask", 1024, NULL, 2, NULL);
    scheduler_add_task(new terminalTask(PRIORITY_HIGH));
    scheduler_start();


    vTaskStartScheduler();
}
