#include "Arduino.h"
/* mutexes or semaphores*/



QueueHandle_t sendreceivingdata; //Declare a queue to pass client status
TaskHandle_t TASK1_Handle,Task2_Handle;

void TASK1(void *pvParameters)
{
      char sendchar[100] = "hello world";

      while (1)
      {
          
            Serial.println("i am task1");
            xQueueSend(sendreceivingdata, sendchar, portMAX_DELAY);
            vTaskDelay(1000/portTICK_PERIOD_MS);
             
      }
}

void TASK2(void *pvParameters)
{

     while(1)
     {
                  char receivedNumber[100];
                  int Queuecount = uxQueueMessagesWaiting(sendreceivingdata); //for stored Queue count
                  if(Queuecount > 0)
                  {
                        if (xQueueReceive(sendreceivingdata, &receivedNumber, portMAX_DELAY) == pdTRUE)
                        {
                                Serial.print("Received String: ");
                                Serial.println(receivedNumber);
                        }
                  }

                   vTaskDelay(100/portTICK_PERIOD_MS);
     }

}


void setup()
{
     
      Serial.begin(9600);
      
      sendreceivingdata = xQueueCreate(100, 350); // Create a queue of size 1 to pass clientStatus - (100) 35000 bytes

      /*************************************RTOS task********************************************/
       //task function        taskname            ram size         priority  task handle , cpu
       xTaskCreatePinnedToCore(TASK1 , "TASK1" , 10000   , NULL , 1      , &TASK1_Handle ,0);
       xTaskCreatePinnedToCore(TASK2 , "TASK2" , 10000   , NULL , 2      , &Task2_Handle ,1);
      /******************************************************************************************/
}

void loop()
{
      
}

