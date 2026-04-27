#include "unity.h"
#include "sl_crc.h"

#include <stddef.h>
#include <stdint.h>

void setUp(void) {
    sl_crc_init(SL_CRC_8_MAXIM_INIT_FF);
    sl_crc_init(SL_CRC_16_MCRF4XX);
}

void tearDown(void) {
}

// CRC-16/MCRF4XX 标准校准向量："123456789" → 0x6F91
void test_sl_crc_16_mcrf4xx_standard_check_value(void) {
    const uint8_t input[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    TEST_ASSERT_EQUAL_HEX16(0x6F91, sl_crc_calculate(SL_CRC_16_MCRF4XX, input, sizeof(input)));
}

void test_sl_crc_calculate_empty_returns_init(void) {
    TEST_ASSERT_EQUAL_HEX16(0xFFFF, sl_crc_calculate(SL_CRC_16_MCRF4XX, NULL, 0));
    TEST_ASSERT_EQUAL_HEX8(0xFF, sl_crc_calculate(SL_CRC_8_MAXIM_INIT_FF, NULL, 0) & 0xFFu);
}

void test_sl_crc_init_is_idempotent(void) {
    // setUp 已经初始化过；再调用一次不应有副作用
    sl_crc_init(SL_CRC_16_MCRF4XX);
    const uint8_t input[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    TEST_ASSERT_EQUAL_HEX16(0x6F91, sl_crc_calculate(SL_CRC_16_MCRF4XX, input, sizeof(input)));
}

void test_sl_crc_16_append_then_verify_succeeds(void) {
    uint8_t buf[10] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0, 0};
    sl_crc_append(SL_CRC_16_MCRF4XX, buf, sizeof(buf));
    TEST_ASSERT_TRUE(sl_crc_verify(SL_CRC_16_MCRF4XX, buf, sizeof(buf)));
}

void test_sl_crc_16_verify_rejects_tampered_payload(void) {
    uint8_t buf[10] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0, 0};
    sl_crc_append(SL_CRC_16_MCRF4XX, buf, sizeof(buf));
    buf[3] ^= 0xFFu;
    TEST_ASSERT_FALSE(sl_crc_verify(SL_CRC_16_MCRF4XX, buf, sizeof(buf)));
}

void test_sl_crc_16_verify_rejects_tampered_crc(void) {
    uint8_t buf[10] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0, 0};
    sl_crc_append(SL_CRC_16_MCRF4XX, buf, sizeof(buf));
    buf[sizeof(buf) - 1] ^= 0x01u;
    TEST_ASSERT_FALSE(sl_crc_verify(SL_CRC_16_MCRF4XX, buf, sizeof(buf)));
}

void test_sl_crc_16_append_writes_little_endian(void) {
    uint8_t buf[3] = {0xAB, 0, 0};
    sl_crc_append(SL_CRC_16_MCRF4XX, buf, sizeof(buf));

    const uint16_t expected = sl_crc_calculate(SL_CRC_16_MCRF4XX, buf, 1);
    TEST_ASSERT_EQUAL_HEX8((uint8_t)(expected & 0xFFu), buf[1]);
    TEST_ASSERT_EQUAL_HEX8((uint8_t)((expected >> 8) & 0xFFu), buf[2]);
}

void test_sl_crc_8_append_then_verify_succeeds(void) {
    uint8_t buf[5] = {0xA5, 0x5A, 0x12, 0x34, 0};
    sl_crc_append(SL_CRC_8_MAXIM_INIT_FF, buf, sizeof(buf));
    TEST_ASSERT_TRUE(sl_crc_verify(SL_CRC_8_MAXIM_INIT_FF, buf, sizeof(buf)));
}

void test_sl_crc_8_verify_rejects_tampered_byte(void) {
    uint8_t buf[5] = {0xA5, 0x5A, 0x12, 0x34, 0};
    sl_crc_append(SL_CRC_8_MAXIM_INIT_FF, buf, sizeof(buf));
    buf[2] ^= 0xFFu;
    TEST_ASSERT_FALSE(sl_crc_verify(SL_CRC_8_MAXIM_INIT_FF, buf, sizeof(buf)));
}

void test_sl_crc_verify_rejects_buffer_too_short(void) {
    uint8_t buf8[1] = {0xAB};
    uint8_t buf16[2] = {0xAB, 0xCD};
    TEST_ASSERT_FALSE(sl_crc_verify(SL_CRC_8_MAXIM_INIT_FF, buf8, sizeof(buf8)));
    TEST_ASSERT_FALSE(sl_crc_verify(SL_CRC_16_MCRF4XX, buf16, sizeof(buf16)));
}
