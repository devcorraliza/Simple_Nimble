#include "btsn-dsc.h"

BTSNDsc::BTSNDsc(const char id[37])
{
    memcpy(this->id, id, 37);
    this->att_flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE;
    this->min_key_size = 16U;
};

void BTSNDsc::setAccessCb(ble_gatt_access_fn *access_cb)
{
    this->access_cb = access_cb;
};

ble_gatt_dsc_def *BTSNDsc::getDefinition()
{
    ble_gatt_dsc_def *descriptor = (ble_gatt_dsc_def *)malloc(sizeof(ble_gatt_dsc_def));
    ble_uuid128_t *DESCRIPTOR_UUID = ble_uuid_init_from_str(this->id);

    descriptor[0] = {
        .uuid = &(*DESCRIPTOR_UUID).u,
        .att_flags = this->att_flags,
        .min_key_size = this->min_key_size,
        .access_cb = this->access_cb,
        .arg = nullptr,
    };
    return descriptor;
}

void BTSNDsc::setReadable(bool readable)
{
    if (readable)
    {
        this->att_flags |= BLE_GATT_CHR_F_READ;
    }
    else
    {
        this->att_flags &= ~BLE_GATT_CHR_F_READ;
    }
};

void BTSNDsc::setWritable(bool writable)
{
    if (writable)
    {
        this->att_flags |= BLE_GATT_CHR_F_WRITE;
    }
    else
    {
        this->att_flags &= ~BLE_GATT_CHR_F_WRITE;
    }
};