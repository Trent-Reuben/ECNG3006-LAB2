**P1.**
For any project that you have already created, identify the following files and how changes in any one will POTENTIALLY affect the FreeRTOS configuration: 
- /project/sdkconfig 
- /project/build/include/sdkconfig.h 
- /sdk/components/freertos/port/esp8266/include/freertos/FreeRTOSConfig.h 

**A1.** 
Using the ques_2 project from this lab (lab 2), the use of vTaskGetRunTimeStats requires the modification of all three files: 
- two output files of the project, i.e., sdkconfig and sdkconfig.h  
- one being FreeRTOSConfig.h which is a "system file" for freeRTOS to function with the SDK. 

FreeRTOSConfig.h houses the configurations for access to freeRTOS functions. For example, the use of vTaskGetRunTimeStats utilizes the following macros (https://www.freertos.org/rtos-run-time-stats.html): 

1.configGENERATE_RUN_TIME_STATS 

2.portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()

3.portGET_RUN_TIME_COUNTER_VALUE()


With respect to the usage of the macros, the application of vTaskGetRunTimeStats for the esp8266 makes use of the configGENERATE_RUN_TIME_STATS. Enabling this macro allows for the enabling of a collection of runtime statistics. By default, this is set to 1 which allows access to the use of the runtime statistics. Setting this to 0 will disable the use of runtime statistics and hence disable the use of vTaskGetRunTimeStats for example. The other 2 macros are considered as further application of debugging using runtime statistics requires the implementations of either counters or timers given runtime statistic is based on time. With respect to the ESP, the internal clock is used. 

The sdkconfig file houses the configurations for the SDK, i.e., the configurations for the esp. By default, in the Espressif IoT Development Framework Configuration (make menuconfig), the configurations for the runtime statistics are disabled (Component config -> FreeRTOS). When this is disabled, when attempting to use uxTaskGetSystemState for example, the error "undefined reference to `uxTaskGetSystemState'" is shown when compiling and hence the compilation fails. By enabling "FreeRTOS to collect run time stats", the "FreeRTOS trace facility" and "FreeRTOS stats formatting functions" is enabled. This adjusts the sdkconfig file which in turns modifies sdkconfig.h allowing for the collection of runtime statistics thus allowing the use of relevant runtime stats functions offered through FreeRTOS.  

**P2.**  
Identify the labels that must be #define'd and the functions that provide task performance information in the ESP SDK. 
 
**A2.** 
The main function that provides task performance in the ESP SDK is

1. vTaskGetRunTimeStats (Uses the header file: task.h) (Source: 
https://www.freertos.org/a00021.html#vTaskGetRunTimeStats) 

	(void vTaskGetRunTimeStats( char *pcWriteBuffer );) 

Source (https://www.freertos.org/rtos-run-time-stats.html) 

This function collects the amount of processing time that has been used by each task. The information is presented in a tabular format where the absolute time and percentage time is presented. Absolute time (Abs Time) is the total time the task has been executing. Percentage time (%Time) is the percentage of the total processing time rather than as an absolute time. 

As discussed in P1, the following macros are required and defined in FreeTROSConfig.h:

1.configGENERATE_RUN_TIME_STATS 

2.portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() 

3.portGET_RUN_TIME_COUNTER_VALUE() 


The sdkconfig file must also be configured: 
#CONFIG_ENABLE_FREERTOS_SLEEP is not set 

#CONFIG_FREERTOS_USE_TRACE_FACILITY is not set 

#CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS is not set 

changed to: 

CONFIG_FREERTOS_USE_TRACE_FACILITY=y 

CONFIG_FREERTOS_USE_STATS_FORMATTING_FUNCTIONS=y

CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS=y 

CONFIG_FREERTOS_RUN_TIME_STATS_USING_ESP_TIMER=y 

This process is done using "make menuconfig". 

The steps for setting up vTaskGetRunTimeStats() are as follows: 
(Source: https://www.freertos.org/a00021.html#vTaskGetRunTimeStats) 

1. Configure the following in FreeRtosConfig.h 

	a. configGenerate_RUN_TIME_STATS is set to 1 

   	b. Ensure portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() function is included in the file 

   	c. Ensure portGET_RUN_TIME_COUNTER_VALUE() function is included in the file 

   	d. Add line: 
      	extern void vConfigureTimerForRunTimeStats(void); 


2. Initialize vTaskGetRunTimeStats in the program i.e.:

	void vTaskGetRunTimestats (char * pcWriteBuffer); 

3. Declare a static character array to hold the information for the vTaskGetRunTimestats 

	static char cbuffer[BUFFER SIZE] 

4. Call vTaskGetRunTimeStats 

	vTaskGetRunTimeStats(cbuffer); 

5. Print the output of the character array 

	printf(cbuffer) 

  

  

  

 

 