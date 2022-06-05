/*Mini Task of Huge Project*/
#include<Arduino.h>
#include<Arduino_FreeRTOS.h>
#include "task.h"
#include "queue.h"
#define ULONG_MAX 0xFFFFFFFFUL

void Task1(void *pvParameters);
void Task2(void *pvParameters);

static TaskHandle_t Task1Handle = NULL;// su dung bien cuc bo cho ca chuong trinh
static TaskHandle_t Task2Handle = NULL;
QueueHandle_t  Queue;

int count =0;
int read_count = 0;

static int sensorValue = 0;

void led4(){
  digitalWrite(4,HIGH);
  delay(200);
  digitalWrite(4,LOW);
  delay(200);
}

void led5(){
  digitalWrite(5,HIGH);
  delay(200);
  digitalWrite(5,LOW);
  delay(200);
}

void led6(){
  digitalWrite(6,HIGH);
  delay(200);
  digitalWrite(6,LOW);
  delay(200);
}

void led7(){
  digitalWrite(7,HIGH);
  delay(200);
  digitalWrite(7,LOW);
  delay(200);
}

void setVariable(int a){
  Serial.print("setVariable: ");Serial.println(a);
}

void setup() 
{
  pinMode(3,OUTPUT);
  digitalWrite(3,LOW);
  pinMode(4,OUTPUT);
  digitalWrite(4,LOW);
  pinMode(5,OUTPUT);
  digitalWrite(5,LOW);
  pinMode(6,OUTPUT);
  digitalWrite(6,LOW);
  pinMode(7,OUTPUT);
  digitalWrite(7,LOW);
  
  Serial.begin(9600);

  Queue = xQueueCreate(1, sizeof(int));
  if(Queue!=NULL)
  {
  
   xTaskCreate(
    Task1
    ,  "Dist"   
    ,  128 
    ,  NULL
    ,  0 
    ,  &Task1Handle);   

   xTaskCreate(
    Task2
    ,  "Drive" 
    ,  128  
    ,  NULL
    ,  0  
    ,  &Task2Handle);     
  }
     vTaskStartScheduler(); 
}

void Task1(void *pvParameters ) 
{
  
   int notificationValue_task1;
    for(;;) 
    {
        sensorValue = analogRead(A0);
        if (sensorValue > 500)
        {
          digitalWrite(3,LOW);
          count ++;
          Serial.print("count: ");Serial.println(count);
          Serial.print("sensorValue: ");Serial.println(sensorValue);

          vTaskDelay(200);       
        }
        if (sensorValue < 200)
        {
          digitalWrite(3,HIGH);
          Serial.println("nothing happened");
          vTaskDelay(200);
        }
        if (count ==3)
        {
          xQueueSend (Queue,&count,portMAX_DELAY);
          xTaskNotify(Task2Handle,0,eNoAction);
          vTaskDelay(pdMS_TO_TICKS(100));
        }
        if (count ==5)
        {
          xQueueSend (Queue,&count,portMAX_DELAY);
          xTaskNotify(Task2Handle,0,eNoAction);
          vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}


void Task2  (void *pvParameters ) 
{

    uint32_t ulNotifiedValue;
    enum States{FORWARD,REVERSE,TURN,STOP};
    States state=FORWARD;
    for(;;) 
    {  
      switch(state) 
      {
        case FORWARD:        
        led4();
        setVariable(110);
        
        xTaskNotifyWaitIndexed( 0,
                        ULONG_MAX, /* Clear all notification bits on entry. */
                        ULONG_MAX, /* Reset the notification value to 0 on exit. */
                        &ulNotifiedValue,
                        portMAX_DELAY);

        xQueueReceive(Queue,&read_count,portMAX_DELAY);
        Serial.print("the varialable named count was sent by Task1 (3 or 5) is: ");Serial.println(read_count);
        if (read_count==3)
        { 
          Serial.println("count = 3, then this task is unblocked");
          state=REVERSE;
        }
        if (read_count ==5)
        {
          Serial.println("count = 5, then this task is unblocked");
          state = STOP;
        }
        break;
        
        case REVERSE:
        led5();
        setVariable(255);
        vTaskDelay(pdMS_TO_TICKS((1000+random(-500, 50))));
        state=TURN;      
        break;
  
        case TURN:     
        led6();
        setVariable(190);
        vTaskDelay(pdMS_TO_TICKS((1000+random(-500, 50))));
        state=FORWARD;      
        break; 

        case STOP:
        led7();
        setVariable(0);
        
    }
  }
}






void loop() 
{
    // empty
}



    
