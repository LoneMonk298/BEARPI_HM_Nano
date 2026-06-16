#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"

#include "iot_errno.h"
#include "iot_gpio.h"
#include "iot_gpio_ex.h"

#include "iot_i2c.h"
#include "iot_i2c_ex.h"

#define IOT_GPIO_FUNC_GPIO_0_I2C1_SDA  6
#define IOT_GPIO_FUNC_GPIO_1_I2C1_SCL  6
#define IOT_GPIO_FUNC_GPIO_8_GPIO      0

#define IOT_GPIO_FUNC_GPIO_11_GPIO     0   
#define IOT_GPIO_FUNC_GPIO_12_GPIO     0  

#define E53_GPIO         8
#define Button_F1_GPIO   11
#define Button_F2_GPIO   12

enum E53State {
    E53_ON = 0,
    E53_OFF
};

enum E53State g_E53State = E53_OFF;

static void F1_Pressed(char *arg)
{
    (void)arg;
    g_E53State = E53_ON;
}

static void F2_Pressed(char *arg)
{
    (void)arg;
    g_E53State = E53_OFF;
}

static void E53Task(void)
{
    // 初始化GPIO_8
    IoTGpioInit(E53_GPIO);
    IoTGpioSetFunc(E53_GPIO, IOT_GPIO_FUNC_GPIO_8_GPIO);
    IoTGpioSetDir(E53_GPIO, IOT_GPIO_DIR_OUT);

    // 按键F1 GPIO初始化
    IoTGpioInit(Button_F1_GPIO);
    IoTGpioSetFunc(Button_F1_GPIO, IOT_GPIO_FUNC_GPIO_11_GPIO);
    IoTGpioSetDir(Button_F1_GPIO, IOT_GPIO_DIR_IN);
    IoTGpioSetPull(Button_F1_GPIO, IOT_GPIO_PULL_UP);
    IoTGpioRegisterIsrFunc(Button_F1_GPIO, IOT_INT_TYPE_EDGE,
                           IOT_GPIO_EDGE_FALL_LEVEL_LOW, F1_Pressed, NULL);

    // 按键F2 GPIO初始化
    IoTGpioInit(Button_F2_GPIO);
    IoTGpioSetFunc(Button_F2_GPIO, IOT_GPIO_FUNC_GPIO_12_GPIO);
    IoTGpioSetDir(Button_F2_GPIO, IOT_GPIO_DIR_IN);
    IoTGpioSetPull(Button_F2_GPIO, IOT_GPIO_PULL_UP);
    IoTGpioRegisterIsrFunc(Button_F2_GPIO, IOT_INT_TYPE_EDGE,
                           IOT_GPIO_EDGE_FALL_LEVEL_LOW, F2_Pressed, NULL);

    while (1) {

        switch (g_E53State) {                      
            case E53_ON:
                IoTGpioSetOutputVal(E53_GPIO, 1);  // 电机开
                osDelay(100);
                break;
            case E53_OFF:
                IoTGpioSetOutputVal(E53_GPIO, 0);  // 电机关
                osDelay(100);
                break;
            default:
                osDelay(100);
                break;
        }
    }
}

static void MotorExampleEntry(void)                 
{
    osThreadAttr_t attr;

    attr.name = "E53Task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 2048;  
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)E53Task, NULL, &attr) == NULL)
    {
        printf("Failed to create E53Task!\n");
    }
}

APP_FEATURE_INIT(MotorExampleEntry);

