/**
 * Application ESP32
 * 
 * @author Prihodko D. G.
 * @copyright CB Asket
 * 
 */

var app = (function(){

    const OBJ_NAME_TRACK = 0x50;
    const OBJ_NAME_SCENARIO = 0x51;
    const OBJ_NAME_ENGINE = 0x52;
    const OBJ_NAME_TOPLIGHT = 0x53;
    const OBJ_NAME_WORK_DEVICE = 0x54;

    const CMD_READ = 0x01;
    const CMD_WRITE = 0x02;
    const CMD_RUN = 0x03;
    const CMD_STOP = 0x04;


    const STATE_RUN_STOP = 0;
    const STATE_RUN_FORWARD = 1;
    const STATE_RUN_BACKWARD = 2;
    const STATE_RUN_TURN = 3;

    const TURN_STOP = 0;
    const TURN_FULL = 1;
    const TURN_FORWARD = 2;
    const TURN_BACKWARD = 3;

    const ENGINE_OFF = 0;
    const ENGINE_ON = 1;
    const ENGINE_STARTER = 2;

    const TOPLIGHT_OFF = 0;
    const TOPLIGHT_ON = 1;

    const WORK_DEVICE_UP = 0;
    const WORK_DEVICE_DOWN = 1;


    var stateRun = STATE_RUN_STOP;
    var stateTurn = TURN_STOP;
    var stateEngine = ENGINE_OFF;
    var stateTopLight = TOPLIGHT_OFF;




    var debug = document.getElementById('debug');

    var websocket = null;


    const outDebug = text => {
        let date = (new Date()).getTime();
        debug.innerHTML = "["+date+"] "+ text + "\n" + debug.innerHTML;
    }

    const prepareData = (data) => {
        const size = 16;
        let byteArray = new Uint8Array(size);
        for(let i=0; i<size; i++){
            byteArray[i] = 0x00;
        }

        if(data !== null && typeof data === "object"){
            data.map((val, ind) => {
                byteArray[ind] = val & 0xFF;
            });
        }

        return byteArray.buffer;
    }


    // установка направления движения
    const setActiveBtnTrack = (targetBtn) => {
        [...document.querySelectorAll('.btnTrack')].map(btn => {
            btn.classList.remove('active');
        });
        targetBtn.classList.add('active');
    }

    [...document.querySelectorAll('.btnTrack')].map(btn => {
        btn.ontouchstart = (event) => {
            let value = event.target.dataset['value'];
            outDebug('set direction: '+value);
            setActiveBtnTrack(btn);
            websocket.send(prepareData([ OBJ_NAME_TRACK, CMD_WRITE, (value) & 0xFF ]));

        }
        btn.ontouchend = (event) => {
            runStop();
        }
        // btn.onclick = (event) => {
        //     let value = event.target.dataset['value'];
        //     outDebug('set direction: '+value);
        //     setActiveBtnTrack(btn);
        //     websocket.send(prepareData([ OBJ_NAME_TRACK, CMD_WRITE, (value) & 0xFF ]));
        // }
    });

    // запуск сценария
    [...document.querySelectorAll('.btnScenario')].map(btn => {
        let state = false;
        btn.onclick = (event) => {
            let value = event.target.dataset['value'];
            // setActiveBtnTrack(btn);
            let type = !state ? CMD_RUN : CMD_STOP;
            if(!state){
                btn.classList.add('active');
                outDebug('scenario run: '+value);
            } else{
                btn.classList.remove('active');
                outDebug('scenario stop: '+value);
            }
            state = !state;
            websocket.send(prepareData([ OBJ_NAME_SCENARIO, type, (value) & 0xFF ]));
        }
    });



    // движение вперёд
    const runForward = () => {
        stateRun = STATE_RUN_FORWARD;
        stateTurn = TURN_STOP;
        let value = 2;
        let btn = document.querySelector('.btnTrack[data-value="'+value+'"]');
        setActiveBtnTrack(btn);
        websocket.send(prepareData([ OBJ_NAME_TRACK, CMD_WRITE, (value) & 0xFF ]));
    }

    // движение назад
    const runBackward = () => {
        stateRun = STATE_RUN_BACKWARD;
        stateTurn = TURN_STOP;
        let value = 8;
        let btn = document.querySelector('.btnTrack[data-value="'+value+'"]');
        setActiveBtnTrack(btn);
        websocket.send(prepareData([ OBJ_NAME_TRACK, CMD_WRITE, (value) & 0xFF ]));
    }

    // остановка движения
    const runStop = () => {
        stateRun = STATE_RUN_STOP;
        stateTurn = TURN_STOP;
        let value = 5;
        let btn = document.querySelector('.btnTrack[data-value="'+value+'"]');
        setActiveBtnTrack(btn);
        websocket.send(prepareData([ OBJ_NAME_TRACK, CMD_WRITE, (value) & 0xFF ]));
        
    }

    // поворот налево
    const turnLeft = (type) => {
        stateRun = STATE_RUN_TURN;
        stateTurn = type;
        let value = null;
        if(type === TURN_FULL) value = 4;
        else if(type === TURN_FORWARD) value = 1;
        else if(type === TURN_BACKWARD) value = 7;
        let btn = document.querySelector('.btnTrack[data-value="'+value+'"]');
        setActiveBtnTrack(btn);
        websocket.send(prepareData([ OBJ_NAME_TRACK, CMD_WRITE, (value) & 0xFF ]));
    }

    // поворот направо
    const turnRight = (type) => {
        stateRun = STATE_RUN_TURN;
        stateTurn = type;
        let value = null;
        if(type === TURN_FULL) value = 6;
        else if(type === TURN_FORWARD) value = 3;
        else if(type === TURN_BACKWARD) value = 9;
        let btn = document.querySelector('.btnTrack[data-value="'+value+'"]');
        setActiveBtnTrack(btn);
        websocket.send(prepareData([ OBJ_NAME_TRACK, CMD_WRITE, (value) & 0xFF ]));
    }



    // Joystick
    [...document.querySelectorAll('.joystick-container')].map(joystickContainer => {

        let joystick = joystickContainer.querySelector('.joystick');
        let pointer = joystickContainer.querySelector('.pointer');
        // console.log(pointer);

        let pw = pointer.clientWidth;
        let jw = joystick.clientWidth
        // console.log(w);

        const setPointerPosition = (x, y) => {

            x = x-pw/2;
            y = y-pw/2;
            if(x < 0) x = 0; else if(x >= jw-pw) x = jw-pw; 
            if(y < 0) y = 0; else if(y >= jw-pw) y = jw-pw; 

            pointer.style.left = x +"px";
            pointer.style.top = y +"px";
        }

        setPointerPosition(jw/2, jw/2);

        joystick.ontouchmove = (event) => {
            event.stopPropagation();
            event.preventDefault();
            let touch = event.changedTouches[0];
            var x = touch.clientX - joystick.getBoundingClientRect().left;
            var y = touch.clientY - joystick.getBoundingClientRect().top;
            if(x >= 0 && x <= 511 && y >= 0 && y <= 511){
                setPointerPosition(x, y);

                y = 511 - y;

                x /= 2;
                y /= 2;
                x = parseInt(x);
                y = parseInt(y);
                var coords = "X coords: " + x + ", Y coords: " + y;
                // outDebug(coords);   

                
                let _dxc = 20;
                if(x < 191-_dxc && x > 63+_dxc){    // X в среднем положении
                    if(y > 191+_dxc && stateRun != STATE_RUN_FORWARD){
                        // вперёд
                        runForward();
                    } else if(y < 191-_dxc && y > 63+_dxc && stateRun != STATE_RUN_STOP){
                        // стоп
                        runStop();
                    } else if(y < 63-_dxc && stateRun != STATE_RUN_BACKWARD){
                        // назад
                        runBackward();
                    }
                }

                else if(x < 63-_dxc){                   // X в левом положении
                    if(y > 191+_dxc && stateTurn != TURN_FORWARD){
                        // поворот налево - вперёд
                        turnLeft(TURN_FORWARD);
                    } else if(y < 191-_dxc && y > 63+_dxc && stateTurn != TURN_FULL){
                        // поворот налево - полный
                        turnLeft(TURN_FULL);
                    } else if(y < 63-_dxc && stateTurn != TURN_BACKWARD){
                        // поворот налево - назад
                        turnLeft(TURN_BACKWARD);
                    }
                }
            
                else if(x > 191+_dxc){                  // X в правом положении
                    if(y > 191+_dxc && stateTurn != TURN_FORWARD){
                        // поворот направо - вперёд
                        turnRight(TURN_FORWARD);
                    } else if(y < 191-_dxc && y > 63+_dxc && stateTurn != TURN_FULL){
                        // поворот направо - полный
                        turnRight(TURN_FULL);
                    } else if(y < 63-_dxc && stateTurn != TURN_BACKWARD){
                        // поворот направо - назад
                        turnRight(TURN_BACKWARD);
                    }
                }
            

            }
        };

        joystick.ontouchend = event => {
            console.log("touch end");
            setPointerPosition(jw/2, jw/2);
            runStop();
        };


        // событие на сворачивание окна
        // останавливаем работу
        document.addEventListener('visibilitychange', event => {
            if(document.hidden){
                outDebug('visibilitychange: hidden');
                runStop();
            } else{

            }
        });


    });


    // двигатель

    const setEngineState = (btn, state) => {
        stateEngine = state;
        switch(state){
            case ENGINE_OFF:
                outDebug("Engine off");
                btn.classList.remove("on");
                btn.classList.remove("starter");
                btn.innerText = "Выключен";
                break;
            case ENGINE_ON:
                outDebug("Engine on");
                btn.classList.add("on");
                btn.classList.remove("starter");
                btn.innerText = "Включен";
                break;
            case ENGINE_STARTER:
                outDebug("Engine starter");
                btn.classList.remove("on");
                btn.classList.add("starter");
                btn.innerText = "Стартер";
                break;
        }
        // console.log(websocket);
        if(websocket !== null)
            websocket.send(prepareData([ OBJ_NAME_ENGINE, CMD_WRITE, (state) & 0xFF ]));
    };

    [...document.querySelectorAll('.engine-state')].map(btn => {
        btn.ontouchstart = (event) => {
            console.log('engine ontouchstart');
            if(stateEngine == ENGINE_OFF){
                setEngineState(btn, ENGINE_STARTER);
            } else if(stateEngine == ENGINE_ON){
                setEngineState(btn, ENGINE_OFF);
            }
        };
        btn.ontouchend = (event) => {
            console.log('engine ontouchend');
            if(stateEngine == ENGINE_STARTER){
                setEngineState(btn, ENGINE_ON);
            }
        };

        setEngineState(btn, ENGINE_OFF);
    });



    // верхний свет
    const setTopLightState = (btn, state) => {
        stateTopLight = state;
        switch(state){
            case TOPLIGHT_OFF:
                outDebug("TopLight off");
                btn.classList.remove("on");
                // btn.innerText = "Выключен";
                break;
            case TOPLIGHT_ON:
                outDebug("TopLight on");
                btn.classList.add("on");
                // btn.innerText = "Включен";
                break;
        }
        if(websocket !== null)
            websocket.send(prepareData([ OBJ_NAME_TOPLIGHT, CMD_WRITE, (state) & 0xFF ]));
    };

    [...document.querySelectorAll('.toplight-state')].map(btn => {
        btn.ontouchstart = (event) => {
            console.log('toplight ontouchstart');
            if(stateTopLight == TOPLIGHT_OFF){
                setTopLightState(btn, TOPLIGHT_ON);
            } else if(stateTopLight == TOPLIGHT_ON){
                setTopLightState(btn, TOPLIGHT_OFF);
            }
        };

        setTopLightState(btn, TOPLIGHT_OFF);
    });


    // рабочая навеска
    const setWorkDeviceState = (btn, state, dir) => {
        if(websocket !== null)
            websocket.send(prepareData([ OBJ_NAME_WORK_DEVICE, (state) & 0xFF, (dir) & 0xFF]));
    };

    [...document.querySelectorAll('.work-device')].map(container => {
        let btn = container.querySelector('button');
        let state = CMD_STOP;
        let dir = null;
        if(container.classList.contains('work-device-up')){
            state = CMD_RUN;
            dir = WORK_DEVICE_UP;
        } else if(container.classList.contains('work-device-down')){
            state = CMD_RUN;
            dir = WORK_DEVICE_DOWN;
        }
        btn.ontouchstart = event => {
            if(state == CMD_RUN && dir != null){
                console.log('work-device dir: '+dir);
                setWorkDeviceState(btn, state, dir);
            }
        };
        btn.ontouchend = event => {
            console.log('work-device stop');
            setWorkDeviceState(btn, CMD_STOP, null);
        };

    })




    // WebSocket
    var wsOpened = false;

    var wsStart = () => {
        websocket = new WebSocket('ws://'+location.hostname+'/');
        websocket.binaryType = 'arraybuffer';

        websocket.onopen = function(evt) {
            // console.log('WebSocket connection opened');
            outDebug('WebSocket connection opened');
            wsOpened = true;
        }
    
        websocket.onmessage = function(event) {

            if(event.data instanceof ArrayBuffer){

            }

            console.log(event.data);

        }
    
        websocket.onclose = function(evt) {
            // console.log('Websocket connection closed');
            outDebug('WS connection closed');
            wsOpened = false;
            websocket = null;
            setTimeout(wsStart, 5000);
        }
    
        websocket.onerror = function(evt) {
            // console.log('Websocket error: ' + evt);
            outDebug('WS error: ' + evt);
            // wsOpened = false;
        }
          
    
    }

    wsStart();




})();

