#include "protocol_ioprox.h"
#include <furi.h>
#include <cli/cli.h>

//Index map
//0           10          20          30          40          50          60
//|           |           |           |           |           |           |
//01234567 8 90123456 7 89012345 6 78901234 5 67890123 4 56789012 3 45678901 23
//-----------------------------------------------------------------------------
//00000000 0 11110000 1 facility 1 version* 1 code*one 1 code*two 1 ???????? 11
//
//XSF(version)facility:codeone+codetwo

typedef uint32_t IoProxCardData;

constexpr uint8_t IoProxCount = 3;
constexpr uint8_t IoProxBitSize = sizeof(IoProxCardData) * 8;

static void write_raw_bit(bool bit, uint8_t position, IoProxCardData* card_data) {
    if(bit) {
        card_data[position / IoProxBitSize] |= 1UL
                                               << (IoProxBitSize - (position % IoProxBitSize) - 1);
    } else {
        card_data[position / (sizeof(IoProxCardData) * 8)] &=
            ~(1UL << (IoProxBitSize - (position % IoProxBitSize) - 1));
    }
}

static void write_bit(bool bit, uint8_t position, IoProxCardData* card_data) {
    write_raw_bit(bit, position + 0, card_data);
    write_raw_bit(!bit, position + 1, card_data);
}

uint8_t ProtocolIoProx::get_encoded_data_size() {
    return 8;
}

uint8_t ProtocolIoProx::get_decoded_data_size() {
    return 4;
}

void ProtocolIoProx::encode(
    const uint8_t* decoded_data,
    const uint8_t decoded_data_size,
    uint8_t* encoded_data,
    const uint8_t encoded_data_size) {
    furi_check(decoded_data_size >= get_decoded_data_size());
    furi_check(encoded_data_size >= get_encoded_data_size());

    IoProxCardData card_data[IoProxCount] = {0, 0, 0};

    uint32_t fc_cn = (decoded_data[0] << 16) | (decoded_data[1] << 8) | decoded_data[2];

    // even parity sum calculation (high 12 bits of data)
    uint8_t even_parity_sum = 0;
    for(int8_t i = 12; i < 24; i++) {
        if(((fc_cn >> i) & 1) == 1) {
            even_parity_sum++;
        }
    }

    // odd parity sum calculation (low 12 bits of data)
    uint8_t odd_parity_sum = 1;
    for(int8_t i = 0; i < 12; i++) {
        if(((fc_cn >> i) & 1) == 1) {
            odd_parity_sum++;
        }
    }

    // 0x1D preamble
    write_raw_bit(0, 0, card_data);
    write_raw_bit(0, 1, card_data);
    write_raw_bit(0, 2, card_data);
    write_raw_bit(1, 3, card_data);
    write_raw_bit(1, 4, card_data);
    write_raw_bit(1, 5, card_data);
    write_raw_bit(0, 6, card_data);
    write_raw_bit(1, 7, card_data);

    // company / OEM code 1
    write_bit(0, 8, card_data);
    write_bit(0, 10, card_data);
    write_bit(0, 12, card_data);
    write_bit(0, 14, card_data);
    write_bit(0, 16, card_data);
    write_bit(0, 18, card_data);
    write_bit(1, 20, card_data);

    // card format / length 1
    write_bit(0, 22, card_data);
    write_bit(0, 24, card_data);
    write_bit(0, 26, card_data);
    write_bit(0, 28, card_data);
    write_bit(0, 30, card_data);
    write_bit(0, 32, card_data);
    write_bit(0, 34, card_data);
    write_bit(0, 36, card_data);
    write_bit(0, 38, card_data);
    write_bit(0, 40, card_data);
    write_bit(1, 42, card_data);

    // even parity bit
    write_bit((even_parity_sum % 2), 44, card_data);

    // data
    for(uint8_t i = 0; i < 24; i++) {
        write_bit((fc_cn >> (23 - i)) & 1, 46 + (i * 2), card_data);
    }

    // odd parity bit
    write_bit((odd_parity_sum % 2), 94, card_data);

    memcpy(encoded_data, &card_data, get_encoded_data_size());
}

void ProtocolIoProx::decode(
    const uint8_t* encoded_data,
    const uint8_t encoded_data_size,
    uint8_t* decoded_data,
    const uint8_t decoded_data_size) {
    furi_check(decoded_data_size >= get_decoded_data_size());
    furi_check(encoded_data_size >= get_encoded_data_size());

    //Index map
    //0           10          20          30          40          50          60
    //|           |           |           |           |           |           |
    //01234567 8 90123456 7 89012345 6 78901234 5 67890123 4 56789012 3 45678901 23
    //-----------------------------------------------------------------------------
    //00000000 0 11110000 1 facility 1 version* 1 code*one 1 code*two 1 ???????? 11

    //0        1        2        3        4        5        6        7
    //|        |        |        |        |        |        |        |
    //01234567 89ABCDEF 01234567 89ABCDEF 01234567 89ABCDEF 01234567 89ABCDEF
    //-----------------------------------------------------------------------
    //00000000 01111000 01FFFFFF FF1VVVVV VVV1CCCC CCCC1CCC CCCCC1?? ??????11
    //
    // F = facility code
    // V = version
    // C = code

    // Facility code
    decoded_data[0] = (encoded_data[2] << 2) | (encoded_data[3] >> 6);

    // Version code.
    decoded_data[1] = (encoded_data[3] << 3) | (encoded_data[4] >> 5);

    // Code bytes.
    decoded_data[2] = (encoded_data[4] << 4) | (encoded_data[5] >> 4);
    decoded_data[3] = (encoded_data[5] << 5) | (encoded_data[6] >> 3);
}

bool ProtocolIoProx::can_be_decoded(const uint8_t* encoded_data, const uint8_t encoded_data_size) {
    furi_check(encoded_data_size >= get_encoded_data_size());

    // packet preamble
    if(encoded_data[0] != 0b00000000) {
        return false;
    }
    if((encoded_data[1] >> 6) != 0b01) {
        return false;
    }

    //0        1        2        3        4        5        6        7
    //|        |        |        |        |        |        |        |
    //01234567 89ABCDEF 01234567 89ABCDEF 01234567 89ABCDEF 01234567 89ABCDEF
    //-----------------------------------------------------------------------
    //00000000 01111000 01FFFFFF FF1VVVVV VVV1CCCC CCCC1CCC CCCCC1XX XXXXXX11
    //
    // F = facility code
    // V = version
    // C = code
    // X = checksum

    // check for known ones
    if((encoded_data[2] & 0b01000000) == 0) {
        return false;
    }
    if((encoded_data[3] & 0b00100000) == 0) {
        return false;
    }
    if((encoded_data[4] & 0b00010000) == 0) {
        return false;
    }
    if((encoded_data[5] & 0b00001000) == 0) {
        return false;
    }
    if((encoded_data[6] & 0b00000100) == 0) {
        return false;
    }
    if((encoded_data[7] & 0b00000011) == 0) {
        return false;
    }

    //Checksum:
    //0        1        2         3         4         5         6         7
    //|        |        |         |         |         |         |         |
    //01234567 8 9ABCDEF0 1 23456789 A BCDEF012 3 456789AB C DEF01234 5 6789ABCD EF
    //00000000 0 11110000 1 11100000 1 00000001 1 00000011 1 10110110 1 01110101 11
    //preamble      F0         E0         01         03         B6         75
    // How to calc checksum,
    // http://www.proxmark.org/forum/viewtopic.php?id=364&p=6
    //   F0 + E0 + 01 + 03 + B6 = 28A
    //   28A & FF = 8A
    //   FF - 8A = 75
    // Checksum: 0x75
    uint8_t checksum = 0;
    uint8_t checkval;

    checksum += (encoded_data[1] << 1) | (encoded_data[2] >> 7); // F0
    checksum += (encoded_data[2] << 2) | (encoded_data[3] >> 6); // E0
    checksum += (encoded_data[3] << 3) | (encoded_data[4] >> 5); // 01
    checksum += (encoded_data[4] << 4) | (encoded_data[5] >> 4); // 03
    checksum += (encoded_data[5] << 5) | (encoded_data[6] >> 3); // B6

    checkval = (encoded_data[6] << 6) | (encoded_data[7] >> 2); // 75
    (void)checkval;

    /*
    if((0xFF - checksum) != checkval) {
        return false;
    }
    */

    // XXX
    if(encoded_data[5] == 0xff) {
        return false;
    }

    return true;
}
