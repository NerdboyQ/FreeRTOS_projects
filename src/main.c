#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"

#include "pico/cyw43_arch.h"

void led_task(){
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed");
        return;
    }
    while (true){
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        vTaskDelay(100);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        vTaskDelay(100);
    }
}

int main()
{
    stdio_init_all();
    printf("-- Starting new Task Demo --\n");

    xTaskCreate(led_task, "LED_TASK", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    while (true) {};
}
