#include "sl_crc.h"

#include "../../config/internal.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Rocksoft 模型 CRC 参数
struct crc_config {
    uint8_t width;
    uint16_t poly;
    uint16_t init;
    bool ref_in;
    bool ref_out;
    uint16_t xor_out;
};

static const struct crc_config configs[SL_CRC_TYPE_COUNT] = {
    [SL_CRC_8_MAXIM_INIT_FF] =
        {
            .width = 8,
            .poly = 0x0031,
            .init = 0x00FF,
            .ref_in = true,
            .ref_out = true,
            .xor_out = 0x0000,
        },
    [SL_CRC_16_MCRF4XX] =
        {
            .width = 16,
            .poly = 0x1021,
            .init = 0xFFFF,
            .ref_in = true,
            .ref_out = true,
            .xor_out = 0x0000,
        },
};

// 每种 CRC 类型预计算的 256 项单字节查表，加速运行时计算
static uint16_t lookup_tables[SL_CRC_TYPE_COUNT][256];
static bool initialized[SL_CRC_TYPE_COUNT];

// 反射 val 的低 width 位
static uint16_t reflect_bits(uint16_t val, uint8_t width) {
    uint16_t result = 0;
    for (uint8_t i = 0; i < width; i++) {
        if (val & (1u << i)) {
            result |= (uint16_t)(1u << (width - 1u - i));
        }
    }
    return result;
}

void sl_crc_init(sl_crc_type_t type) {
    SL_ASSERT(type < SL_CRC_TYPE_COUNT);

    if (initialized[type]) {
        return;
    }

    const struct crc_config *cfg = &configs[type];
    const uint16_t mask = (cfg->width == 8u) ? 0x00FFu : 0xFFFFu;
    uint16_t *table = lookup_tables[type];

    if (cfg->ref_in) {
        // 反射表（LSB-first）：使用位反射后的多项式
        const uint16_t rpoly = reflect_bits(cfg->poly, cfg->width);
        for (uint16_t i = 0; i < 256u; i++) {
            uint16_t crc = i;
            for (uint8_t j = 0; j < 8u; j++) {
                crc = (crc & 1u) ? ((crc >> 1) ^ rpoly) : (crc >> 1);
            }
            table[i] = crc;
        }
    } else {
        // 标准表（MSB-first）
        const uint8_t shift = (uint8_t)(cfg->width - 8u);
        const uint16_t top_bit = (uint16_t)(1u << (cfg->width - 1u));
        for (uint16_t i = 0; i < 256u; i++) {
            uint16_t crc = (uint16_t)(i << shift);
            for (uint8_t j = 0; j < 8u; j++) {
                crc = (crc & top_bit) ? (uint16_t)((crc << 1) ^ cfg->poly) : (uint16_t)(crc << 1);
            }
            table[i] = crc & mask;
        }
    }

    initialized[type] = true;
}

uint16_t sl_crc_calculate(sl_crc_type_t type, const uint8_t *data, size_t length) {
    SL_ASSERT(data != NULL || length == 0);
    SL_ASSERT(type < SL_CRC_TYPE_COUNT);
    SL_ASSERT(initialized[type]);

    const struct crc_config *cfg = &configs[type];
    const uint16_t mask = (cfg->width == 8u) ? 0x00FFu : 0xFFFFu;
    const uint16_t *table = lookup_tables[type];
    uint16_t crc = cfg->init;

    for (size_t i = 0; i < length; i++) {
        if (cfg->ref_in) {
            if (cfg->width == 16u) {
                // 反射 16 位：新字节从 LSB 端进入
                crc = (uint16_t)((crc >> 8) ^ table[(crc ^ data[i]) & 0xFFu]);
            } else {
                // 反射 8 位
                crc = table[(crc ^ data[i]) & 0xFFu];
            }
        } else {
            if (cfg->width == 16u) {
                // 标准 16 位：新字节从 MSB 端进入
                crc = (uint16_t)(table[((crc >> 8) ^ data[i]) & 0xFFu] ^ (crc << 8));
            } else {
                // 标准 8 位
                crc = table[(crc ^ data[i]) & 0xFFu];
            }
        }
    }

    // 输出反射与输入反射不一致时的极少数情形
    if (cfg->ref_out != cfg->ref_in) {
        crc = reflect_bits(crc, cfg->width);
    }

    return (uint16_t)((crc ^ cfg->xor_out) & mask);
}

bool sl_crc_verify(sl_crc_type_t type, const uint8_t *data, size_t length) {
    SL_ASSERT(data != NULL);
    SL_ASSERT(type < SL_CRC_TYPE_COUNT);
    SL_ASSERT(initialized[type]);

    const struct crc_config *cfg = &configs[type];
    const size_t crc_len = cfg->width / 8u;

    if (length <= crc_len) {
        return false;
    }

    const uint16_t computed = sl_crc_calculate(type, data, length - crc_len);

    if (crc_len == 1u) {
        return (uint8_t)computed == data[length - 1u];
    }
    // CRC-16 little-endian（LSB 在低地址）
    return ((uint8_t)(computed & 0xFFu) == data[length - 2u]) &&
           ((uint8_t)((computed >> 8) & 0xFFu) == data[length - 1u]);
}

void sl_crc_append(sl_crc_type_t type, uint8_t *data, size_t length) {
    SL_ASSERT(data != NULL);
    SL_ASSERT(type < SL_CRC_TYPE_COUNT);
    SL_ASSERT(initialized[type]);

    const struct crc_config *cfg = &configs[type];
    const size_t crc_len = cfg->width / 8u;

    if (length <= crc_len) {
        return;
    }

    const uint16_t crc = sl_crc_calculate(type, data, length - crc_len);

    if (crc_len == 1u) {
        data[length - 1u] = (uint8_t)crc;
    } else {
        // CRC-16 little-endian（LSB 在低地址）
        data[length - 2u] = (uint8_t)(crc & 0xFFu);
        data[length - 1u] = (uint8_t)((crc >> 8) & 0xFFu);
    }
}
