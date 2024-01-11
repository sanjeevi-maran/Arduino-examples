#include "Arduino.h"
#include <PubSubClient.h >
#include <WiFi.h>
#include <ArduinoJson.h>

/* mutexes or semaphores*/

/*************************************************************************************************************************/

#define adc 32 // Enter the GPIO pin

/********************************************************************************************************/
#define MQTT_SERVER ""      // Enter Your MQTT Server
#define MQTT_PORT 1883                    // Enter Your MQTT PORT
#define SSID ""                   // Enter Your WIFI Name
#define PASSWORD ""               // Enter Your WIFI Password
#define MSG_ID 10000                      // Enter Your Message ID
#define DEVICE_TOKEN "" // Enter Your Boodskap Platform token

String clientid = DEVICE_TOKEN;                                       // Enter your Token
String topic = "/" + String(DEVICE_TOKEN) + "/pub/" + String(MSG_ID); // topic = "/YOUR_DEVICE_TOKEN/pub/YOUR MESSSAGE ID"

/************************************************************************************************************/

WiFiClient espClient;
PubSubClient client(espClient);

QueueHandle_t sendreceivingdata; // Declare a queue to pass client status
// TaskHandle_t Boodskapwebserver_Handle, Boodskappublish_Handle, BoodskapSerialrun_Handle; //for task run suspend task
TaskHandle_t TASK1_Handle, Task2_Handle;

void TASK1(void *pvParameters)
{

  uint32_t lastReport = 0;
  uint32_t Reporting_Interval = 1000 * 5;
  char sendchar[100];
  while (1)
  {
    float t = analogRead(adc);

    StaticJsonDocument<256> tvalue;
    tvalue["t"] = t;
    String tdata;
    serializeJson(tvalue, tdata);
    // Serial.println(tdata);

    if ((millis() - lastReport) >= Reporting_Interval)
    {
      strcpy(sendchar, tdata.c_str());
      xQueueSend(sendreceivingdata, sendchar, portMAX_DELAY);
      lastReport = millis();
      // delay(2000);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void TASK2(void *pvParameters)
{

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);

  WiFi.begin(SSID, PASSWORD);
  char receiveddata[100];

  client.setServer(MQTT_SERVER, MQTT_PORT);

  while (1)
  {

    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("wifi connected");
      delay(100);

      if (client.connected() == 1)
      {
        Serial.println("mqtt connected");
        int Queuecount = uxQueueMessagesWaiting(sendreceivingdata); // for stored Queue count

        String incomingLine;

        if (Queuecount > 0)
        {

          if (xQueueReceive(sendreceivingdata, &receiveddata, portMAX_DELAY) == pdTRUE)
          {

            // String incomingLine = String((char*)receiveddata);
            // Serial.print("Received String: ");
            // Serial.println(incomingLine);
            incomingLine = String(receiveddata);
          }
        }
        else
        {
          vTaskDelay(100 / portTICK_PERIOD_MS);
        }

        Serial.print("Received String: ");
        Serial.println(incomingLine);
        if (incomingLine.length() > 0)
        {
          bool publishstate = client.publish(topic.c_str(), incomingLine.c_str(), 1); // 1 is qos
          Serial.println(publishstate);
          vTaskDelay(100 / portTICK_PERIOD_MS);
        }
      }
      else
      {
        Serial.println("Client not connected ....");
        Serial.println(clientid);
        client.connect("DEVOSEZHQGRunWSFXH");
        vTaskDelay(10000 / portTICK_PERIOD_MS);
      }
    }
    else
    {
      Serial.println("wifi connecting....");
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup()
{

  Serial.begin(9600);
  /*****************************************************************/
  pinMode(adc, INPUT);
  sendreceivingdata = xQueueCreate(100, 350); // Create a queue of size 1 to pass clientStatus - (100) 35000 bytes
                                              /*************************************RTOS task********************************************/
  // task function        taskname            ram size         priority  task handle , cpu
  xTaskCreatePinnedToCore(TASK1, "TASK1", 15000, NULL, 1, &TASK1_Handle, 0);
  xTaskCreatePinnedToCore(TASK2, "TASK2", 15000, NULL, 2, &Task2_Handle, 1);
  /******************************************************************************************/
}

void loop()
{
}
