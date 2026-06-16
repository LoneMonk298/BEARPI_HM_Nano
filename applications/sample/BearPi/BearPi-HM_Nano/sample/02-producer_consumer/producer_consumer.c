#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

// 1. 生产者线程函数
void producer_thread(void *arg) {
    (void)arg;
    while(1) {
        // 打印线程名，证明生产者在运行
        printf("[producer_thread] %s produces a product.\r\n", osThreadGetName(osThreadGetId()));
        osDelay(4);  // 延时40ms
    }
}

// 2. 消费者线程函数
void consumer_thread(void *arg) {
    (void)arg;
    while(1){
        // 打印线程名，证明消费者在运行
        printf("[consumer_thread] %s consumes a product.\r\n", osThreadGetName(osThreadGetId()));
        osDelay(3);  // 延时30ms
    }
}

// 3. 线程创建封装函数
osThreadId_t newThread(char *name, osThreadFunc_t func, void *arg) {
    osThreadAttr_t attr = {
        .name = name,
        .attr_bits = 0,
        .cb_mem = NULL,
        .cb_size = 0,
        .stack_mem = NULL,
        .stack_size = 1024*2,  // 栈大小2K
        .priority = osPriorityNormal,  // 普通优先级
        .tz_module = 0,
        .reserved = 0
    };
    osThreadId_t tid = osThreadNew(func, arg, &attr);
    if (tid == NULL) {
        printf("osThreadNew(%s) failed.\r\n", name);
    } else {
        printf("osThreadNew(%s) success, thread id: %d.\r\n", name, tid);
    }
    return tid;
}

// 4. 主任务：创建、运行、终止线程
void rtosv2_producer_consumer_main(void *arg) {
    (void)arg;
    
    // 创建生产者线程（名字：producer1）
    osThreadId_t producer_tid = newThread("producer1", producer_thread, NULL);
    // 创建消费者线程（名字：consumer1）
    osThreadId_t consumer_tid = newThread("consumer1", consumer_thread, NULL);

    // 线程运行500ms
    osDelay(50);

    // 终止生产者线程
    if (producer_tid != NULL) {
        osThreadTerminate(producer_tid);
        printf("[main] producer1 thread terminated.\r\n");
    }
    // 终止消费者线程
    if (consumer_tid != NULL) {
        osThreadTerminate(consumer_tid);
        printf("[main] consumer1 thread terminated.\r\n");
    }

    // 线程结束后退出
    osThreadExit();
}

// 5. 入口任务：系统启动后自动运行
static void TestTask(void)
{
    osThreadAttr_t attr = {
        .name = "rtosv2_producer_main",
        .stack_size = 1024,
        .priority = osPriorityNormal
    };

    // 创建主任务线程
    if (osThreadNew((osThreadFunc_t)rtosv2_producer_consumer_main, NULL, &attr) == NULL) {
        printf("[TestTask] Failed to create main thread!\n");
    }
}

// 6. 系统入口：开机自动启动任务
APP_FEATURE_INIT(TestTask);