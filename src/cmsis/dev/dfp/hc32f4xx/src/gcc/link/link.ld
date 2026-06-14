/* 描述芯片物理内存区域 */
MEMORY
{
    FULL_ACCESS_FLASH   (rx)    : ORIGIN = 0x00000000, LENGTH = 512K    /* 主机总线全访问 FULL_ACCESS_FLASH */
    OTP_DATA            (rx)    : ORIGIN = 0x03000C00, LENGTH = 960
    OTP_LOCK            (rx)    : ORIGIN = 0x03000FC0, LENGTH = 60
    FULL_ACCESS_RAM     (rwx)   : ORIGIN = 0x1FFF8000, LENGTH = 32K     /* 主机总线全访问 RAM */
    RAM                 (rwx)   : ORIGIN = 0x20000000, LENGTH = 128K    /* 普通 RAM */
    ECC_RAM             (rwx)   : ORIGIN = 0x20020000, LENGTH = 28K     /* 具有 ECC 功能的 RAM */
    RET_RAM             (rwx)   : ORIGIN = 0x200F0000, LENGTH = 4K      /* 支持在掉电模式下运行的 RAM */
}

/* MSP 栈顶 */
_msp_top = ORIGIN(FULL_ACCESS_RAM) + LENGTH(FULL_ACCESS_RAM);

/* 保留 4KB 栈空间 */
_msp_limit = _msp_top - 0x1000;

/* 描述程序段在内存中的分配 */
SECTIONS
{
    /* 复位向量表 */
    .reset ORIGIN(FULL_ACCESS_FLASH) : AT(0x00000000)
    {
        /* 把复位向量表放在 FULL_ACCESS_FLASH 的起始地址 */
        KEEP(*(.reset_vector_table))
    } > FULL_ACCESS_FLASH

    /* 硬件自动初始化配置 */
    .icg 0x00000400 : AT(0x00000400)
    {
        KEEP(*(.icg_sec))
    } > FULL_ACCESS_FLASH

    /* 普通代码 */
    .code :
    {
        . = ALIGN(4);
        *(.text*)
    } > FULL_ACCESS_FLASH

    /* 程序常量 */
    .const :
    {
        . = ALIGN(4);
        *(.rodata*)
        KEEP(*(SORT(.rti_fn*)))
        __fsymtab_start = .;
        KEEP(*(FSymTab*))
        __fsymtab_end = .;
    } > FULL_ACCESS_FLASH

    /* ARM */
    /* .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FULL_ACCESS_FLASH */

    /* 快速代码 */
    .fast :
    {
        . = ALIGN(4);
        _fast_data_ram_start = .;
        *(.fast*)
        _fast_data_ram_end = .;
    } > FULL_ACCESS_RAM AT > FULL_ACCESS_FLASH
    _fast_data_flash_addr = LOADADDR(.fast);

    /* 已初始化数据 */
    .init :
    {
        . = ALIGN(4);
        _init_data_ram_start = .;
        *(.rtt_cb*)
        *(.data*)
        _init_data_ram_end = .;
    } > RAM AT > FULL_ACCESS_FLASH
    _init_data_flash_addr = LOADADDR(.init);

    /* 未初始化数据 */
    .uninit :
    {
        . = ALIGN(4);
        _uninit_data_ram_start = .;
        *(.rtt_buf*)
        *(.bss*)
        *(.noinit*)
        _uninit_data_ram_end = .;
    } > RAM

    /* 堆内存 */
    .heap :
    {
        . = ALIGN(4);
        _heap_start = .;
        _heap_end = ORIGIN(RAM) + LENGTH(RAM);
    } > RAM

    /* 数据错误检查与纠正 */
    .ecc :
    {
        . = ALIGN(4);
        *(.ecc*)
    } > RET_RAM

    /* 数据在掉电模式下持续保持 */
    .retain :
    {
        . = ALIGN(4);
        *(.retain*)
    } > RET_RAM
}

/* 确保快速代码不会覆盖 MSP 栈空间 */
ASSERT(_fast_data_ram_end <= _msp_limit, ".fast section overlaps MSP stack space")

/* 定义调试时的程序入口点 */
ENTRY(Reset_Handler)

/* 导出 ICG 配置值，避免 ICG 段被裁剪掉 */
EXTERN(u32ICGValue)

/* 保持 Default_Handler 不被 --gc-sections 裁剪 */
EXTERN(Default_Handler)

/* 为 CMSIS 标准异常处理函数名提供弱默认值 */
PROVIDE(NMI_Handler = Default_Handler);
PROVIDE(HardFault_Handler = Default_Handler);
PROVIDE(MemManage_Handler = Default_Handler);
PROVIDE(BusFault_Handler = Default_Handler);
PROVIDE(UsageFault_Handler = Default_Handler);
PROVIDE(SVC_Handler = Default_Handler);
PROVIDE(DebugMon_Handler = Default_Handler);
PROVIDE(PendSV_Handler = Default_Handler);
PROVIDE(SysTick_Handler = Default_Handler);

/* 为外设中断处理函数名提供弱默认值 */
PROVIDE(IRQ000_Handler = Default_Handler);
PROVIDE(IRQ001_Handler = Default_Handler);
PROVIDE(IRQ002_Handler = Default_Handler);
PROVIDE(IRQ003_Handler = Default_Handler);
PROVIDE(IRQ004_Handler = Default_Handler);
PROVIDE(IRQ005_Handler = Default_Handler);
PROVIDE(IRQ006_Handler = Default_Handler);
PROVIDE(IRQ007_Handler = Default_Handler);
PROVIDE(IRQ008_Handler = Default_Handler);
PROVIDE(IRQ009_Handler = Default_Handler);
PROVIDE(IRQ010_Handler = Default_Handler);
PROVIDE(IRQ011_Handler = Default_Handler);
PROVIDE(IRQ012_Handler = Default_Handler);
PROVIDE(IRQ013_Handler = Default_Handler);
PROVIDE(IRQ014_Handler = Default_Handler);
PROVIDE(IRQ015_Handler = Default_Handler);
PROVIDE(IRQ016_Handler = Default_Handler);
PROVIDE(IRQ017_Handler = Default_Handler);
PROVIDE(IRQ018_Handler = Default_Handler);
PROVIDE(IRQ019_Handler = Default_Handler);
PROVIDE(IRQ020_Handler = Default_Handler);
PROVIDE(IRQ021_Handler = Default_Handler);
PROVIDE(IRQ022_Handler = Default_Handler);
PROVIDE(IRQ023_Handler = Default_Handler);
PROVIDE(IRQ024_Handler = Default_Handler);
PROVIDE(IRQ025_Handler = Default_Handler);
PROVIDE(IRQ026_Handler = Default_Handler);
PROVIDE(IRQ027_Handler = Default_Handler);
PROVIDE(IRQ028_Handler = Default_Handler);
PROVIDE(IRQ029_Handler = Default_Handler);
PROVIDE(IRQ030_Handler = Default_Handler);
PROVIDE(IRQ031_Handler = Default_Handler);
PROVIDE(IRQ032_Handler = Default_Handler);
PROVIDE(IRQ033_Handler = Default_Handler);
PROVIDE(IRQ034_Handler = Default_Handler);
PROVIDE(IRQ035_Handler = Default_Handler);
PROVIDE(IRQ036_Handler = Default_Handler);
PROVIDE(IRQ037_Handler = Default_Handler);
PROVIDE(IRQ038_Handler = Default_Handler);
PROVIDE(IRQ039_Handler = Default_Handler);
PROVIDE(IRQ040_Handler = Default_Handler);
PROVIDE(IRQ041_Handler = Default_Handler);
PROVIDE(IRQ042_Handler = Default_Handler);
PROVIDE(IRQ043_Handler = Default_Handler);
PROVIDE(IRQ044_Handler = Default_Handler);
PROVIDE(IRQ045_Handler = Default_Handler);
PROVIDE(IRQ046_Handler = Default_Handler);
PROVIDE(IRQ047_Handler = Default_Handler);
PROVIDE(IRQ048_Handler = Default_Handler);
PROVIDE(IRQ049_Handler = Default_Handler);
PROVIDE(IRQ050_Handler = Default_Handler);
PROVIDE(IRQ051_Handler = Default_Handler);
PROVIDE(IRQ052_Handler = Default_Handler);
PROVIDE(IRQ053_Handler = Default_Handler);
PROVIDE(IRQ054_Handler = Default_Handler);
PROVIDE(IRQ055_Handler = Default_Handler);
PROVIDE(IRQ056_Handler = Default_Handler);
PROVIDE(IRQ057_Handler = Default_Handler);
PROVIDE(IRQ058_Handler = Default_Handler);
PROVIDE(IRQ059_Handler = Default_Handler);
PROVIDE(IRQ060_Handler = Default_Handler);
PROVIDE(IRQ061_Handler = Default_Handler);
PROVIDE(IRQ062_Handler = Default_Handler);
PROVIDE(IRQ063_Handler = Default_Handler);
PROVIDE(IRQ064_Handler = Default_Handler);
PROVIDE(IRQ065_Handler = Default_Handler);
PROVIDE(IRQ066_Handler = Default_Handler);
PROVIDE(IRQ067_Handler = Default_Handler);
PROVIDE(IRQ068_Handler = Default_Handler);
PROVIDE(IRQ069_Handler = Default_Handler);
PROVIDE(IRQ070_Handler = Default_Handler);
PROVIDE(IRQ071_Handler = Default_Handler);
PROVIDE(IRQ072_Handler = Default_Handler);
PROVIDE(IRQ073_Handler = Default_Handler);
PROVIDE(IRQ074_Handler = Default_Handler);
PROVIDE(IRQ075_Handler = Default_Handler);
PROVIDE(IRQ076_Handler = Default_Handler);
PROVIDE(IRQ077_Handler = Default_Handler);
PROVIDE(IRQ078_Handler = Default_Handler);
PROVIDE(IRQ079_Handler = Default_Handler);
PROVIDE(IRQ080_Handler = Default_Handler);
PROVIDE(IRQ081_Handler = Default_Handler);
PROVIDE(IRQ082_Handler = Default_Handler);
PROVIDE(IRQ083_Handler = Default_Handler);
PROVIDE(IRQ084_Handler = Default_Handler);
PROVIDE(IRQ085_Handler = Default_Handler);
PROVIDE(IRQ086_Handler = Default_Handler);
PROVIDE(IRQ087_Handler = Default_Handler);
PROVIDE(IRQ088_Handler = Default_Handler);
PROVIDE(IRQ089_Handler = Default_Handler);
PROVIDE(IRQ090_Handler = Default_Handler);
PROVIDE(IRQ091_Handler = Default_Handler);
PROVIDE(IRQ092_Handler = Default_Handler);
PROVIDE(IRQ093_Handler = Default_Handler);
PROVIDE(IRQ094_Handler = Default_Handler);
PROVIDE(IRQ095_Handler = Default_Handler);
PROVIDE(IRQ096_Handler = Default_Handler);
PROVIDE(IRQ097_Handler = Default_Handler);
PROVIDE(IRQ098_Handler = Default_Handler);
PROVIDE(IRQ099_Handler = Default_Handler);
PROVIDE(IRQ100_Handler = Default_Handler);
PROVIDE(IRQ101_Handler = Default_Handler);
PROVIDE(IRQ102_Handler = Default_Handler);
PROVIDE(IRQ103_Handler = Default_Handler);
PROVIDE(IRQ104_Handler = Default_Handler);
PROVIDE(IRQ105_Handler = Default_Handler);
PROVIDE(IRQ106_Handler = Default_Handler);
PROVIDE(IRQ107_Handler = Default_Handler);
PROVIDE(IRQ108_Handler = Default_Handler);
PROVIDE(IRQ109_Handler = Default_Handler);
PROVIDE(IRQ110_Handler = Default_Handler);
PROVIDE(IRQ111_Handler = Default_Handler);
PROVIDE(IRQ112_Handler = Default_Handler);
PROVIDE(IRQ113_Handler = Default_Handler);
PROVIDE(IRQ114_Handler = Default_Handler);
PROVIDE(IRQ115_Handler = Default_Handler);
PROVIDE(IRQ116_Handler = Default_Handler);
PROVIDE(IRQ117_Handler = Default_Handler);
PROVIDE(IRQ118_Handler = Default_Handler);
PROVIDE(IRQ119_Handler = Default_Handler);
PROVIDE(IRQ120_Handler = Default_Handler);
PROVIDE(IRQ121_Handler = Default_Handler);
PROVIDE(IRQ122_Handler = Default_Handler);
PROVIDE(IRQ123_Handler = Default_Handler);
PROVIDE(IRQ124_Handler = Default_Handler);
PROVIDE(IRQ125_Handler = Default_Handler);
PROVIDE(IRQ126_Handler = Default_Handler);
PROVIDE(IRQ127_Handler = Default_Handler);
PROVIDE(IRQ128_Handler = Default_Handler);
PROVIDE(IRQ129_Handler = Default_Handler);
PROVIDE(IRQ130_Handler = Default_Handler);
PROVIDE(IRQ131_Handler = Default_Handler);
PROVIDE(IRQ132_Handler = Default_Handler);
PROVIDE(IRQ133_Handler = Default_Handler);
PROVIDE(IRQ134_Handler = Default_Handler);
PROVIDE(IRQ135_Handler = Default_Handler);
PROVIDE(IRQ136_Handler = Default_Handler);
PROVIDE(IRQ137_Handler = Default_Handler);
PROVIDE(IRQ138_Handler = Default_Handler);
PROVIDE(IRQ139_Handler = Default_Handler);
PROVIDE(IRQ140_Handler = Default_Handler);
PROVIDE(IRQ141_Handler = Default_Handler);
PROVIDE(IRQ142_Handler = Default_Handler);
PROVIDE(IRQ143_Handler = Default_Handler);
