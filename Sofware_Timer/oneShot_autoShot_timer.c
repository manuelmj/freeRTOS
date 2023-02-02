/*
Manuel Manjarres Rivera

en este ejemplo vemos crear como crear un timer one-shot y un auto-reload timer, y como activarlos
a demas usamos una tarea que se encargará de activar el timer one-shot cada 3 segundos
para ejemplificar como este tipo de timers se comportan
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

// definimos los periodos de tiempo de los timer
#define Auto_Reload_Timer_PERIOD pdMS_TO_TICKS(500)
#define One_Shot_Timer_PERIOD pdMS_TO_TICKS(1000)

TimerHandle_t OneShotTimer;    // creamos el one-shot timer
TimerHandle_t AutoReloadTimer; // creamos el auto-reload timer

void vAutoReloadTimerCallback(TimerHandle_t xTimer);                                      // creamos la función de callback del auto-reload timer
void vOneShotTimerCallback(TimerHandle_t xTimer);                                         // creamos la función de callback del one-shot timer
void init_timer(const TimerHandle_t *OneShotTimer, const TimerHandle_t *AutoReloadTimer); // creamos la función que inicializará los timer

// creamos una tarea que se encargará de activar el timer one-shor cada 3 segundos
void vCallToTimer(void *pvParameters);

void app_main()
{

    // creamos el one-shot timer
    OneShotTimer = xTimerCreate("OneShotTimer", One_Shot_Timer_PERIOD, pdFALSE, (void *)0, vOneShotTimerCallback); // creamos el one-shot timer
    // creamos el auto-reload timer
    AutoReloadTimer = xTimerCreate("AutoReloadTimer", Auto_Reload_Timer_PERIOD, pdTRUE, (void *)0, vAutoReloadTimerCallback); // creamos el auto-reload timer

    // iniciamos los timer
    init_timer(&OneShotTimer, &AutoReloadTimer); // iniciamos los timer

    xTaskCreatePinnedToCore(vCallToTimer, "CallToTimer", 2048, NULL, 1, NULL, 0); // creamos la tarea que se encargará de activar el timer one-shot cada 3 segundos

    return;
}

void init_timer(const TimerHandle_t *OneShotTimer, const TimerHandle_t *AutoReloadTimer)
{

    if (*OneShotTimer != NULL && *AutoReloadTimer != NULL)
    {
        BaseType_t xTimer1Start = xTimerStart(*AutoReloadTimer, 0); // iniciamos el auto-reload timer
        BaseType_t xTimer2Start = xTimerStart(*OneShotTimer, 0);    // iniciamos el one-shot timer

        if (xTimer1Start == pdPASS && xTimer2Start == pdPASS)
        {
            printf("Timer iniciados correctamente");
            return;
        }
        printf("Error al iniciar los timer");
    }
}

void vAutoReloadTimerCallback(TimerHandle_t xTimer)
{
    TickType_t xTimeNow = xTaskGetTickCount();      // obtenemos el tiempo actual
    printf("auto reload Time now: %d\n", xTimeNow); // imprimimos el tiempo actual
    // ulCallCount++; //incrementamos el contador de llamadas
}

void vOneShotTimerCallback(TimerHandle_t xTimer)
{
    TickType_t xTimeNow = xTaskGetTickCount();                        // obtenemos el tiempo actual
    printf("\n<<<<<<<One shot Time now: %d>>>>>>>>\n", xTimeNow); // imprimimos el tiempo actual
}

void vCallToTimer(void *pvParameters)
{
    while (1)
    {
        vTaskDelay(3000 / portTICK_PERIOD_MS); // esperamos 3 segundos
        // printf("Activamos el timer one-shot:");
        xTimerReset(OneShotTimer, 0); // reiniciamos el timer one-shot
    }
}