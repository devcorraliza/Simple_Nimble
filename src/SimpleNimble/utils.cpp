#include "utils.h"

// Converts a hex char to decimal t_int8
u_int8_t hex2dec(const char str)
{
    return str < 58 ? str - 48 : str - 87;
}

// Converts a 4 char hex string to int
u_int8_t hex2int_8(const char str[2])
{
    int uuid = hex2dec(str[0]);
    uuid = uuid << 4;
    uuid += hex2dec(str[1]);
    return uuid;
}

ble_uuid128_t *ble_uuid_init_from_str(const char str[37])
{
    // Initially this array contains the positions of the uuid in the string
    u_int8_t uuid[16] = {34, 32, 30, 28, 26, 24, 21, 19, 16, 14, 11, 9, 6, 4, 2, 0};
    for (int i = 0; i < 16; i++)
    {
        uuid[i] = hex2int_8(&str[uuid[i]]);
    }
    ble_uuid128_t *ble_uuid = (ble_uuid128_t *)malloc(sizeof(ble_uuid128_t));
    ble_uuid_init_from_buf((ble_uuid_any_t *)ble_uuid, uuid, 16);
    return ble_uuid;
}
