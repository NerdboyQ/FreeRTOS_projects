#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "tusb.h"

#include "pico/cyw43_arch.h"

uint32_t led_delay = 500;
SemaphoreHandle_t led_delay_mutex;

/**
 * Task that Blinks an LED with a given delay 
 */
void led_task(void *pvParameters) {
    // initialize Pico W specific hardware interface
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed");
        return;
    }
    while (true){
        // try to attain mutex for led control
        if (xSemaphoreTake(led_delay_mutex, portMAX_DELAY) == pdTRUE) {
            printf("\t->%lu\n", led_delay);
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1); // turn on LED
            vTaskDelay(led_delay);
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0); // turn off LED
            vTaskDelay(led_delay);
            xSemaphoreGive(led_delay_mutex); // release mutex after flashing
        } else {
            printf("ERROR: LED_TASK could not get mutex\n");
            vTaskDelay(pdMS_TO_TICKS(100)); // delay task for otheer tasks
        }
    }
}

/**
 * Task that updates the LED blink delay
 */
void update_delay_task(void *pvParameters) {
    char input_buffer[10]; // buffer to store up to an 8 digit integer string
    int input_index = 0; // index that points at where to store the latest char
    int ch; // placeholder when reading characters from serial input
    uint32_t new_delay; // placeholder for the updated delay value

    printf("Enter LED delay time in milliseconds and press Enter:\n");

    while (true) {
        if (tud_cdc_connected()) { // Check if USB CDC is connected
            if (tud_cdc_available()) { // Check if data is available
                ch = tud_cdc_read_char(); // Read a character (non-blocking)
                printf("%c", ch);
                // check for newline or carriage return from serial input
                if (ch == '\r' || ch == '\n') {
                    // add termination char once return char received
                    input_buffer[input_index] = '\0';
                    // printf("new_val: %s\n", input_buffer);
                    printf("new_val (raw): '%s' (length: %d)\n", input_buffer, input_index); // Debug print
                    printf("Entry 1\n");
                    // read serial input from buffer as unsigned long
                    // then store the vaue at new_delay address
                    if (sscanf(input_buffer, "%lu", &new_delay) == 1) {
                        printf("Entry 2\n");
                        // if the entry was valid try to acquire the mutex lock
                        // protecting shared resource : led_delay
                        if (xSemaphoreTake(led_delay_mutex, portMAX_DELAY) == pdTRUE) {
                            printf("Entry 3\n");
                            led_delay = new_delay; // update new_delay w/ serial input value
                            xSemaphoreGive(led_delay_mutex); // release the mutex lock
                            printf("LED delay updated to %lums\n", led_delay);
                        } else {
                            printf("ERROR: UPDATE DELAY TASK could not take mutex\n");
                        }
                    } else {
                        printf("ERROR: Invalid input, please enter a number\n");
                    }
                    input_index = 0; // resetthe buffer index
                } else if (input_index < sizeof(input_buffer) - 1) {
                    // keep reading serial input increasing the index intil
                    // the buffer limit is reached or the return/newlone char
                    // is received 
                    input_buffer[input_index++] = ch;
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Small delay to yield CPU
    }
}

int main()
{
    stdio_init_all();
    printf("-- Starting new Task Demo --\n");
    
    // Create the mutex for led_delay
    led_delay_mutex = xSemaphoreCreateMutex();
    if (led_delay_mutex == NULL) {
        printf("Failed to create LED delay mutex\n");
        while (1);
    }
    // Create led task with lower proprity
    xTaskCreate(led_task, "LED_TASK", 256, NULL, 1, NULL);
    // Create update delay task with higher priority to preempt
    // the led_task when an update is received for a new delay value
    xTaskCreate(update_delay_task, "UPDATE_DELAY_TASK", 512, NULL, 2, NULL);
    // start the scheduler for the tasks
    vTaskStartScheduler();
    // endless loop to constantly run on microcontroller 
    while (true) {};
}
