#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#define BUFFER_SIZE 5U

/*product_number这个变量，我们把它当成一种资源，我们场景中的‘包子’数量*/

static int product_number = 0;

/*定义一个信号量变量（empty_id）对可用（空）缓冲区进行倒计数，
即生产者线程可以通过从这个缓冲区获取可用缓冲区时隙*/

osSemaphoreId_t empty_id;

/*定义一个信号量变量（filled_id）对使用过的（填充的）缓冲区进行计数，
即消费者线程可以通过从这个缓冲区获取可用数据来等待用于生产者消费者的同步*/

osSemaphoreId_t filled_id;

/*线程在给定序列中获取和释放两个信号量的正确行为是至关重要的*/

void producer_thread(void *arg) {
    (void)arg;
    while(1) {
        /*如果没有令牌可用，则获取信号量标记或超时，学员自行补充*/
        osSemaphoreAcquire(empty_id, osWaitForever);
        product_number++;
        /*生产者线程，打印信息，证明生产者线程正在被执行*/
        printf("[producer_thread]%s produces a product, now product number: %d.\r\n", osThreadGetName(osThreadGetId()), product_number);
        osDelay(4);
        /*释放一个信号量令牌直到最初的最大数量，学员自行补充*/
        osSemaphoreRelease(empty_id);
    }
}

void consumer_thread(void *arg) {
    (void)arg;
    while(1){
        /*如果没有令牌可用，则获取信号量标记或超时，学员自行补充*/
        osSemaphoreAcquire(filled_id,osWaitForever);
        product_number--;   
        /*消费者线程，打印信息，证明消费者线程正在被执行*/
        printf("[consumer_thread]%s consumes a product, now product number: %d.\r\n", osThreadGetName(osThreadGetId()), product_number);
        osDelay(3);
        /*释放一个信号量令牌直到最初的最大数量，学员自行补充*/
        osSemaphoreRelease(filled_id);
    }
}
/*函数newThread统一对创建新的线程进行封装*/
osThreadId_t newThread(char *name, osThreadFunc_t func, void *arg) {
    osThreadAttr_t attr = {
        name, 0, NULL, 0, NULL, 1024*2, osPriorityNormal, 0, 0
    };
    osThreadId_t tid = osThreadNew(func, arg, &attr);
    if (tid == NULL) {
        printf("osThreadNew(%s) failed.\r\n", name);
    } else {
        printf("osThreadNew(%s) success, thread id: %d.\r\n", name, tid);
    }
    return tid;
}

void rtosv2_producer_consumer_main(void *arg) {
    (void)arg;
    /*创建信号量*/
    empty_id = osSemaphoreNew(BUFFER_SIZE, BUFFER_SIZE, NULL);
    /*创建信号量*/
    filled_id = osSemaphoreNew(BUFFER_SIZE, 0U, NULL);
    /*创建生产者线程，取线程名字为producer1，可以创建多个*/ 
    osThreadId_t ptid1 = newThread("producer1", producer_thread, NULL);
    osThreadId_t ptid2 = newThread("producer2", producer_thread, NULL);
    osThreadId_t ptid3 = newThread("producer3", producer_thread, NULL);
    /*创建生产者线程，取线程名字为consumer1，可以创建多个*/
    osThreadId_t ctid1 = newThread("consumer1", consumer_thread, NULL);
    osThreadId_t ctid2 = newThread("consumer2", consumer_thread, NULL);
    /*运行500毫秒*/
    osDelay(50);
    /*终止生产者线程，创建了就要进行销毁*/ 
    osThreadTerminate(ptid1);
    osThreadTerminate(ptid2);
    osThreadTerminate(ptid3);
    osThreadTerminate(ctid1);
    osThreadTerminate(ctid2);
    /*删除信号量*/
    osSemaphoreDelete(empty_id);
    osSemaphoreDelete(filled_id);    
}

static void TestTask(void)
{
    osThreadAttr_t attr;

    attr.name = "rtosv2_semp_main";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024;
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)rtosv2_producer_consumer_main, NULL, &attr) == NULL) {
        printf("[TestTask] Falied to create rtosv2_producer_consumer_main!\n");
    }
}

APP_FEATURE_INIT(TestTask);