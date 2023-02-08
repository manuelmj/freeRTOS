/*
Manuel manjarres (manuelmj)

Este código implementa una aplicación en FreeRTOS que crea tres tareas y un semáforo.
La tarea "vPeriodicTask" y "vPeriodicTask2" simulan interrupciones diferentes y llaman
a la función ISR_simulator. 
La función ISR_simulator "da" el semáforo tres veces para simular múltiples eventos de interrupción.
La tarea "vHandleISR" es responsable de manejar estos eventos de interrupción y tomamdo el semáforo
para procesa los eventos.
El semáforo se usa para proteger el acceso a los eventos y evitar la pérdida de datos
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"



SemaphoreHandle_t xSemaphore = NULL;

void ISR_simulator(void);

void vPeriodicTask(void *pvParameters);
void vPeriodicTask2(void *pvParameters);
void vHandleISR(void *pvParameters);

void app_main() {

    xSemaphore = xSemaphoreCreateCounting(10, 0); 


    xTaskCreate(vPeriodicTask, "Periodic Task", 2048, NULL, 5, NULL);
    xTaskCreate(vPeriodicTask2, "Periodic Task2", 2048, NULL, 5, NULL);
    xTaskCreate(vHandleISR, "Handle ISR", 2048, NULL, 5, NULL);
    
    
    return;
}




void ISR_simulator(void){

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if(!xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken))printf("Error: xSemaphoreGiveFromISR\n");
    if(!xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken))printf("Error: xSemaphoreGiveFromISR\n");
    if(!xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken))printf("Error: xSemaphoreGiveFromISR\n");

    if(xHigherPriorityTaskWoken == pdTRUE){
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}



/*
'Dar' el semáforo varias veces. La primera desbloqueará la tarea de gestión de interrupciones diferidas.
diferida, los siguientes 'gives' son para demostrar que el semáforo bloquea los eventos para permitir que
la tarea a la que se aplazan las interrupciones procesarlas a su vez, sin que los eventos se pierdan. 
Esto simula múltiples interrupciones, aunque en este caso los eventos se simulan dentro de dos interrupciónes
con periodos diferentes.
*/

void vPeriodicTask(void *pvParameters){

    while(1){
        vTaskDelay(500 / portTICK_PERIOD_MS);
        printf("Peridic Task\n");
        ISR_simulator();
        printf("ISR simulated\n");
    }
}

void vPeriodicTask2(void *pvParameters){

    while(1){
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("Peridic Task2\n");
        ISR_simulator();
        printf("ISR simulated-2\n");
    }
}

void vHandleISR(void *pvParameters){

    while(1){
        if(!xSemaphoreTake(xSemaphore, portMAX_DELAY)){
            printf("Error: xSemaphoreTake\n");
            continue;
        }
        printf("ISR handled\n");
    }
}


