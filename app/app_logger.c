#include "elog.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "main.h"

#define LOG_EVT_OUTPUT  (1 << 0) // 定义日志输出事件的通知位（使用 FreeRTOS Task Notification）

extern void elog_port_output(const char *log, size_t size);

static TaskHandle_t elog_task_handle; // 静态变量：用于保存日志任务的句柄，用于通知该任务

static void elog_task_handler(void *args)
{
    char log_buff[ELOG_LINE_BUF_SIZE];        // 日志行缓冲区
    size_t log_size;
    uint32_t notify_bits;             // 用于接收任务通知的标志位

    for(;;)
    {
        // 1. 阻塞等待通知
        // xTaskNotifyWait(0, 0xffffffff, &notif_bits, portMAX_DELAY);
        // - ulClearOnEntry: 0, 任务进入阻塞前不清除任何通知位
        // - ulClearOnExit: 0xffffffff, 任务被唤醒后清除所有通知位（等待任何通知）
        // - pulNotificationValue: &notif_bits, 接收到的通知位存储在这里
        // - xTicksToWait: portMAX_DELAY, 永久阻塞，直到被通知
        xTaskNotifyWait(0, 0xffffffff, &notify_bits, portMAX_DELAY);

        // 2. 循环处理所有积压的日志
        while (true)
        {
            // 从 elog 内部的异步环形缓冲区中获取一行日志
            log_size = elog_async_get_log(log_buff, ELOG_LINE_BUF_SIZE);

            if (log_size)
            {
                // 如果成功获取到日志 (log_size > 0)，则通过驱动接口输出
                elog_port_output(log_buff, log_size);
            }
            else
            {
                // 缓冲区为空，所有日志已处理完毕，跳出内层循环
                break;
            }
        
        }
    }
}

static void elog_lowlevel_init(void)
{
    elog_init(); // 初始化 elog 库

    // 设置不同日志级别的输出格式
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL); // ASSERT 级别：输出所有信息
    // ERROR/WARN 级别：输出级别、标签、时间戳、进程/线程信息
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME | ELOG_FMT_P_INFO);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME | ELOG_FMT_P_INFO);
    // INFO/DEBUG 级别：输出级别、标签、时间戳
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_TAG); // VERBOSE 级别：只输出标签

    elog_start(); // 启动日志系统（开始接收日志写入）
}

void app_logger_init(void)
{
    elog_lowlevel_init(); // 调用低级初始化函数

    // 创建日志处理任务
    xTaskCreate(elog_task_handler, "log", 512, NULL, 1, &elog_task_handle);
    configASSERT(elog_task_handle); // 确保任务创建成功
}

//当有新的日志被写入 elog 缓冲区时，elog 库会调用此函数来通知日志任务进行处理
void elog_async_output_notice(void)
{
    // 确保日志任务句柄已创建（否则无法通知）
    CHECK_RET(elog_task_handle); 

    // 区分当前执行上下文是任务上下文还是中断上下文
    if (is_user()) // 如果在用户（任务）上下文
    {
        // 调用任务级通知 API，唤醒 elog 任务
        xTaskNotify(elog_task_handle, LOG_EVT_OUTPUT, eSetBits);
    }
    else // 如果在中断上下文
    {
        // 调用中断安全通知 API
        BaseType_t xHigherPriorityTaskWoken;
        xTaskNotifyFromISR(elog_task_handle, LOG_EVT_OUTPUT, eSetBits, &xHigherPriorityTaskWoken);
        
        // 如果此通知唤醒了更高优先级的任务（即 elog 任务），则进行上下文切换
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}