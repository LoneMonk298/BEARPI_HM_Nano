#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

// 学员补充：定义互斥锁的ID
osMutexId_t mid;


// 打印任务1：输出阿拉伯数字0-9
void printer1(void)
{
    // 申请信号量，必然会阻塞睡眠，直到printer2释放信号量为止


    // 学员补充：获取互斥锁
    osMutexAcquire(mid, osWaitForever); // 临界区开始

    for (int i=0; i<10; i++)
    {
        printf("%d", i);
        osDelay(5); // 单位是10毫秒
    }

    // 学员补充：释放互斥锁
    osMutexRelease(mid); // 临界区结束
}

// 打印任务2：输出小写字母a-z
void printer2(void)
{
    // 学员补充：获取互斥锁
    osMutexAcquire(mid, osWaitForever); // 临界区开始

    for (int i=0; i<26; i++)
    {
        printf("%c", 'a' + i);
        osDelay(12); // 单位是10毫秒
    }

    // 学员补充：释放互斥锁
    osMutexRelease(mid); // 临界区结束

    // 释放信号量，相当于一个开关，让任务1可以开始运行
}

// 统一对创建新的线程进行封装
osThreadId_t newThread(char *name, osThreadFunc_t func, void *arg)
{
    // 线程属性
    osThreadAttr_t attr = {
        name, 0, NULL, 0, NULL, 1024*2, osPriorityNormal, 0, 0
    };

    // 创建线程
    osThreadId_t tid = osThreadNew(func, arg, &attr);
    if (tid == NULL)
        printf("osThreadNew(%s) failed.\r\n", name);
    else
        printf("osThreadNew(%s) success, thread id: %d.\r\n", name, tid);

    return tid;
}

void testTask(void)
{
    // 设定信号量的初始值为0

    // 学员补充：创建互斥锁
    mid = osMutexNew(NULL);

    osThreadId_t t1 = newThread("printer1", (osThreadFunc_t)printer1, NULL);
    osThreadId_t t2 = newThread("printer2", (osThreadFunc_t)printer2, NULL);

    // 运行5秒
    osDelay(500);

    // 删除线程
    osThreadTerminate(t1);
    osThreadTerminate(t2);

    // 学员补充：删除互斥锁
    osMutexDelete(mid);
}

APP_FEATURE_INIT(testTask);