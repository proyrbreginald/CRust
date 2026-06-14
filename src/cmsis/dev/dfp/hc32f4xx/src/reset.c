#include <hc32_ll.h>
#include <symbol.h>

/**
 * @brief  Default exception/interrupt handler. Infinite loop.
 *         Weak definition — user handlers with the same name will override.
 * @param  None
 * @retval None
 */
__attribute__((used, weak)) void Default_Handler(void)
{
        while (1)
        {
                ;
        }
}

/**
 * @brief 将数据从flash拷贝到ram。
 *        Flash 源地址 (src) 可能未对齐（取决于链接脚本中段的 LMA 布局），
 *        因此统一使用字节拷贝，避免从未对齐地址做 32 位读取导致的问题。
 * @param dest 拷贝数据到该地址（RAM）。
 * @param src  从该地址拷贝数据（Flash）。
 * @param len  拷贝字节数。
 */
static inline char* reset_copy_ram_init(char* restrict dest, const char* restrict src, size_t len)
{
        while (len--)
        {
                *dest++ = *src++;
        }
        return dest;
}

/**
 * @brief 将ram指定区域清零。
 *        统一使用字节操作，简单可靠。
 * @param start 起始地址。
 * @param end   结束地址。
 */
static inline char* reset_clear_ram_uninit(char* start, char* end)
{
        uint32_t n = (uint32_t)end - (uint32_t)start;

        while (n--)
        {
                *start++ = 0u;
        }
        return start;
}

/**
 * @brief 复位处理函数，配置程序运行所需的环境，
 * 包括设置栈初始指针、开启mcu内核相关配置、初始化ram数据、
 * 根据条件跳转到指定程序开始执行。
 */
void Reset_Handler(void)
{
        // 设置初始栈指针
        __set_MSP((uint32_t)_msp_top);

        /* 加载 fast 的数据 */
        reset_copy_ram_init((char*)_fast_data_ram_start, _fast_data_flash_addr,
                            (size_t)_fast_data_ram_end -
                                    (size_t)_fast_data_ram_start);

        /* 加载 init 的数据 */
        reset_copy_ram_init((char*)_init_data_ram_start, _init_data_flash_addr,
                            (size_t)_init_data_ram_end -
                                    (size_t)_init_data_ram_start);

        /* 清空 uninit 的数据 */
        reset_clear_ram_uninit((char*)_uninit_data_ram_start,
                               (char*)_uninit_data_ram_end);

        /* 进入启动流程 */
        extern void startup(void);
        startup();
}