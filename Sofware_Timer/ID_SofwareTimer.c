/*
    Manuel Manjarres Rivera

    en este ejemplo se muestra uno de los usos del id de los timers, el cual es util para la misma funcion que se ejecuta
    cuando el timer se dispara, en este caso se utiliza para contar el numero de veces que se ha ejecutado el timer.
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

// Definimos el tiempo de ejecucion de los timers
#define oneShot_timer_period_ms pdMS_TO_TICKS(1000)
#define autoReload_timer_period_ms pdMS_TO_TICKS(500)
// creamos los handles de los timers
TimerHandle_t xOneShotTimer = NULL;
TimerHandle_t xAutoReloadTimer = NULL;

// funcion que se ejecuta cuando el timer se dispara (callback function)
void vTimerCallback(TimerHandle_t xTimer);

void app_main()
{
    xOneShotTimer = xTimerCreate("Timer one shot", oneShot_timer_period_ms, pdFALSE, (void *)0, vTimerCallback);
    xAutoReloadTimer = xTimerCreate("Timer auto reload", autoReload_timer_period_ms, pdTRUE, (void *)0, vTimerCallback);

    if (xOneShotTimer != NULL && xAutoReloadTimer != NULL)
    {
        xTimerStart(xOneShotTimer, 0);
        xTimerStart(xAutoReloadTimer, 0);
    }

    // creamos un bucle infinito con un tiempo muerto para evitar un reinicio del microcontrolador
    while (1)
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    return;
}

void vTimerCallback(TimerHandle_t xTimer)
{

    /*
        el valor de veces que se ha ejecutado el timmer es almacenado en el id del timer
        de esta forma luego de que se haya ejecutado 5 veces el timer se detiene
        Nota: el id del timer se asigna por primera vez cuando el timer es creado y se puede
             cambiar en cualquier momento
    */
    unsigned int ulCount = (unsigned int)pvTimerGetTimerID(xTimer);
    // printf("Timer callback %d\n", ulCount);
    ulCount++;
    vTimerSetTimerID(xTimer, (void *)ulCount);

    if (xTimer == xOneShotTimer)
    {
        printf("One shot timer callback\n");
    }
    else if (xTimer == xAutoReloadTimer)
    {
        printf("Auto reload timer callback\n");
        if (ulCount == 5)
        {
            printf("Stop auto reload timer\n");
            xTimerStop(xAutoReloadTimer, 0);
            xTimerReset(xOneShotTimer, 0);
        }
    }
}
