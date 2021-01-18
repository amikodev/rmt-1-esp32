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

#include "scenario1.hpp"
#include "rmt1.hpp"

TaskHandle_t Scenario1::xTask1Handle = NULL;


/**
 * Запуск сценария
 */
void Scenario1::run(){
    printf("Scenario1::run \n");
    xTaskCreate(&Scenario1::task1, "Scenario1::task1", 3000, NULL, 9, &Scenario1::xTask1Handle);
}

/**
 * Остановка сценария
 */
void Scenario1::stop(){
    if(Scenario1::xTask1Handle != NULL){
        printf("Scenario1::stop \n");
        vTaskDelete(Scenario1::xTask1Handle);
        Scenario1::xTask1Handle = NULL;
        RMT1::runStop();
    }
}



void Scenario1::task1(void* pvParameters){

    enum RTYPE{
        RTYPE_STOP = 0,
        RTYPE_RUN,
        RTYPE_TURN
    };


    uint8_t cs = 0;
    RTYPE type = RTYPE_STOP;


    for(;;){

        if(type == RTYPE_STOP){
            type = RTYPE_RUN;
            cs = 0;
            RMT1::runForward();
        }

        if(type == RTYPE_RUN && cs > 10){
            type = RTYPE_TURN;
            cs = 0;
            RMT1::turnRight(RMT1::TURN_FULL);
        }

        if(type == RTYPE_TURN && cs > 20){
            type = RTYPE_RUN;
            cs = 0;
            RMT1::runForward();
        }


        cs++;

        vTaskDelay(pdMS_TO_TICKS(100));
    }

}



