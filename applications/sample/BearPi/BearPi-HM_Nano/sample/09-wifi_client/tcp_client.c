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

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "lwip/sockets.h"
#include "wifi_connect.h"

// 配置wifi账号密码，学员自行补充
#define CONFIG_WIFI_SSID "studying"
#define CONFIG_WIFI_PWD "11111111"
// 配置服务器IP和端口 ，学员自行补充
#define CONFIG_SERVER_ADDR "192.168.206.221" 
#define CONFIG_SERVER_PORT 51380

//在sock_fd 进行监听，在 new_fd 接收新的链接
int sock_fd;
int addr_length;
static const char *send_data = "Hello! I'm BearPi-HM_Nano TCP Client!\r\n";

static void TCPClientTask(void)
{
    //服务器的地址信息
    struct sockaddr_in send_addr = {0};
    socklen_t addr_length = sizeof(send_addr);
    char recvBuf[512];
    uint8_t ret;

    //连接Wifi
    WifiConnect(CONFIG_WIFI_SSID, CONFIG_WIFI_PWD);

    //创建socket，学员自行补充
    if(sock_fd = socket(AF_INET, SOCK_STREAM, 0) == -1)
    {
        printf("创建socket失败\n");
        return 0;
    }

    //初始化预连接的服务端地址，学员自行补充
    send_addr.sin_family = AF_INET;
    send_addr.sin_addr.s_addr = inet_addr(CONFIG_SERVER_ADDR);
    send_addr.sin_port = htons(CONFIG_SERVER_PORT);
    addr_length = sizeof(send_addr);


    // 连接ubuntu的tcp服务器
    connect(sock_fd, (struct sockaddr *)&send_addr, addr_length);

    while (1)
    {
        bzero(recvBuf, sizeof(recvBuf));
		if(send(sock_fd, send_data, strlen(send_data), 0) < 0)
        {
            printf("send error\r\n");
            goto disconnect;    
        }
        if(recv(sock_fd, recvBuf, sizeof(recvBuf), 0) < 0)
        {
            printf("recv error\r\n");
            goto disconnect;   
        }
		printf("recv :%s\r\n", recvBuf);
    }
disconnect:
    //关闭这个 socket
    printf("closesocket!\r\n");
    close(sock_fd);
closesocket:
    //关闭这个 socket
    printf("closesocket!\r\n");
    close(sock_fd);
}


static void TCPClientDemo(void)
{
    osThreadAttr_t attr;

    attr.name = "TCPClientTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 10240;
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)TCPClientTask, NULL, &attr) == NULL)
    {
        printf("[TCPClientDemo] Falied to create TCPClientTask!\n");
    }
}

APP_FEATURE_INIT(TCPClientDemo);
