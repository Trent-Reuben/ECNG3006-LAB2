/*Lab2
Name: TrentReuben
I.D.:  816015091

Question 3:
Use IDLE "hook" function to put the processor to sleep for some time when the system is not going to be "in-use".

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "freertos/semphr.h"

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_system.h"
#include "sys/time.h"
#include "esp_sleep.h"

static const char *TAG = "main";

#define GPIO_OUTPUT_IO  2

//This program make use of 3 tasks with a defined priority hence the following are defined
#define PRIORITY_HIGH   3
#define PRIORITY_MID    2
#define PRIORITY_LOW    1

//Runtime statistics for identification of performance metrics
void vTaskGetRunTimeStats(char *pcWriteBuffer);

SemaphoreHandle_t xMutex = NULL;

//this task display the status of the gpio pin
static void status_msg(void* arg)
{
    for(;;)
    {
        //display message for status
        ESP_LOGI(TAG,"GPIO_2 Pin STATUS: %d\n", gpio_get_level(GPIO_OUTPUT_IO));
        //1s delay
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

static void active_wait()
{
    //0.5s active delay
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);    
    int64_t time_start = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec; //time since last reset in us

    while(true)
    {
        gettimeofday(&tv_now, NULL);    
        int64_t time_now = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;

        //if 0.5s has elapsed
        if((time_now - time_start) >= 500000)
        {
            break;
        }
    }

    printf("0.5s active wait complete. \n" );
}

//This task set the gpio high, active delays for 0.5s and then task delays for 1s
static void gpio_on(void* arg)
{
    for (;;)
    {
        //Try to take semaphore.
        while(true)
        {
            if(xSemaphoreTake(xMutex, (TickType_t) 10) == pdTRUE)
            {
                //if mutex is obtained display the foll. message
                ESP_LOGI("Task: gpio_on", "Setting GPIO2 high\n");
                gpio_set_level(GPIO_OUTPUT_IO,1);
                break;
            }
            else
            {
                continue;
            }
        }

        //0.5s active delay
        active_wait();

        //the mutex must be given back
        xSemaphoreGive(xMutex);
        //1s delay
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

//This task set the gpio low, active delays for 0.5s and then task delays for 1s
static void gpio_off(void* arg){
    for (;;){
        //Try to take semaphore.
        while(true){
            if(xSemaphoreTake(xMutex, (TickType_t) 10) == pdTRUE)
            {
                //if mutex is obtained display the foll. message
                ESP_LOGI("Task: gpio_off", "Setting GPIO2 low\n");
                gpio_set_level(GPIO_OUTPUT_IO,0);
                //break for loop once semaphore is obtained
                break;
            }
            else
            {
                continue;
            }
        }

        //0.5s active delay
        active_wait();

        //the mutex must be given back
        xSemaphoreGive(xMutex);

        //1s delay
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

// IDLE "hook" function to put processor to sleep
void vApplicationIdleHook(void)
{
    //sleep for 0.5s 
    ESP_LOGI(TAG,"Going to sleep for 0.5s. \n");
    // configured for 0.5s 
    esp_sleep_enable_timer_wakeup(500000);
    //enters light sleep with the configured wakeup options
    esp_light_sleep_start();
}

void app_main(void)
{
    // buffer for vTaskGetRunTimeStats of size 2000 however (rule of thumb) 40 bytes is required per task
    static char cbuffer[2000];
	
	//Create mutex type semaphore
    xMutex = xSemaphoreCreateMutex();
	
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO15/16
    io_conf.pin_bit_mask = (1ULL<<GPIO_OUTPUT_IO);
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //Check the semaphore was created successfully before creating the tasks
	if( xMutex != NULL)
    {
		//Create instance of the task to turn set the gpio mid with priority 2
		xTaskCreate(gpio_on, "gpio_on", 2048, NULL, PRIORITY_MID, NULL);

		//Create instance of the task to turn set the gpio low with priority 1
		xTaskCreate(gpio_off, "gpio_off", 2048, NULL, PRIORITY_LOW, NULL);
	} 
	//Create instance of the task to display the status of the gpio2 pin with highest priority
    xTaskCreate(status_msg, "status_msg", 2048, NULL, PRIORITY_HIGH, NULL);

    printf("Task            Abs Time       %%Time \n");
    printf("**************************************\n");

    //Run runtime statistics function
    vTaskGetRunTimeStats(cbuffer);
    //print output of runtime statistics
    printf(cbuffer, "\n");	
  
    //for heap memory management
	for(;;)
    {
        vApplicationIdleHook();
    };
}


