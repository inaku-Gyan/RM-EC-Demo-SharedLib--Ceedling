#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * 受支持的 CRC 算法变体（Rocksoft 模型参数）：
 *   (width, poly, init, ref_in, ref_out, xor_out)
 *
 * T = true（位反射，LSB-first），F = false（标准 MSB-first）
 */
typedef enum {
    /**
     * @brief 非标准 CRC-8/MAXIM，初始值改为 0xFF。
     *
     * (8, 0x31, 0xFF, T, T, 0x00)
     * @note Dji RoboMaster 裁判系统串口协议 CRC-8 帧头校验
     */
    SL_CRC_8_MAXIM_INIT_FF = 0,

    /**
     * @brief 标准 CRC-16/MCRF4XX。
     *
     * (16, 0x1021, 0xFFFF, T, T, 0x0000)
     * @note Dji RoboMaster 裁判系统串口协议 CRC-16 整包校验；VTM 图传 RC 数据校验
     */
    SL_CRC_16_MCRF4XX,

    SL_CRC_TYPE_COUNT
} sl_crc_type_t;

/** 构建指定 CRC 类型的查表，使用前必须调用一次；重复调用为安全 no-op。 */
void sl_crc_init(sl_crc_type_t type);

/**
 * 计算缓冲区的 CRC。
 * @param type   CRC 类型，必须先 sl_crc_init。
 * @param data   输入数据。
 * @param length 字节数。
 * @return       CRC 值（CRC-8 占低 8 位，CRC-16 占完整 16 位）。
 */
uint16_t sl_crc_calculate(sl_crc_type_t type, const uint8_t *data, size_t length);

/**
 * 校验缓冲区末尾追加的 CRC 字节是否匹配。CRC-16 字节按 little-endian 排列（LSB 在低地址）。
 * @param type   CRC 类型，必须先 sl_crc_init。
 * @param data   含 CRC 尾的缓冲区。
 * @param length 缓冲区总字节数（含 CRC 尾）。
 */
bool sl_crc_verify(sl_crc_type_t type, const uint8_t *data, size_t length);

/**
 * 计算并把 CRC 字节写入缓冲区末尾保留位。CRC-16 字节按 little-endian 排列（LSB 在低地址）。
 * @param type   CRC 类型，必须先 sl_crc_init。
 * @param data   含 CRC 尾保留位的缓冲区。
 * @param length 缓冲区总字节数（含保留位）。
 */
void sl_crc_append(sl_crc_type_t type, uint8_t *data, size_t length);
