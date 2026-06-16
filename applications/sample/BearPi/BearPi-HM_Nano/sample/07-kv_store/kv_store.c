/*
 * Copyright (c) 2020 Nanjing Xiaoxiongpai Intelligent Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "utils_file.h"

// Key:Value（键值对）
// 假设外部输入的信息
char ssid_key[] = "ssid";
char ssid_value[] = "studying";

char pwd_key[] = "pwd";
char pwd_value[] = "11111111";

static void Kv_Store_Task(void)
{
    char buf[512]={0};
    
    //延时2S便于查看日志
    osDelay(200);

    //根据关键字（ssid），获取值wifi1账号名，学员自行补充
    UtilsGetValue(ssid_key, buf, 512); // 在KV系统中查询键为ssid的值
    printf("---> get ssid_key's value is %s <---\n", buf);
    
    //根据关键字（pwd），获取值wifi1密码，学员自行补充
    UtilsGetValue(pwd_key, buf, 512); // 在KV系统中查询键为pwd的值
    printf("---> get pwd_key's value is %s <---\n", buf);
    
    //设置关键字（ssid）的值，学员自行补充
    UtilsSetValue(ssid_key, ssid_value); // 将ssid的值设置为studying（ssid:studying），并存储起来
    //设置关键字（pwd）的值，学员自行补充
    UtilsSetValue(pwd_key, pwd_value); // 将pwd的值设置为11111111（pwd:11111111），并存储起来
}



static void Kv_Store_Example_Entry(void)
{
    osThreadAttr_t attr;

    attr.name = "Kv_Store_Task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024*10;
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)Kv_Store_Task, NULL, &attr) == NULL)
    {
        printf("Falied to create Kv_Store_Task!\n");
    }
}

APP_FEATURE_INIT(Kv_Store_Example_Entry);