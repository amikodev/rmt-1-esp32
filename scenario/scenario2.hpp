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

#ifndef __RMT1_SCENARIO_2_HPP__
#define __RMT1_SCENARIO_2_HPP__

#include "scenario.hpp"

class Scenario2 : public Scenario{

private:


public:


    static TaskHandle_t xTask1Handle;

    /**
     * Запуск сценария
     */
    void run();

    /**
     * Остановка сценария
     */
    void stop();

    static void task1(void* pvParameters);

};



#endif      // __RMT1_SCENARIO_2_HPP__

