/*
Manuel Manjarres rivera (manuelmj)

este codigo muestra un ejemplo de tres de las formas en las que puede ser usada una queue
la primera es copiando los datos enviados, la segunda es enviando los datos por medio de apuntadores
y la tercera es usando un set de queue que es comun cuando tenemos aplicaciones que requieren queues diferentes
para datos que seran consumidos por una misma tarea


*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// definimos el tipo de dato que se enviara en la cola
typedef enum
{
    activity1,
    activity2,
    activity3
} DataSource_t;

// estructura que define el tipo de dato enviado en la queue (cola)
typedef struct
{
    DataSource_t source; // origen de los datos
    uint32_t data;       // datos
} Data_t;

// creamos el queue handle
QueueHandle_t xQueue; // handle de la cola de datos struct Data_t

QueueHandle_t xQueue_string; // handle de la queue tipo puntero a char (string)

// creamos dos handles que haran parte de un queue set
QueueHandle_t xQueueSet1;
QueueHandle_t xQueueSet2;
QueueHandle_t xQueueSet; // creamos el handre del queue set

// tareas para simular el envio y recepcion de datos
void vReciveTask(void *pvParameters);
void vSendTask_1(void *pvParameters);
void vSendTask_2(void *pvParameters);

// tareas para simular el envio y recepcion de los datos de (de longitud variable)
void vSendStringTask(void *pvParameters);
void vReciveStringTask(void *pvParameters);

// tareas para simular el envio y recepcion de datos de el queue set (datos de longitud variable)
void vReciveSetTask(void *pvParameters);
void vSendSetTask_1(void *pvParameters);
void vSendSetTask_2(void *pvParameters);

void app_main()
{

    xQueue = xQueueCreate(8, sizeof(Data_t));

    xQueue_string = xQueueCreate(8, sizeof(char *));

    xQueueSet1 = xQueueCreate(2, sizeof(char *));
    xQueueSet2 = xQueueCreate(2, sizeof(char *));

    xQueueSet = xQueueCreateSet(2 * 2); // creamos el queue set con el tamaño de los dos handles es decir 2 queue de 2 elementos cada uno

    xQueueAddToSet(xQueueSet1, xQueueSet); // agregamos la queue1 al queue set
    xQueueAddToSet(xQueueSet2, xQueueSet); // agregamos la queue2 al queue set

    xTaskCreate(vReciveTask, "vReciveTask", 2048, NULL, 5, NULL);
    xTaskCreate(vSendTask_1, "vSendTask_1", 2048, NULL, 5, NULL);
    xTaskCreate(vSendTask_2, "vSendTask_2", 2048, NULL, 5, NULL);

    xTaskCreate(vSendStringTask, "vSendStringTask", 2048, NULL, 5, NULL);
    xTaskCreate(vReciveStringTask, "vReciveStringTask", 2048, NULL, 4, NULL);

    xTaskCreate(vReciveSetTask, "vReciveSetTask", 2048, NULL, 6, NULL);
    xTaskCreate(vSendSetTask_1, "vSendSetTask_1", 2048, NULL, 6, NULL);
    xTaskCreate(vSendSetTask_2, "vSendSetTask_2", 2048, NULL, 6, NULL);
}

// tareas para simular el envio de los datos, en este caso se envian datos de tipo struct Data_t que son copiados en la cola
void vSendTask_1(void *pvParameters)
{

    Data_t data;
    data.source = activity1;
    data.data = 10;
    while (1)
    {
        if (!xQueueSendToBack(xQueue, &data, 1000 / portTICK_PERIOD_MS))
        {
            printf("error al enviar los datos de la actividad 1 \r \n");
            continue;
        }
    }
}

void vSendTask_2(void *pvParameters)
{

    Data_t data;
    data.source = activity2;
    data.data = 20;
    while (1)
    {

        if (uxQueueMessagesWaiting(xQueue) > 4)
        {
            data.data = 30;
            data.source = activity3;
        }

        if (!xQueueSendToBack(xQueue, &data, 1000 / portTICK_PERIOD_MS))
        {
            printf("error al enviar los datos de la actividad 2 \r \n");
            continue;
        }
    }
}

void vReciveTask(void *pvParameters)
{

    Data_t data; // estructura de los datos recibidos

    while (1)
    {

        if (!xQueueReceive(xQueue, &data, 1000 / portTICK_PERIOD_MS))
        {
            printf("Error al recibir los datos \r\n");
        }

        if (data.source == activity1)
        {
            printf(" dato %d recibidos de la actividad 1 \r \n", data.data);
        }
        else if (data.source == activity2)
        {
            printf(" dato %d recibidos de la actividad 2 \r \n", data.data);
        }
        else if (data.source == activity3)
        {
            printf(" dato %d recibidos de la actividad 3 \r \n", data.data);
        }
    }
}

// tareas para simular el envio de los datos string, se envia un puntero
void vSendStringTask(void *pvParameters)
{

    char *string_data;
    const int size = 20;
    while (1)
    {

        string_data = (char *)malloc(size);
        if (string_data == NULL)
        {
            printf("error al asignar memoria \r \n");
            continue;
        }

        sprintf(string_data, "hola mundo %d", 1);

        if (!xQueueSendToBack(xQueue_string, &string_data, 100 / portTICK_PERIOD_MS))
        {
            printf("error al enviar los datos de la actividad 1 \r \n");
            continue;
        }
    }
}

void vReciveStringTask(void *pvParameters)
{

    char *string_data;

    while (1)
    {

        if (!xQueueReceive(xQueue_string, &string_data, 100 / portTICK_PERIOD_MS))
        {
            printf("Error al recibir los datos \r\n");
            continue;
        }

        printf(" %s \r \n", string_data);
        free(string_data);
    }
}

// tareas para simular el envio y recepcion de datos de el queue set (datos de longitud variable)

void vSendSetTask_1(void *pvParameters)
{

    char *string_data;
    const int size = 30;
    while (1)
    {

        string_data = (char *)malloc(size);
        if (string_data == NULL)
        {
            printf("error al asignar memoria \r \n");
            continue;
        }

        sprintf(string_data, "envio desde el set %d\r\n", 1);

        if (!xQueueSendToBack(xQueueSet1, &string_data, 100 / portTICK_PERIOD_MS))
        {
            printf("error al enviar los datos de la actividad 1 \r \n");
            continue;
        }
    }
}

void vSendSetTask_2(void *pvParameters)
{

    char *string_data_set;
    const int size = 30;
    while (1)
    {

        string_data_set = (char *)malloc(size);
        if (string_data_set == NULL)
        {
            printf("error al asignar memoria \r \n");
            continue;
        }

        sprintf(string_data_set, "envio desde el set %d\r\n", 2);

        if (!xQueueSendToBack(xQueueSet2, &string_data_set, 100 / portTICK_PERIOD_MS))
        {
            printf("error al enviar los datos de la actividad 1 \r \n");
            continue;
        }
    }
}

void vReciveSetTask(void *pvParameters)
{

    char *string_data;
    QueueHandle_t xQueueSetHandle;

    while (1)
    {

        xQueueSetHandle = xQueueSelectFromSet(xQueueSet, portMAX_DELAY);

        if (xQueueSetHandle == NULL)
        {
            printf("Error al recibir los datos \r\n");
            continue;
        }

        if (!xQueueReceive(xQueueSetHandle, &string_data, 100 / portTICK_PERIOD_MS))
        {
            printf("Error al recibir los datos \r\n");
            continue;
        }

        printf(" %s \r \n", string_data);

        // liberamos la memoria usada para almacenar el dato
        free(string_data);
    }
}
