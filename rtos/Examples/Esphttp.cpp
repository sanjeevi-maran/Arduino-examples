#include <WiFi.h>
#include <HTTPClient.h>

#include <HTTPUpdate.h>


int http_post(String servername,Strig get_data)
{
      WiFiClient client;
      HTTPClient http;  
                                   
      Serial.println(serverName);
      Serial.println(get_data);

      http.begin(client,serverName.c_str());
      http.addHeader("accept","application/json");
      http.addHeader("Content-Type", "application/json");
      int httpsCode = http.POST(get_data);
      Serial.print("HTTP Response code: ");
      Serial.println(httpsCode);

      delay(3000);
      // Free resources
      if (httpsCode == 200) 
      {
                      // HTTP header has been send and Server response header has been handled
                      Serial.printf("[HTTP] POST... code: %d\n", httpsCode);
                      if(push_status == 2)
                      {
                          String response = http.getString();  //Get the response to the request 
                          Serial.println(response);    
                          microapiexe.micro_api_json_parse(response);
                      }
                      if(push_status == 3)
                      {
                          http.end();
                          return 9;
                      }
                      http.end();
                      return 1;    
      }
     else
      {
                     Serial.printf("[HTTP] POST... failed: %d\n", httpsCode);
                     http.end();
                     return 0; 
      }
      return(0);
      
}


String http_get_data(String get_http_url)
{
    
            HTTPClient http;
            
            HTTP_config_json();

            String serverPath = get_http_url;
            
            // Your Domain name with URL path or IP address with path
            http.begin(serverPath.c_str());
            
            // Send HTTP GET request
            int httpResponseCode = http.GET();
            
            if (httpResponseCode>0) 
            {
                Serial.print("HTTP Response code: ");
                Serial.println(httpResponseCode);
                String config_details = http.getString();
                Serial.println(config_details);
                //filesys_3.write_file("/Boodskap_config.json",config_details);
                delay(5000);
                http.end();
                return config_details;
            }
            else
            {

                Serial.print("Error code: ");
                Serial.println(httpResponseCode);
                http.end();
                return "0";
            }
            // Free resources
}


int httpcommunication :: do_ota(String OTA_URL)
{
        WiFiClient client;

        HTTP_config_json();

        digitalWrite(27, HIGH);
        digitalWrite(17, HIGH);
       // HTTP_config_json();
        Serial.println(OTA_URL);
        t_httpUpdate_return ret = httpUpdate.update(client, OTA_URL);
        // Or:
        //t_httpUpdate_return ret = httpUpdate.update(client, "server", 80, "/file.bin");
        delay(1000);

        switch (ret)
        {
 
            case HTTP_UPDATE_FAILED:
                Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
                return 3;
                //break;

            case HTTP_UPDATE_NO_UPDATES:
                Serial.println("HTTP_UPDATE_NO_UPDATES");
                return 2;
                //break;

            case HTTP_UPDATE_OK:
                Serial.println("SUCCESS HTTP_UPDATE_OK");
                delay(5000);
                return 1;
                //break;
        }

        return 8;
}


