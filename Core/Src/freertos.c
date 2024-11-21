/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lwip.h"
#include "usart.h"
#include <stdio.h>
#include "lwip/api.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "string.h"
extern struct netif gnetif;
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SERVER_IP   "192.168.0.27"  // Replace with your server's IP address
#define SERVER_PORT 6000             // Replace with your server's port
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for myTask02 */
osThreadId_t myTask02Handle;
const osThreadAttr_t myTask02_attributes = {
  .name = "myTask02",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for myTimer01 */
osTimerId_t myTimer01Handle;
const osTimerAttr_t myTimer01_attributes = {
  .name = "myTimer01"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void tcp_client_task(void* argument);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTask02(void *argument);
void Callback01(void *argument);

extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of myTimer01 */
  myTimer01Handle = osTimerNew(Callback01, osTimerPeriodic, NULL, &myTimer01_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  if (osTimerStart(myTimer01Handle, 1000) != osOK) {
          // Handle error in starting the timer
          printf("Failed to start timer\n");
          return;
      }
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of myTask02 */
  myTask02Handle = osThreadNew(StartTask02, NULL, &myTask02_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN StartDefaultTask */
  osDelay(5000);

  printf("before\n");
    //tcp_client_task((void*)argument);

  /* Infinite loop */
  for(;;)
  {
	//ethernetif_input(&gnetif);
	//sys_check_timeouts();
	  HAL_GPIO_TogglePin(GPIOB, LD1_Pin);
	  //printf("StartDefaultTask\n");
	  osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
	  HAL_GPIO_TogglePin(GPIOB, LD2_Pin);
	  osDelay(500);

  }
  /* USER CODE END StartTask02 */
}

/* Callback01 function */
void Callback01(void *argument)
{
  /* USER CODE BEGIN Callback01 */
	HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
  /* USER CODE END Callback01 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void tcp_client_task(void *arg) {


	int sock;
	    struct sockaddr_in server_addr;
	    char message[] = "Hello from STM32";
	    char buffer[1024];
	    int bytes_received;

	    // Create socket
	    printf("sock = socket\n");
	    sock = socket(AF_INET, SOCK_STREAM, 0);
	    if (sock < 0) {
	        printf("Failed to create socket\n");
	        vTaskDelete(NULL);
	        return;
	    }
	    printf("memset\n");
	    // Set up the server address structure
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(SERVER_PORT);
	    if (!inet_aton(SERVER_IP, &server_addr.sin_addr)) {
	            printf("Invalid IP address\n");
	            lwip_close(sock);
	            return;
	        }

	    // Set up server address
	    //server_addr.sin_family = AF_INET;
	    //server_addr.sin_port = 6000;//htons(SERVER_PORT);
	    //inet_aton(SERVER_IP, &server_addr.sin_addr);



	    // Connect to server
	    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
	        printf("Failed to connect to server : %d\n", errno);
	        close(sock);
	        vTaskDelete(NULL);

	        return;
	    }

	    printf("Connected to server\n");

	    // Send data to server
	    if (send(sock, message, strlen(message), 0) < 0) {
	        printf("Failed to send data\n");
	        close(sock);
	        vTaskDelete(NULL);
	        return;
	    }

	    printf("Data sent successfully\n");

	    // Receive data from server
	    bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
	    if (bytes_received > 0) {
	        buffer[bytes_received] = '\0';  // Null-terminate received data
	        printf("Received from server: %s\n", buffer);
	    } else {
	        printf("Failed to receive data\n");
	    }

	    // Close socket and clean up
	    close(sock);

	    vTaskDelete(NULL);  // Delete task after completion
}
/* USER CODE END Application */

