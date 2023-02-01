/*
Manuel Manjarres Rivera

en este ejemplo se usa una queue para mostrar el comportamiento de una mailbox que puede ser leido por
cualquier tarea, pero solo puede ser escrito por una sola tarea.
los datos no pasan por el buzon, sino que permanecen en el mismo hasta que son sobreescribidos por nuevos datos.
quien sobreescribe este valor es unicamente el emisor, por otro lado el receptor lee le valor de buzon pero no lo elimina.

*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// creamos el tipo de dato que se enviará por la cola
typedef struct
{
    unsigned char id;
    void *data;
} xData;

// creamos el handle de la cola
static QueueHandle_t mailbox;

// creamos las tareas que simulan el envio y recepción de datos
void vSendTask(void *pvParameters);
void vReceiveTask_1(void *pvParameters);
void vReceiveTask_2(void *pvParameters);

void app_main()
{

    // creamos la cola
    mailbox = xQueueCreate(1, sizeof(xData));
    xTaskCreatePinnedToCore(vSendTask, "SendTask", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(vReceiveTask_1, "ReceiveTask_1", 2048, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(vReceiveTask_2, "ReceiveTask_2", 2048, NULL, 1, NULL, 1);
    return;
}

void vSendTask(void *pvParameters)
{
    xData data;

    while (1)
    {
        for (int i = 0; i < 10; i++)
        {
            data.data = (int *)malloc(sizeof(int));
            if (data.data == NULL)
            {
                printf("Error al reservar memoria");
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                continue;
            }
            data.id = i;
            *((int *)data.data) = i + 300;
            xQueueOverwrite(mailbox, &data);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            free(data.data);
        }
    }
}

void vReceiveTask_1(void *pvParameters)
{
    xData data;
    while (1)
    {
        if (!xQueuePeek(mailbox, &data, portMAX_DELAY))
        {
            printf("Error al leer la cola");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        printf("Task 1: id: %d, data: %d\r\n", data.id, *((int *)data.data));
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void vReceiveTask_2(void *pvParameters)
{
    xData data;
    while (1)
    {
        if (!xQueuePeek(mailbox, &data, portMAX_DELAY))
        {
            printf("Error al leer la cola");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        printf("Task 2: id: %d, data: %d\r\n", data.id, *((int *)data.data));
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
