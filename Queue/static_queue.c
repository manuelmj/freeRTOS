/*

En este ejemplo se muestra el uso de una cola que tiene diferentes escritores y un solo lector. esta cola 
tambien envia siempre el mismo tipo de dato, en este caso un entero, pero se puede usar para enviar cualquier tipo de dato

Por otro lado la tarea de lectura de la cola tiene mayor prioridad que las tareas de escritura, por lo cual la cola nunca se llenara
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define __TIME__ 2000

//creamos la enumeracion de los diferentes ids de las tareas que escribiran en la cola
typedef enum {
    send_task1,
    send_task2
} type_t;

//creamos la structura de datos que usaremos en la queue
typedef struct {
    type_t type;
    int data;
} data_t;


//creamos el handle de la queue
QueueHandle_t xQueue;


//creamos las tareas que simularan el envio y recepcion de los datos a las diferentes queues
void vSendTask1(void *pvParameters);
void vSendTask2(void *pvParameters);
void vTaskRecive(void *pvParameters);

void app_main() { 
    //creamos la queue
    xQueue = xQueueCreate(10, sizeof(data_t));

    //creamos las diferentes tareas
    xTaskCreate(vTaskRecive, "ReciveTake", 2048, NULL,2, NULL); //creamos la tarea que simulara la recepcion de los datos
    xTaskCreate(vSendTask1, "SendTask1", 2048, NULL, 3, NULL); //creamos la tarea que simulara el envio de datos
    xTaskCreate(vSendTask2, "SendTask2", 2048, NULL, 3, NULL); //creamos la tarea que simulara el envio de datos

    return;
}




void vSendTask1(void *pvParameters) {
    data_t data;
    data.type = send_task1;
    data.data = 0;
    while (1) {

        if((data.data++) == 10) data.data = 0;
        
        if(!xQueueSend(xQueue, &data, 100 / portTICK_PERIOD_MS)){
            printf("Error al enviar datos\r\n");
            continue;
        }
        
        vTaskDelay(__TIME__ / portTICK_PERIOD_MS);
    }
}


void vSendTask2(void *pvParameters) {
    data_t data;
    data.type = send_task2;
    data.data = 9;
    while (1) {

        if((data.data--) == 0) data.data = 9;
        
        if(!xQueueSend(xQueue, &data, 100 / portTICK_PERIOD_MS)){
            printf("Error al enviar datos\r\n");
            continue;
        }
        
        vTaskDelay((__TIME__/2) / portTICK_PERIOD_MS);
    }
}



void vTaskRecive(void *pvParameters) {
    data_t data;
    while (1) {

        if(!xQueueReceive(xQueue, &data, portMAX_DELAY)){
            printf("Error al recibir datos\r\n");
            continue;
        }
        
        if(data.type == send_task1) 
            printf("Recibido de la tarea 1: %d\r\n", data.data);
        else if(data.type == send_task2) 
            printf("Recibido de la tarea 2: %d\r\n", data.data);
        else 
            printf("Error al recibir datos\r\n");

    }
}
