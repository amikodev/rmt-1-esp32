Автор: Приходько Дмитрий
[asketcnc@yandex.ru](mailto:asketcnc@yandex.ru)

# Робот мини трактор

## Настройка
* Скачать [VSCode](https://code.visualstudio.com/download)
* Установить [Platformio IDE for VSCode](https://platformio.org/install/ide?install=vscode)
* Изучить [основы работы Platformio](https://docs.platformio.org/en/latest/integration/ide/vscode.html#quick-start)

## Структура проекта
```
Projects
└---PlatformIO
    └---RMT1_ESP32
    └---components
```

## Установка
* В папке `RMT1_ESP32` выполнить `git clone https://github.com/amikodev/rmt-1-esp32.git .`
* В папке `components` выполнить `git clone https://github.com/amikodev/components-esp32.git .`

## Настройка переменных для компиляции
* В папке `RMT1_ESP32` выполнить `pio run -t menuconfig`
```
Component config --->
    Amikodev Wi-Fi --->
        далее задаёте настройки новой точки доступа и/или подключение к существующей сети
        Use Websocket - включить
        Include embed web files - включить
    RMT1 --->
        Wifi (управление по Wifi)
        Bluetooth (управление по Bluetooth)
        ShiftLoad (зарезервировано)
        R8EF (зарезервировано)
    FAT Filesystem support --->
        Long filename support --->
            Long filename buffer in heap
```

## Управление
* По Wifi - http://192.168.4.1 в браузере Google Chrome
* По Bluetooth - джойстик Magicsee R1

## Электрическая схема
![scheme-1](git.docs/scheme-1.jpg?raw=true "Схема 1")
![scheme-2](git.docs/scheme-2.jpg?raw=true "Схема 2")

## Компоненты блока управления
* Аварийный выключатель
* ESP32-DEVKIT-30
* LM7805-TO220
* Конденсаторы на 220 и 100 nF
* Релейный модуль на 8 реле

