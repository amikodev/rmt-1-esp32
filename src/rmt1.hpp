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

#ifndef __RMT_1_HPP__
#define __RMT_1_HPP__

#include <stdio.h>
#include <string.h>


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_system.h"
#include "esp_log.h"
#include "string.h"

#if CONFIG_RMT1_BLUETOOTH
#include "esp_r1_api.h"
#endif

#if CONFIG_RMT1_SHIFTLOAD
#include "shiftload.hpp"
#endif

#if CONFIG_RMT1_R8EF
#include "r8ef.hpp"
#endif

#include "relay.hpp"

#include "scenario1.hpp"
#include "scenario2.hpp"

/**
 * Робот мини трактор
 * @author Приходько Д. Г.
 */
class RMT1{

public: 

    static const uint8_t PIN_NOT_USED = 255;

    enum ControlLoadNum{
        CONTROL_LEFT_UP = 7,
        CONTROL_LEFT_DOWN = 6,
        CONTROL_RIGHT_UP = 5,
        CONTROL_RIGHT_DOWN = 4,
        CONTROL_WORK_UP = PIN_NOT_USED,
        CONTROL_WORK_DOWN = PIN_NOT_USED,

        CONTROL_ENGINE_RUN = PIN_NOT_USED,   //2,
        CONTROL_ENGINE_STARTER = PIN_NOT_USED,   //3,
        CONTROL_THROTTLE_UP = PIN_NOT_USED,
        CONTROL_THROTTLE_DOWN = PIN_NOT_USED,
        CONTROL_LIGHT_TOP = PIN_NOT_USED,    //1,

        CONTROL_WORK_DEVICE_UP = 0,
        CONTROL_WORK_DEVICE_DOWN = 1
    };

    enum StateRun{
        STATE_RUN_STOP = 0,
        STATE_RUN_FORWARD,
        STATE_RUN_BACKWARD,
        STATE_RUN_TURN
    };

    enum StateEngine{
        STATE_ENGINE_OFF,
        STATE_ENGINE_STARTER_OFF,
        STATE_ENGINE_START
    };

    enum StateIgnitionSwitch{
        STATE_IGNITION_LOW = 255,
        STATE_IGNITION_MID = 127,
        STATE_IGNITION_HIGH = 0
    };

    // enum StateWorkDevice{
    //     STATE_WORK_DEVICE_STOP = 0,
    //     STATE_WORK_DEVICE_RUN
    // };

    enum TurnType{
        TURN_STOP = 0,
        TURN_FULL,
        TURN_FORWARD,
        TURN_BACKWARD
    };

    enum ShiftLoad8PCB{
        ShiftLoad8PCB_0 = 0,
        ShiftLoad8PCB_1
    };

private:

#if CONFIG_RMT1_SHIFTLOAD
    static ShiftLoad _sl;
#endif

    static uint8_t _dxc;

    static StateRun stateRun;
    static TurnType stateTurn;
    static StateEngine stateEngine;
    static uint8_t stateWorkDevice;

    static bool lightTopState;

    static const char* LOG_NAME;

// #define FREQ_CONVERTER_WS_OBJ_NAME_ENGINE 0x50
// #define FREQ_CONVERTER_WS_OBJ_NAME_FREQ 0x51
// #define FREQ_CONVERTER_WS_OBJ_NAME_DIRECTION 0x52

// #define FREQ_CONVERTER_WS_CMD_READ 0x01
// #define FREQ_CONVERTER_WS_CMD_WRITE 0x02

// #define FREQ_CONVERTER_WS_ENGINE_STATE_RUN 0x01
// #define FREQ_CONVERTER_WS_ENGINE_STATE_STOP 0x02
    static const uint8_t WS_OBJNAME_TRACK;
    static const uint8_t WS_OBJNAME_SCENARIO;
    static const uint8_t WS_OBJNAME_ENGINE;
    static const uint8_t WS_OBJNAME_TOPLIGHT;
    static const uint8_t WS_OBJNAME_WORK_DEVICE;

    static const uint8_t WS_CMD_READ;
    static const uint8_t WS_CMD_WRITE;
    static const uint8_t WS_CMD_RUN;
    static const uint8_t WS_CMD_STOP;

    static const uint8_t WS_TRACK_1;
    static const uint8_t WS_TRACK_2;
    static const uint8_t WS_TRACK_3;
    static const uint8_t WS_TRACK_4;
    static const uint8_t WS_TRACK_5;
    static const uint8_t WS_TRACK_6;
    static const uint8_t WS_TRACK_7;
    static const uint8_t WS_TRACK_8;
    static const uint8_t WS_TRACK_9;

    static const uint8_t WS_ENGINE_OFF;
    static const uint8_t WS_ENGINE_ON;
    static const uint8_t WS_ENGINE_STARTER;

    static const uint8_t WS_TOPLIGHT_OFF;
    static const uint8_t WS_TOPLIGHT_ON;

    static const uint8_t WS_WORK_DEVICE_UP;
    static const uint8_t WS_WORK_DEVICE_DOWN;


    /**
     * Получение значения новых данных для езды
     * @param leftUp включить/выключить leftUp
     * @param leftDown включить/выключить leftDown
     * @param rightUp включить/выключить rightUp
     * @param rightDown включить/выключить rightDown
     */
    static uint8_t getDataControlDrive(bool leftUp, bool leftDown, bool rightUp, bool rightDown);


    static Relay *_relay;
    // static gpio_num_t relayPins[8];
    // static uint8_t relayData;


#if CONFIG_RMT1_BLUETOOTH
    static esp_r1_key_state_t r1StateBtn7;
    static esp_r1_key_state_t r1StateBtn8;
#endif


public:

    static Scenario1 *sc1;
    static Scenario2 *sc2;

#if CONFIG_RMT1_R8EF
    static void r8ef_ch1(uint8_t value);
    static void r8ef_ch2(uint8_t value);
    static void r8ef_ch3(uint8_t value);
    static void r8ef_ch4(uint8_t value);
    static void r8ef_ch5(uint8_t value);
    static void r8ef_ch6(uint8_t value);
    static void r8ef_ch7(uint8_t value);
    static void r8ef_ch8(uint8_t value);
#endif

#if CONFIG_RMT1_SHIFTLOAD
    /**
     * Установка сдвигового регистра с нагрузкой
     * @param sl нагрузка
     */
    static void setShiftLoad(ShiftLoad sl);
#endif

    /**
     * Поездка вперёд
     */
    static void runForward();

    /**
     * Поездка назад
     */
    static void runBackward();

    /**
     * Остановка
     */
    static void runStop();

    /**
     * Поворот налево
     * @param type тип поворота (полный, вперёд, назад)
     */
    static void turnLeft(TurnType type);

    /**
     * Поворот направо
     * @param type тип поворота (полный, вперёд, назад)
     */
    static void turnRight(TurnType type);

    /**
     * Запуск/выключение двигателя
     */
    static void setEngineState(StateEngine state);

    /**
     * Включение/выключение головного света
     * @param state статус (включить/выключить)
     */
    static void turnLightTop(bool state);

    /**
     * Парсинг входящих данных по WebSocket
     * @param data указатель на данные
     * @param length длина данных
     */
    static void parseWsData(uint8_t *data, uint32_t length);

    /**
     * Установка релейного модуля
     */
    static void setRelay(Relay *relay);
    // static void setRelayPins(gpio_num_t pin1, gpio_num_t pin2, gpio_num_t pin3, gpio_num_t pin4, gpio_num_t pin5, gpio_num_t pin6, gpio_num_t pin7, gpio_num_t pin8);

    // static void setRelayData(uint8_t data);

    /**
     * Запуск движения рабочего органа
     * @param state статус работы (запустить, остановить)
     * @param dir направление движения (вверх, вниз)
     */
    static void setWorkDeviceState(uint8_t state, uint8_t dir);

    /**
     * Остановка движения рабочего органа
     */
    static void setWorkDeviceStop();


    static void createScenario();
    static void runScenario(Scenario *sc);
    static void stopScenario(Scenario *sc);


    static void clientDisconnected();


#if CONFIG_RMT1_BLUETOOTH
    /**
     * Функция обработки нажатия кнопок джойстика Magicsee R1
     */
    static void magicseeKeyboardProcessEvent(esp_r1_keyboard_data_t *data);

    /**
     * Функция обработки событий от устройства джойстика Magicsee R1
     */
    static void magicseeDeviceEvent(enum esp_r1_device_event_e event);
#endif

};





#endif      // __RMT_1_HPP__

