#ifndef __SYMBOL_H_
#define __SYMBOL_H_

/**
 * @brief 导出堆栈符号。
 */
extern const char _msp_top[]; //!< 初始栈地址(从高地址向底地址生长)
extern const char _msp_limit[];  //!< 栈最小大小
extern const char _heap_start[]; //!< 紧跟在数据段之后
extern const char _heap_end[]; //!< 堆结束地址(紧靠栈且不覆盖最小栈空间)

/**
 * @brief 导入 fast 初始化符号定义。
 */
extern const char _fast_data_flash_addr[]; //!< flash 中快速代码段的起始地址
extern const char _fast_data_ram_start[]; //!< fast 中快速代码段的起始地址
extern const char _fast_data_ram_end[]; //!< fast 中快速代码段的结束地址

/**
 * @brief 导入 init 初始化符号定义。
 */
extern const char _init_data_flash_addr[]; //!< flash 中有值数据的起始地址
extern const char _init_data_ram_start[]; //!< init 中有值数据的起始地址
extern const char _init_data_ram_end[]; //!< init 中有值数据的结束地址

/**
 * @brief 导入 uninit 初始化符号定义。
 */
extern const char _uninit_data_ram_start[]; //!< uninit 中无值数据的起始地址
extern const char _uninit_data_ram_end[]; //!< uninit 中无值数据的结束地址

#endif /* __SYMBOL_H_ */