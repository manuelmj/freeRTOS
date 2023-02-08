/*
Manuel manjarres (manuelmj)

Este código muestra la creación de varias tareas en FreeRTOS. También se usa un semáforo binario 
(xBinarySemaphore) para sincronizar la interrupción simulada (ISR_Simulate) y la tarea que la maneja
(handleISR).

La tarea "vPeriodicTask" se encarga de generar la interrupción simulada cada 200ms, y la función 
"ISR_Simulate" se encarga de dar el semáforo binario correspondiente. La tarea "handleISR" se encarga 
de tomar el semáforo y hacer alguna acción.

Además, se tienen las tareas "vTask1" y "vTask2" que usan otro semáforo binario (xBinarySemaphore2) para
sincronizarse entre ellas. La tarea "vTask1" da el semáforo y la tarea "vTask2" lo toma.

La función "portYIELD_FROM_ISR()" se utiliza en la función "ISR_Simulate" para ceder el control a una tarea
de mayor prioridad en caso de que se haya dado el semáforo y haya habido un cambio en la prioridad de tareas. 
De esta manera, se asegura que la tarea crítica sea ejecutada de manera oportuna y que la ISR no bloquee la 
ejecución del sistema.
*/


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"


//creamos el handle del semaforo binario
SemaphoreHandle_t xBinarySemaphore;
SemaphoreHandle_t xBinarySemaphore2;


void ISR_Simulate(void *interrupNumber);
//creamos la tarea periodica
void vPeriodicTask(void *pvParameters);
void handleISR(void *pvParameters);


void vTask1(void *pvParameters);
void vTask2(void *pvParameters);

void app_main() {
    xBinarySemaphore = xSemaphoreCreateBinary();
    xBinarySemaphore2 = xSemaphoreCreateBinary();

    xTaskCreate(vPeriodicTask, "vPeriodicTask", 2048, NULL, 5, NULL);
    xTaskCreate(handleISR, "handlerISR", 2048, NULL, 4, NULL);
    xTaskCreate(vTask1, "vTask1", 2048, NULL, 3, NULL);
    xTaskCreate(vTask2, "vTask2", 2048, NULL, 6, NULL);
    return;
}


void vPeriodicTask(void *pvParameters){
    while(1){
        vTaskDelay(pdMS_TO_TICKS(200));
        printf("genetaring interrupt \r\n");
        ISR_Simulate((void *) 1);
        printf("<<<<>>>> \r\n");
    }
}


void ISR_Simulate(void *interrupNumber){
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);
    if(xHigherPriorityTaskWoken == pdTRUE){
        portYIELD_FROM_ISR();
    }
}


void handleISR(void *pvParameters){
    while(1){
        if(xSemaphoreTake(xBinarySemaphore, portMAX_DELAY) == pdTRUE){
            printf("ISR is handled \r\n");
        }
    }
}


void vTask1(void *pvParameters){
    while(1){
        if(!xSemaphoreGive(xBinarySemaphore2)){
            printf(" Eror al dar el semaforo \r\n");
        }

        printf("Tarea 1 \r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vTask2(void *pvParameters){
    while(1){
        if(!xSemaphoreTake(xBinarySemaphore2, portMAX_DELAY)){
            printf(" Eror al tomar el semaforo \r\n");
        }
            
        printf(" Tarea 2 \r\n");

    }
}











