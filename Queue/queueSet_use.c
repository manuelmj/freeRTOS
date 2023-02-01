/*
`   Manuel Manjarres Rivera (manuelmj)

 This example shows how to use a queue set with different types of elements (pointers, integers and binary semaphores)
 which is more common when the application requires different queues for the types of data that are handled
 and it is required that a single task handles these different types of data without having to create a task for each queue
    
Este ejemplo muestra como usar un queue set con diferentes tipos de elementos (punteros, enteros y semaforos binarios)
lo que es mas comun cuando la aplicacion requiere diferentes colas para los tipos de datos que se manejan
y se requiere que una misma tarea maneje estos diferentes tipos de datos sin tener que crear una tarea por cada cola
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"


//creamos los diferentes handles de cada una de las queue

QueueHandle_t xQueuePointer; // handle de la queue de punteros
QueueHandle_t xQueueInt; // handle de la queue de enteros
SemaphoreHandle_t xSemaphore; // handle del semaforo binario
QueueHandle_t xQueueSet; // handle de la queue set


//definimos las tareas que simularan el envio y recepcion de los datos a las diferentes queues

void vSendPointerTaks(void *pvParameters);
void vSendIntTaks(void *pvParameters);
void vSendSemaphoreTaks(void *pvParameters);
void vTaskRecive(void *pvParameters);
void vTasksaludo(void *pvParameters);

void app_main() {

    //creamos las diferentes queues
    xQueuePointer = xQueueCreate(5, sizeof(char *)); //creamos la queue de punteros
    xQueueInt = xQueueCreate(5, sizeof(int)); //creamos la queue de enteros
    xSemaphore = xSemaphoreCreateBinary(); //creamos el semaforo binario
    xQueueSet = xQueueCreateSet(5 * 3); //creamos la queue set
    
    //añadimos las diferentes queues a la queue set
    xQueueAddToSet(xQueuePointer, xQueueSet); //añadimos la queue de punteros a la queue set
    xQueueAddToSet(xQueueInt, xQueueSet); //añadimos la queue de enteros a la queue set
    xQueueAddToSet(xSemaphore, xQueueSet); //añadimos el semaforo binario a la queue set

    //creamos las diferentes tareas
    xTaskCreate(vTaskRecive, "ReciveTake", 2048, NULL, 2, NULL); //creamos la tarea que simulara la recepcion de los datos
    xTaskCreate(vSendPointerTaks, "SendPointerTake", 2048, NULL, 3, NULL); //creamos la tarea que simulara el envio de punteros
    xTaskCreate(vSendIntTaks, "SendIntTake", 2048, NULL, 3, NULL); //creamos la tarea que simulara el envio de enteros
    xTaskCreate(vSendSemaphoreTaks, "SendSemaphoreTake", 2048, NULL, 3, NULL); //creamos la tarea que simulara el envio de semaforos


    return;
}


//creamos las tareas y codificamos su funcionamiento
void vSendPointerTaks(void *pvParameters) {
    char *data = NULL;
    unsigned const char zise = 20;
    while (1) {
        //reservamos memoria para el puntero
        data = (char *) malloc(zise);
        if(data == NULL) {
            printf("Error al reservar memoria\r\n");
            vTaskDelay(200 / portTICK_PERIOD_MS);
            continue;
        }
        //copiamos lo que enviaremos en el buffer reservado
        sprintf(data, "Hola mundo\r\n");

        if(!xQueueSend(xQueuePointer, &data, portMAX_DELAY)){ //enviamos el puntero a la queue de punteros
            printf("Error al enviar el puntero\r\n");
            continue;
        }
        vTaskDelay(800 / portTICK_PERIOD_MS);
    }
}


void vSendIntTaks(void *pvParameters) {
    int data = 0;
    while (1) {
        data++;
        if(!xQueueSend(xQueueInt, &data, portMAX_DELAY)){ //enviamos el entero a la queue de enteros
            printf("Error al enviar el entero\r\n");
            continue;
        }
        if(data == 10) data = 0;

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}


void vSendSemaphoreTaks(void *pvParameters) {
    while (1) {
        if(!xSemaphoreGive(xSemaphore)){ //enviamos el semaforo binario
            printf("Error al enviar el semaforo\r\n");
            continue;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}


void vTaskRecive(void *pvParameters){
    QueueSetMemberHandle_t xActivatedMember;
    char *data = NULL;
    int dataInt = 0;
    while (1) {
        xActivatedMember = xQueueSelectFromSet(xQueueSet, portMAX_DELAY); //esperamos a que se active algun elemento de la queue set

        if(xActivatedMember == NULL){
            printf("Error al recibir el elemento\r\n");
            continue;
        }
        else if(xActivatedMember == (QueueSetMemberHandle_t) xQueuePointer){ //si el elemento activado es la queue de punteros
            if(!xQueueReceive(xQueuePointer, &data, portMAX_DELAY)){ //recibimos el puntero
                printf("Error al recibir el puntero\r\n");
                continue;
            }
            printf("Recibido puntero: %s\r\n", data);
            free(data); //liberamos la memoria reservada para el puntero
        
        }
        else if(xActivatedMember == (QueueSetMemberHandle_t) xQueueInt){ //si el elemento activado es la queue de enteros
            if(!xQueueReceive(xQueueInt, &dataInt, portMAX_DELAY)){ //recibimos el entero
                printf("Error al recibir el entero\r\n");
                continue;
            }
            printf("Recibido entero: %d\r\n", dataInt);
        }
        else if(xActivatedMember == (QueueSetMemberHandle_t) xSemaphore){ //si el elemento activado es el semaforo binario
            if(!xSemaphoreTake(xSemaphore, 0)){ //recibimos el semaforo con tiempo 0 debido a que sabemos que esta disponible 
                printf("Error al recibir el semaforo\r\n");
                continue;
            }
            printf("Recibido semaforo\r\n");
            // uso opcional cuando requerimos hacer algo con el semaforo recibido y es de alta prioridad    
            xTaskCreate(vTasksaludo, "Saludo", 2048, NULL, 2, NULL); //creamos la tarea temporal
        }
    }
}

void vTasksaludo(void *pvParameters){
    while (1) {
        printf("<<<<<<Hola mundo desde una tarea temporal>>>>>>>\r\n");
        vTaskDelete(NULL);
    }
}
