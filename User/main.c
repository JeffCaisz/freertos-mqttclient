#include "main.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"

#include "mqttclient.h"

//#define TEST_USEING_TLS  

extern const char *test_ca_get(void);

static TaskHandle_t app_task_create_handle = NULL;/* ���������� */
static TaskHandle_t mqtt_task_handle = NULL;/* LED������ */

static mqtt_client_t client;
static client_init_params_t init_params;

static void app_task_create(void);/* ���ڴ������� */

static void mqtt_task(void* pvParameters);/* mqtt_task����ʵ�� */

extern void TCPIP_Init(void);

/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  /* ������Ӳ����ʼ�� */
  BSP_Init();

  /* ����app_task_create���� */
  xReturn = xTaskCreate((TaskFunction_t )app_task_create,  /* ������ں��� */
                        (const char*    )"app_task_create",/* �������� */
                        (uint16_t       )2048,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )10, /* ��������ȼ� */
                        (TaskHandle_t*  )&app_task_create_handle);/* ������ƿ�ָ�� */ 
  /* ����������� */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* �������񣬿������� */
  else
    return -1;  
  
  while(1);   /* ��������ִ�е����� */    
}


/***********************************************************************
  * @ ������  �� app_task_create
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void app_task_create(void)
{
    int err;
    
    BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */

    TCPIP_Init();

    mqtt_log_init();
    
    MQTT_LOG_I("\nwelcome to mqttclient test...\n");

    init_params.read_buf_size = 1024;
    init_params.write_buf_size = 1024;

#ifdef TEST_USEING_TLS
    init_params.network.ca_crt = test_ca_get();
    init_params.network.port = "8883";
#else
    init_params.network.port = "1883";
#endif
    init_params.network.host = "www.jiejie01.top"; //"47.95.164.112";//"jiejie01.top"; //"129.204.201.235"; //"192.168.1.101";

    init_params.connect_params.user_name = random_string(10); // random_string(10); //"jiejietop-acer1";
    init_params.connect_params.password = random_string(10); //random_string(10); // "123456";
    init_params.connect_params.client_id = random_string(10); //random_string(10); // "clientid-acer1";
    init_params.connect_params.clean_session = 1;
    
    mqtt_init(&client, &init_params);

    err = mqtt_connect(&client);
    
    MQTT_LOG_I("mqtt_connect err = %d", err);
    
    err = mqtt_subscribe(&client, "freertos-topic", QOS0, NULL);
    
    MQTT_LOG_I("mqtt_subscribe err = %d", err);    


    taskENTER_CRITICAL();           //�����ٽ���

    /* ����mqtt_task���� */
    xReturn = xTaskCreate((TaskFunction_t )mqtt_task, /* ������ں��� */
                        (const char*    )"mqtt_task",/* �������� */
                        (uint16_t       )2048,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )10,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&mqtt_task_handle);/* ������ƿ�ָ�� */
    if(pdPASS == xReturn)
        printf("Create mqtt_task sucess...\r\n");

    vTaskDelete(app_task_create_handle); //ɾ��app_task_create����

    taskEXIT_CRITICAL();            //�˳��ٽ���
}



/**********************************************************************
  * @ ������  �� mqtt_task
  * @ ����˵���� mqtt_task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void mqtt_task(void* parameter)
{	
    char buf[100] = { 0 };
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));
    sprintf(buf, "welcome to mqttclient, this is a publish test...");

    vTaskDelay(4000);
    
    mqtt_list_subscribe_topic(&client);

    msg.payload = (void *) buf;
    msg.qos = QOS0;
    
    while(1) {
        sprintf(buf, "welcome to mqttclient, this is a publish test, a rand number: %d ...", random_number());

        mqtt_publish(&client, "freertos-topic", &msg);
        
        vTaskDelay(4000);
    }
}
