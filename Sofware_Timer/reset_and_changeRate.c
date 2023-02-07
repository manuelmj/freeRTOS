/*
    Manuel Manjarres (manuelmj)

    en este ejemplo se muestra el uso de las funciones de reset cambio de periodo para los timers, 
    el ejemplo consiste en un timer que se encarga de apagar y encender un backlight de un teclado,
    el timer se encarga de apagar el backlight cada 500ms, si el timer llega a 3 veces se cambia el periodo a 1000ms
    esto para simular el uso de un teclado, si el timer llega a 6 veces se detiene el timer y se resetea a 0, lo que simula
    el uso del teclado.

*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define blacklightTimerPeriod pdMS_TO_TICKS(500)

volatile unsigned int xBlacklightFlag = pdFALSE;


//creamos los handles para los timers y las colas
TimerHandle_t xbacklightTimer;
QueueHandle_t xQueue;

//creamos las funciones de callback
void backlightTimerCallback(TimerHandle_t xTimer);

//creamos las tareas
void keypadBacklightOnTask(void *pvParameters);
void checkTask(void *pvParameters);

void app_main() {

    xbacklightTimer = xTimerCreate("backlightTimer",blacklightTimerPeriod, pdTRUE, (void *) 0, backlightTimerCallback);
    if(xbacklightTimer != NULL){
        xTimerStart(xbacklightTimer, 0);
    }
    xQueue = xQueueCreate(1, sizeof(unsigned int));

    xTaskCreate(keypadBacklightOnTask, "keypadBacklightOnTask", 2048, NULL, 5, NULL);
    xTaskCreate(checkTask, "checkTask", 2048, NULL, 4, NULL);
    return;
}

void backlightTimerCallback(TimerHandle_t xTimer){
    unsigned int timerCountId = (unsigned int) pvTimerGetTimerID(xTimer);
    timerCountId++;
    vTimerSetTimerID(xbacklightTimer, (void *) timerCountId);
    printf("Timer count id is %d \r\n", timerCountId);
    xBlacklightFlag = pdFALSE;

    if(timerCountId == 3 ){
        printf("Timer period is changed \r\n");
        //cambiamos el periodo del timer
        xTimerChangePeriod(xbacklightTimer, pdMS_TO_TICKS(1000), 0);
    }
         
    if(timerCountId >= 6){
        printf("Timer is stopped \r\n");
        xTimerStop(xbacklightTimer, 0); //detenemos el timer
        vTimerSetTimerID(xbacklightTimer, (void *) 0);
    }
    
    xQueueOverwrite(xQueue, &timerCountId);
}

void keypadBacklightOnTask(void *pvParameters){
    while(1){

        if(xBlacklightFlag == pdFALSE){
            printf(" Backlight is on \r\n"); 
            xBlacklightFlag = pdTRUE;
        }
        else{
            printf(" Backlight is off \r\n");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void checkTask(void *pvParameters){
    unsigned int recivedValue = 0;
    while(1){
        
        if(!xQueuePeek(xQueue, &recivedValue, portMAX_DELAY))continue;
        
        if(recivedValue >= 6){
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            xTimerReset(xbacklightTimer, 0); //reseta el timer
            printf("Timer is reset \r\n");
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);

    }
}
