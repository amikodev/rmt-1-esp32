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

#include "rmt1.hpp"

#if CONFIG_RMT1_SHIFTLOAD
ShiftLoad RMT1::_sl;
#endif

#if CONFIG_RMT1_BLUETOOTH
esp_r1_key_state_t RMT1::r1StateBtn7 = R1_KEY_RELEASED;
esp_r1_key_state_t RMT1::r1StateBtn8 = R1_KEY_RELEASED;
#endif


uint8_t RMT1::_dxc = 20;
RMT1::StateRun RMT1::stateRun = RMT1::STATE_RUN_STOP;
RMT1::TurnType RMT1::stateTurn = RMT1::TURN_STOP;
RMT1::StateEngine RMT1::stateEngine = RMT1::STATE_ENGINE_OFF;
bool RMT1::lightTopState = false;
uint8_t RMT1::stateWorkDevice = 0x04;   // == RMT1::WS_CMD_STOP

const char* RMT1::LOG_NAME = "RMT-1";

const uint8_t RMT1::WS_OBJNAME_TRACK = 0x50;
const uint8_t RMT1::WS_OBJNAME_SCENARIO = 0x51;
const uint8_t RMT1::WS_OBJNAME_ENGINE = 0x52;
const uint8_t RMT1::WS_OBJNAME_TOPLIGHT = 0x53;
const uint8_t RMT1::WS_OBJNAME_WORK_DEVICE = 0x54;

const uint8_t RMT1::WS_CMD_READ = 0x01;
const uint8_t RMT1::WS_CMD_WRITE = 0x02;
const uint8_t RMT1::WS_CMD_RUN = 0x03;
const uint8_t RMT1::WS_CMD_STOP = 0x04;

const uint8_t RMT1::WS_TRACK_1 = 0x01;
const uint8_t RMT1::WS_TRACK_2 = 0x02;
const uint8_t RMT1::WS_TRACK_3 = 0x03;
const uint8_t RMT1::WS_TRACK_4 = 0x04;
const uint8_t RMT1::WS_TRACK_5 = 0x05;
const uint8_t RMT1::WS_TRACK_6 = 0x06;
const uint8_t RMT1::WS_TRACK_7 = 0x07;
const uint8_t RMT1::WS_TRACK_8 = 0x08;
const uint8_t RMT1::WS_TRACK_9 = 0x09;

const uint8_t RMT1::WS_ENGINE_OFF = 0x00;
const uint8_t RMT1::WS_ENGINE_ON = 0x01;
const uint8_t RMT1::WS_ENGINE_STARTER = 0x02;

const uint8_t RMT1::WS_TOPLIGHT_OFF = 0x00;
const uint8_t RMT1::WS_TOPLIGHT_ON = 0x01;

const uint8_t RMT1::WS_WORK_DEVICE_UP = 0x00;
const uint8_t RMT1::WS_WORK_DEVICE_DOWN = 0x01;


Relay* RMT1::_relay = NULL;
// gpio_num_t RMT1::relayPins[8] = {GPIO_NUM_NC};
// uint8_t RMT1::relayData = 0x00;

Scenario1* RMT1::sc1 = NULL;
Scenario2* RMT1::sc2 = NULL;


#if CONFIG_RMT1_R8EF
void RMT1::r8ef_ch1(uint8_t value){
//    printf("channel %d: %d \n", 1, value);
//    j2_x
}

void RMT1::r8ef_ch2(uint8_t value){
//    printf("channel %d: %d \n", 2, value);
//    j2_y
}

void RMT1::r8ef_ch3(uint8_t value){
    // printf("channel %d: %d \n", 3, value);
//    j1_y

    if(R8EF::pwmsf[R8EF::SWA] == 127){
        // джойстик выключен
        if(stateRun != STATE_RUN_STOP){
            runStop();
        }
        return;
    }

    uint8_t x1Val = R8EF::pwmsf[R8EF::J1_X];
    if(x1Val < 191-_dxc && x1Val > 63+_dxc){    // X в среднем положении
        if(value > 191+_dxc && stateRun != STATE_RUN_FORWARD){
            // вперёд
            runForward();
        } else if(value < 191-_dxc && value > 63+_dxc && stateRun != STATE_RUN_STOP){
            // стоп
            runStop();
        } else if(value < 63-_dxc && stateRun != STATE_RUN_BACKWARD){
            // назад
            runBackward();
        }
    }

    else if(x1Val < 63-_dxc){                   // X в левом положении
        if(value > 191+_dxc && stateTurn != TURN_FORWARD){
            // поворот налево - вперёд
            turnLeft(TURN_FORWARD);
        } else if(value < 191-_dxc && value > 63+_dxc && stateTurn != TURN_FULL){
            // поворот налево - полный
            turnLeft(TURN_FULL);
        } else if(value < 63-_dxc && stateTurn != TURN_BACKWARD){
            // поворот налево - назад
            turnLeft(TURN_BACKWARD);
        }
    }

    else if(x1Val > 191+_dxc){                  // X в правом положении
        if(value > 191+_dxc && stateTurn != TURN_FORWARD){
            // поворот направо - вперёд
            turnRight(TURN_FORWARD);
        } else if(value < 191-_dxc && value > 63+_dxc && stateTurn != TURN_FULL){
            // поворот направо - полный
            turnRight(TURN_FULL);
        } else if(value < 63-_dxc && stateTurn != TURN_BACKWARD){
            // поворот направо - назад
            turnRight(TURN_BACKWARD);
        }
    }


}

void RMT1::r8ef_ch4(uint8_t value){
    // printf("channel %d: %d \n", 4, value);
//    j1_x
}

void RMT1::r8ef_ch5(uint8_t value){
    // printf("channel %d: %d \n", 5, value);
    return;
    if(value == STATE_IGNITION_LOW && stateEngine != STATE_ENGINE_OFF){
        setEngineState(STATE_ENGINE_OFF);
    } else if(value == STATE_IGNITION_MID && stateEngine != STATE_ENGINE_STARTER_OFF){
        setEngineState(STATE_ENGINE_STARTER_OFF);
    } else if(value == STATE_IGNITION_HIGH && stateEngine != STATE_ENGINE_START){
        setEngineState(STATE_ENGINE_START);
    }
}

void RMT1::r8ef_ch6(uint8_t value){
//    printf("channel %d: %d \n", 6, value);
}

void RMT1::r8ef_ch7(uint8_t value){
   printf("channel %d: %d \n", 7, value);
   if(value == 127){
       // джойстик выключен
       printf("Joystick OFF ! \n");
    //    runStop();
   } else if(value == 0){
       // включить головной свет
       turnLightTop(true);
   } else if(value == 255){
       // выключить головной свет
       turnLightTop(false);
   }
}

void RMT1::r8ef_ch8(uint8_t value){
//    printf("channel %d: %d \n", 8, value);
}
#endif      // CONFIG_RMT1_R8EF

#if CONFIG_RMT1_SHIFTLOAD
/**
 * Установка сдвигового регистра с нагрузкой
 * @param sl нагрузка
 */
void RMT1::setShiftLoad(ShiftLoad sl){
    _sl = sl;
}
#endif      // CONFIG_RMT1_SHIFTLOAD

/**
 * Поездка вперёд
 */
void RMT1::runForward(){
    stateRun = STATE_RUN_FORWARD;
    stateTurn = TURN_STOP;
    printf("runForward \n");
    // _sl.write(ShiftLoad8PCB_0, getDataControlDrive(true, false, true, false));
    _relay->setData(getDataControlDrive(true, false, true, false));
}

/**
 * Поездка назад
 */
void RMT1::runBackward(){
    stateRun = STATE_RUN_BACKWARD;
    stateTurn = TURN_STOP;
    printf("runBackward \n");
    // _sl.write(ShiftLoad8PCB_0, getDataControlDrive(false, true, false, true));
    _relay->setData(getDataControlDrive(false, true, false, true));
}

/**
 * Остановка
 */
void RMT1::runStop(){
    stateRun = STATE_RUN_STOP;
    stateTurn = TURN_STOP;
    printf("runStop \n");
    // _sl.write(ShiftLoad8PCB_0, getDataControlDrive(false, false, false, false));
    _relay->setData(getDataControlDrive(false, false, false, false));
}

/**
 * Поворот налево
 * @param type тип поворота (полный, вперёд, назад)
 */
void RMT1::turnLeft(TurnType type){
    stateRun = STATE_RUN_TURN;
    stateTurn = type;

    printf("turnLeft: %d \n", type);
    if(type == TURN_FULL){
        // _sl.write(ShiftLoad8PCB_0, getDataControlDrive(false, true, true, false));
        _relay->setData(getDataControlDrive(false, true, true, false));
    } else if(type == TURN_FORWARD){
        // _sl.write(ShiftLoad8PCB_0, getDataControlDrive(false, false, true, false));
        _relay->setData(getDataControlDrive(false, false, true, false));
    } else if(type == TURN_BACKWARD){
        // _sl.write(ShiftLoad8PCB_0, getDataControlDrive(false, true, false, false));
        _relay->setData(getDataControlDrive(false, true, false, false));
    }
}

/**
 * Поворот направо
 * @param type тип поворота (полный, вперёд, назад)
 */
void RMT1::turnRight(TurnType type){
    stateRun = STATE_RUN_TURN;
    stateTurn = type;

    printf("turnRight: %d \n", type);
    if(type == TURN_FULL){
        // _sl.write(ShiftLoad8PCB_0, getDataControlDrive(true, false, false, true));
        _relay->setData(getDataControlDrive(true, false, false, true));
    } else if(type == TURN_FORWARD){
        // _sl.write(ShiftLoad8PCB_0, getDataControlDrive(true, false, false, false));
        _relay->setData(getDataControlDrive(true, false, false, false));
    } else if(type == TURN_BACKWARD){
        // _sl.write(ShiftLoad8PCB_0, getDataControlDrive(false, false, false, true));
        _relay->setData(getDataControlDrive(false, false, false, true));
    }
}

/**
 * Запуск/выключение двигателя
 */
void RMT1::setEngineState(StateEngine state){
    if(stateEngine != state){
        printf("engine state: %d \n", state);

        if(state == STATE_ENGINE_OFF){
            // _sl.writeByNum(ShiftLoad8PCB_1, CONTROL_ENGINE_RUN, false);
            // _sl.writeByNum(ShiftLoad8PCB_1, CONTROL_ENGINE_STARTER, false);
            // gpio_set_level(relayPins[CONTROL_ENGINE_RUN], 1);
            // gpio_set_level(relayPins[CONTROL_ENGINE_STARTER], 1);
            _relay->writeByNum(CONTROL_ENGINE_RUN, 0);
            _relay->writeByNum(CONTROL_ENGINE_STARTER, 0);
        } else if(state == STATE_ENGINE_START){
            // _sl.writeByNum(ShiftLoad8PCB_1, CONTROL_ENGINE_RUN, true);
            // _sl.writeByNum(ShiftLoad8PCB_1, CONTROL_ENGINE_STARTER, true);
            // gpio_set_level(relayPins[CONTROL_ENGINE_RUN], 0);
            // gpio_set_level(relayPins[CONTROL_ENGINE_STARTER], 0);
            _relay->writeByNum(CONTROL_ENGINE_RUN, 1);
            _relay->writeByNum(CONTROL_ENGINE_STARTER, 1);
        } else if(state == STATE_ENGINE_STARTER_OFF){
            // _sl.writeByNum(ShiftLoad8PCB_1, CONTROL_ENGINE_RUN, true);
            // _sl.writeByNum(ShiftLoad8PCB_1, CONTROL_ENGINE_STARTER, false);
            // gpio_set_level(relayPins[CONTROL_ENGINE_RUN], 0);
            // gpio_set_level(relayPins[CONTROL_ENGINE_STARTER], 1);
            _relay->writeByNum(CONTROL_ENGINE_RUN, 1);
            _relay->writeByNum(CONTROL_ENGINE_STARTER, 0);
        }

        stateEngine = state;
    }

}

/**
 * Получение значения новых данных для езды
 * @param leftUp включить/выключить leftUp
 * @param leftDown включить/выключить leftDown
 * @param rightUp включить/выключить rightUp
 * @param rightDown включить/выключить rightDown
 */
uint8_t RMT1::getDataControlDrive(bool leftUp, bool leftDown, bool rightUp, bool rightDown){
    // uint8_t sd = _sl.getData(ShiftLoad8PCB_0);
    uint8_t sd = _relay->getData();

    if((leftUp && leftDown) || (rightUp && rightDown)){
        // защита от одновременного включения вперёд и назад
        return sd;
    }

    if(leftUp) sd |= 1<<CONTROL_LEFT_UP; else sd &= ~(1<<CONTROL_LEFT_UP);
    if(leftDown) sd |= 1<<CONTROL_LEFT_DOWN; else sd &= ~(1<<CONTROL_LEFT_DOWN);
    if(rightUp) sd |= 1<<CONTROL_RIGHT_UP; else sd &= ~(1<<CONTROL_RIGHT_UP);
    if(rightDown) sd |= 1<<CONTROL_RIGHT_DOWN; else sd &= ~(1<<CONTROL_RIGHT_DOWN);
    return sd;
}

/**
 * Включение/выключение головного света
 * @param state статус (включить/выключить)
 */
void RMT1::turnLightTop(bool state){
    if(lightTopState != state){
        // _sl.writeByNum(ShiftLoad8PCB_1, CONTROL_LIGHT_TOP, state);
        // gpio_set_level(relayPins[CONTROL_LIGHT_TOP], state ? 0 : 1);
        _relay->writeByNum(CONTROL_LIGHT_TOP, state ? 1 : 0);
        lightTopState = state;
    }

}

/**
 * Парсинг входящих данных по WebSocket
 * @param data указатель на данные
 * @param length длина данных
 */
void RMT1::parseWsData(uint8_t *data, uint32_t length){
    printf("recieve binary length: %d \n", length);
    // вывод входящих данных
    char *s = (char *) malloc(length*2+1);
    for (size_t i=0; i<length; i++)
        sprintf(s + i*2, "%02X ", *(data+i));

    ESP_LOG_BUFFER_HEXDUMP(LOG_NAME, s, length, ESP_LOG_INFO);
    free(s);

    // обработка входящих данных
    if(length == 16){
        if(*(data) == WS_OBJNAME_TRACK){
            if(*(data+1) == WS_CMD_WRITE){
                uint8_t value = *(data+2);
                printf("WS_OBJNAME_TRACK value: %d \n", value);
                if(value == 1) turnLeft(TURN_FORWARD);
                else if(value == 2) runForward();
                else if(value == 3) turnRight(TURN_FORWARD);
                else if(value == 4) turnLeft(TURN_FULL);
                else if(value == 5) runStop();
                else if(value == 6) turnRight(TURN_FULL);
                else if(value == 7) turnLeft(TURN_BACKWARD);
                else if(value == 8) runBackward();
                else if(value == 9) turnRight(TURN_BACKWARD);

                stopScenario(sc1);
                stopScenario(sc2);
            }
        }
        else if(*(data) == WS_OBJNAME_SCENARIO){
            if(*(data+1) == WS_CMD_RUN){
                uint8_t value = *(data+2);
                printf("WS_OBJNAME_SCENARIO [RUN] value: %d \n", value);
                if(value == 1) runScenario(sc1);
                else if(value == 2) runScenario(sc2);
            }
            else if(*(data+1) == WS_CMD_STOP){
                uint8_t value = *(data+2);
                printf("WS_OBJNAME_SCENARIO [STOP] value: %d \n", value);
                if(value == 1) stopScenario(sc1);
                else if(value == 2) stopScenario(sc2);
            }
        }
        else if(*(data) == WS_OBJNAME_ENGINE){
            if(*(data+1) == WS_CMD_WRITE){
                uint8_t value = *(data+2);
                printf("WS_OBJNAME_ENGINE value: %d \n", value);
                if(value == WS_ENGINE_OFF) setEngineState(STATE_ENGINE_OFF);
                if(value == WS_ENGINE_ON) setEngineState(STATE_ENGINE_STARTER_OFF);
                if(value == WS_ENGINE_STARTER) setEngineState(STATE_ENGINE_START);
            }
        }
        else if(*(data) == WS_OBJNAME_TOPLIGHT){
            if(*(data+1) == WS_CMD_WRITE){
                uint8_t value = *(data+2);
                printf("WS_OBJNAME_TOPLIGHT value: %d \n", value);
                if(value == WS_TOPLIGHT_OFF) turnLightTop(false);
                else if(value == WS_TOPLIGHT_ON) turnLightTop(true);
            }
        }
        else if(*(data) == WS_OBJNAME_WORK_DEVICE){
            uint8_t state = *(data+1);
            if(state == WS_CMD_RUN){
                uint8_t dir = *(data+2);
                setWorkDeviceState(state, dir);
            } else if(state == WS_CMD_STOP){
                setWorkDeviceStop();
            }
        }
    }
}

/**
 * Установка релейного модуля
 */
void RMT1::setRelay(Relay *relay){
    _relay = relay;
}

/**
 * Запуск движения рабочего органа
 * @param state статус работы (запустить, остановить)
 * @param dir направление движения (вверх, вниз)
 */
void RMT1::setWorkDeviceState(uint8_t state, uint8_t dir){
    if(stateWorkDevice != state){
        printf("work device state: %d, dir: %d \n", state, dir);
        if(state == WS_CMD_RUN){
            if(dir == WS_WORK_DEVICE_UP){
                _relay->writeByNum(CONTROL_WORK_DEVICE_DOWN, 0);
                _relay->writeByNum(CONTROL_WORK_DEVICE_UP, 1);
            } else if(dir == WS_WORK_DEVICE_DOWN){
                _relay->writeByNum(CONTROL_WORK_DEVICE_UP, 0);
                _relay->writeByNum(CONTROL_WORK_DEVICE_DOWN, 1);
            }
        } else if(state == WS_CMD_STOP){
            _relay->writeByNum(CONTROL_WORK_DEVICE_UP, 0);
            _relay->writeByNum(CONTROL_WORK_DEVICE_DOWN, 0);
        }

        // runStop();

        stateWorkDevice = state;
    }


}

/**
 * Остановка движения рабочего органа
 */
void RMT1::setWorkDeviceStop(){
    printf("work device STOP \n");

    _relay->writeByNum(CONTROL_WORK_DEVICE_UP, 0);
    _relay->writeByNum(CONTROL_WORK_DEVICE_DOWN, 0);

    // runStop();

    stateWorkDevice = WS_CMD_STOP;
}



// void RMT1::setRelayPins(gpio_num_t pin1, gpio_num_t pin2, gpio_num_t pin3, gpio_num_t pin4, gpio_num_t pin5, gpio_num_t pin6, gpio_num_t pin7, gpio_num_t pin8){
//     relayPins[0] = pin1;
//     relayPins[1] = pin2;
//     relayPins[2] = pin3;
//     relayPins[3] = pin4;
//     relayPins[4] = pin5;
//     relayPins[5] = pin6;
//     relayPins[6] = pin7;
//     relayPins[7] = pin8;

//     uint64_t bitMask = 0;
//     for(uint8_t i=0; i<8; i++){
//         if(relayPins[i] > GPIO_NUM_NC){
//             bitMask |= (1Ull << relayPins[i]);
//         }
//     }

//     gpio_config_t io_conf;
//     // io_conf.intr_type = GPIO_INTR_POSEDGE;
//     io_conf.mode = GPIO_MODE_OUTPUT;
//     io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
//     // io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
//     io_conf.pin_bit_mask = bitMask;
//     gpio_config(&io_conf);

//     for(uint8_t i=0; i<8; i++){
//         if(relayPins[i] > GPIO_NUM_NC){
//             gpio_set_level(relayPins[i], 1);
//         }
//     }


// }

// void RMT1::setRelayData(uint8_t data){
//     relayData = data;

//     for(uint8_t i=0; i<8; i++){
//         if(relayPins[i] > GPIO_NUM_NC){
//             if( ((relayData >> i) & 0x01) == 0 ){
//                 gpio_set_level(relayPins[i], 1);
//             } else{
//                 gpio_set_level(relayPins[i], 0);
//             }
//         }
//     }

// }

void RMT1::createScenario(){
    sc1 = new Scenario1();
    sc2 = new Scenario2();
}

void RMT1::runScenario(Scenario *sc){
    if(sc != NULL){
        sc->run();
    }
}

void RMT1::stopScenario(Scenario *sc){
    if(sc != NULL){
        sc->stop();
    }
}

void RMT1::clientDisconnected(){
    printf("RMT1::clientDisconnected \n");

    stopScenario(sc1);
    stopScenario(sc2);

    runStop();

}

#if CONFIG_RMT1_BLUETOOTH
    /**
     * Функция обработки нажатия кнопок джойстика Magicsee R1
     */
void RMT1::magicseeKeyboardProcessEvent(esp_r1_keyboard_data_t *data){
    // ESP_LOGI(TAG, "keyboard event: %s in state %d"          
    //             , r1_event_name[data->id], data->state);
    // printf("magicseeKeyboardProcessEvent event: %s in state %d \n", r1_event_name[data->id], data->state);
    printf("magicseeKeyboardProcessEvent event: %d in state %d \n", data->id, data->state);

    switch(data->id){
        case R1_BUTTON4:        // кнопка B
            if(r1StateBtn7 == R1_KEY_PRESSED){
                if(data->state == R1_KEY_PRESSED){
                    // запуск сценария 1
                    runScenario(sc1);
                }
            } else{
                if(data->state == R1_KEY_PRESSED){
                    // рабочая навеска - поднять
                    setWorkDeviceState(WS_CMD_RUN, WS_WORK_DEVICE_UP);
                } else{
                    setWorkDeviceStop();
                }
            }
            break;
        case R1_BUTTON5:        // кнопка D
            if(r1StateBtn7 == R1_KEY_PRESSED){
                if(data->state == R1_KEY_PRESSED){
                    // запуск сценария 2
                    runScenario(sc2);
                }
            } else{
                if(data->state == R1_KEY_PRESSED){
                    // рабочая навеска - опустить
                    setWorkDeviceState(WS_CMD_RUN, WS_WORK_DEVICE_DOWN);
                } else{
                    setWorkDeviceStop();
                }
            }
            break;

        case R1_BUTTON2:        // кнопка A
            if(data->state == R1_KEY_PRESSED){
                // остановить движение и запущенные сценарии
                stopScenario(sc1);
                stopScenario(sc2);

                runStop();
            }
            break;

        case R1_BUTTON7:        // нижняя кнопка 
            r1StateBtn7 = data->state;
            break;
        case R1_BUTTON8:        // верхняя кнопка 
            r1StateBtn8 = data->state;
            break;
        case R1_AXIS_X:         // джойстик X
            if(data->state == R1_AXIS_PLUS){
                // поворот налево
                if(r1StateBtn8 == R1_KEY_PRESSED){
                    turnLeft(TURN_FORWARD);
                } else{
                    turnLeft(TURN_FULL);
                }
            } else if(data->state == R1_AXIS_CENTER){
                // остановить движение
                runStop();
            } else if(data->state == R1_AXIS_MINUS){
                // поворот направо
                if(r1StateBtn8 == R1_KEY_PRESSED){
                    turnRight(TURN_FORWARD);
                } else{
                    turnRight(TURN_FULL);
                }
            }
            break;
        case R1_AXIS_Y:         // джойстик Y
            if(data->state == R1_AXIS_PLUS){
                // движение вперёд
                runForward();
            } else if(data->state == R1_AXIS_CENTER){
                // остановить движение
                runStop();
            } else if(data->state == R1_AXIS_MINUS){
                // движение назад
                runBackward();
            }
            break;
        default:
            break;
    }

}

/**
 * Функция обработки событий от устройства джойстика Magicsee R1
 */
void RMT1::magicseeDeviceEvent(enum esp_r1_device_event_e event){
    switch (event) {
        case R1_EVENT_DISCONNECTED:
            printf("magicseeDeviceEvent event: disconnected \n");
            r1StateBtn8 = R1_KEY_RELEASED;
            clientDisconnected();
            break;

        case R1_EVENT_CONNECTED:
            printf("magicseeDeviceEvent event: connected \n");
            break;
    }

}


#endif      // CONFIG_RMT1_BLUETOOTH

