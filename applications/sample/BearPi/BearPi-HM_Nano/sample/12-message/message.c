#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#define QUEUE_SIZE 3
/*消息队列结构体*/
typedef struct {
    osThreadId_t tid;
    int count;
} message_entry;
/*消息队列ID*/
osMessageQueueId_t qid;

/*发送线程*/
void send_thread(void *arg) {
    static int count=0;
    message_entry sentry;
    (void)arg;
    while(1) {
        /*获取当前线程ID，并把它作为一个信息传递给recv线程*/
        sentry.tid = osThreadGetId();
        sentry.count = count;
        printf("[Message Test] %s send %d to message queue.\r\n", osThreadGetName(osThreadGetId()), count);
        
        /*消息队列发送，学员自行补充*/
        // 参数1：消息队列ID (qid)
        // 参数2：要发送的消息内容地址 (&sentry)
        // 参数3：发送的优先级 (0为默认优先级)
        // 参数4：等待超时时间 (0表示不等待，队列满了直接返回错误)
        osMessageQueuePut(qid, &sentry, 0, 0);
        
        count++;
        osDelay(5);
    }
}

/*接收线程*/
void recv_thread(void *arg) {
    (void)arg;
    message_entry rentry;
    while(1) {
        /*消息队列获取,学员自行补充*/
        // 参数1：消息队列ID (qid)
        // 参数2：存放获取到的消息的地址 (&rentry)
        // 参数3：获取消息的优先级 (NULL表示不关注优先级)
        // 参数4：等待超时时间 (osWaitForever表示一直阻塞等待，直到有消息到来)
        osMessageQueueGet(qid, &rentry, NULL, osWaitForever);
        
        /*获取到的数据进行打印*/
        printf("[Message Test] %s get %d from %s by message queue.\r\n", osThreadGetName(osThreadGetId()), rentry.count, osThreadGetName(rentry.tid));
        osDelay(3);
    }
}

/*函数newMessageThread统一对创建新的线程进行封装*/
osThreadId_t newMessageThread(char *name, osThreadFunc_t func, void *arg) {
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

void rtosv2_message_main(void *arg) {
    (void)arg;
    
    /*创建一个消息队列,学员自行补充*/
    // 参数1：消息队列能够容纳的最大消息数 (QUEUE_SIZE)
    // 参数2：每条消息的大小 (使用 sizeof 计算结构体大小)
    // 参数3：消息队列属性 (NULL表示使用默认属性)
    qid = osMessageQueueNew(QUEUE_SIZE, sizeof(message_entry), NULL);
    
    /*创建发送线程，取线程名字为send*/
    osThreadId_t tid1 = newMessageThread("send", send_thread, NULL);
    /*创建接收线程，取线程名字为recv*/
    osThreadId_t tid2 = newMessageThread("recv", recv_thread, NULL);
  
    osDelay(20);
    /*打印消息队列的信息，仅作为参考，可不打印*/
    uint32_t cap = osMessageQueueGetCapacity(qid);
    printf("[Message Test] osMessageQueueGetCapacity, capacity: %d.\r\n",cap);
    uint32_t msg_size =  osMessageQueueGetMsgSize(qid);
    printf("[Message Test] osMessageQueueGetMsgSize, size: %d.\r\n",msg_size);
    uint32_t count = osMessageQueueGetCount(qid);
    printf("[Message Test] osMessageQueueGetCount, count: %d.\r\n",count);
    uint32_t space = osMessageQueueGetSpace(qid);
    printf("[Message Test] osMessageQueueGetSpace, space: %d.\r\n",space);

    osDelay(80);
    /*销毁线程*/
    osThreadTerminate(tid1);
    osThreadTerminate(tid2);
    /*删除消息队列*/
    osMessageQueueDelete(qid);
}

static void TestTask(void)
{
    osThreadAttr_t attr;

    attr.name = "rtosv2_message_main";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024;
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)rtosv2_message_main, NULL, &attr) == NULL) {
        printf("[TestTask] Falied to create rtosv2_message_main!\n");
    }
}

APP_FEATURE_INIT(TestTask);
