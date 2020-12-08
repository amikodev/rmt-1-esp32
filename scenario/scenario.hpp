/*
amikodev/rmt-1-esp32 - Robot mini tractor on esp-idf
Copyright © 2020 Prihodko Dmitriy - prihdmitriy@yandex.ru
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

#ifndef __RMT1_SCENARIO_HPP__
#define __RMT1_SCENARIO_HPP__

#include <stdio.h>
#include <string.h>


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

class Scenario{

private:

public:

    /**
     * Конструктор
     */
    Scenario();

    /**
     * Запуск сценария
     */
    virtual void run() = 0;

    /**
     * Остановка сценария
     */
    virtual void stop() = 0;

};



#endif      // __RMT1_SCENARIO_HPP__

