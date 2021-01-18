/*
amikodev/rmt-1-esp32 - Robot mini tractor on esp-idf
Copyright © 2020 Prihodko Dmitriy - asketcnc@yandex.ru
*/

/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include <unistd.h>
// #include "esp_timer.h"
#include "nvs_flash.h"


#define ESP_INTR_FLAG_DEFAULT 0

#include "esp_r1_api.h"


#if CONFIG_RMT1_WIFI
#include "wifi.hpp"
#endif

// #include "shiftload.hpp"
// #include "r8ef.hpp"
#include "rmt1.hpp"
#include "relay.hpp"


extern "C" {
    void app_main(void);
}


// ShiftLoad
// ShiftLoad sl;
// RMT1 rmt1;


// void fch5(uint8_t value){
//     printf("channel %d: %d \n", 5, value);
//     if(value == 0){
//         sl.writeByNum(0, true);
//     } else if(value == 127){
//         sl.writeByNum(0, false);
//     }
// }

void app_main() {


    printf("Project RMT-1 ESP32 \n");
    fflush(stdout);


    // nvs flash
    esp_err_t ret;
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );



#if CONFIG_RMT1_WIFI
    // Wifi
    Wifi wifi;
    wifi.setupAP();
    // wifi.recieveBinary([](uint8_t *data, uint32_t length){
    //     printf("recieve binary length: %d \n", length);
    //     RMT1::parseWsData(data, length);
    // });
    wifi.recieveBinary(RMT1::parseWsData);

    ws_server_start();
    xTaskCreate(&Wifi::serverTask, "server_task", 3000, NULL, 9, NULL);
    xTaskCreate(&Wifi::serverHandleTask, "server_handle_task", 4000, NULL, 6, NULL);
    // xTaskCreate(&Wifi::countTask, "count_task", 6000, NULL, 2, NULL);
#endif      // CONFIG_RMT1_WIFI

    // // ShiftLoad
    // ShiftLoad sl;
    // sl.initSpi(GPIO_NUM_12, GPIO_NUM_13, GPIO_NUM_14, GPIO_NUM_15);
    // sl.registerCount(1);
    // sl.setInverse(true);
    // sl.write(0, 0x00);

    // R8EF::setPwmPins(GPIO_NUM_21, GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_5, GPIO_NUM_17, GPIO_NUM_16, GPIO_NUM_4, GPIO_NUM_2);
    // R8EF::startPwmTask();

    // RMT1::setShiftLoad(sl);

    // R8EF::funcChannel(1, RMT1::r8ef_ch1);
    // R8EF::funcChannel(2, RMT1::r8ef_ch2);
    // R8EF::funcChannel(3, RMT1::r8ef_ch3);
    // R8EF::funcChannel(4, RMT1::r8ef_ch4);
    // R8EF::funcChannel(5, RMT1::r8ef_ch5);
    // R8EF::funcChannel(6, RMT1::r8ef_ch6);
    // R8EF::funcChannel(7, RMT1::r8ef_ch7);
    // R8EF::funcChannel(8, RMT1::r8ef_ch8);

    // Relay *relay = new Relay(GPIO_NUM_15, GPIO_NUM_2, GPIO_NUM_17, GPIO_NUM_5, GPIO_NUM_18, GPIO_NUM_19, GPIO_NUM_21, GPIO_NUM_22);
    Relay *relay = new Relay(GPIO_NUM_15, GPIO_NUM_2, GPIO_NUM_4, GPIO_NUM_18, GPIO_NUM_19, GPIO_NUM_21, GPIO_NUM_22, GPIO_NUM_23);
    RMT1::setRelay(relay);


    RMT1::setEngineState(RMT1::STATE_ENGINE_OFF);
    // RMT1::turnLightTop(false);

    // RMT1::setRelayPins(GPIO_NUM_15, GPIO_NUM_2, GPIO_NUM_17, GPIO_NUM_5, GPIO_NUM_18, GPIO_NUM_19, GPIO_NUM_21, GPIO_NUM_22);

    RMT1::createScenario();
    // RMT1::runScenario(RMT1::sc1);

#if CONFIG_RMT1_WIFI
    wifi.apStaDisconnect(RMT1::clientDisconnected);
    wifi.wsDisconnect(RMT1::clientDisconnected);
#endif      // CONFIG_RMT1_WIFI

#if CONFIG_RMT1_BLUETOOTH
    // Bluetooth joystick MagicSee R1
    ESP_ERROR_CHECK(esp_r1_init());
    ESP_ERROR_CHECK(esp_r1_keyboard_register_callback(RMT1::magicseeKeyboardProcessEvent));
    ESP_ERROR_CHECK(esp_r1_device_event_register_callback(RMT1::magicseeDeviceEvent));
    ESP_ERROR_CHECK(esp_r1_enable());
#endif


}

