#include "btsn-chr.h"

BTSNChr::BTSNChr(const char id[37])
{
    memcpy(this->id, id, 37);
    this->flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE;
    this->min_key_size = 16U;
    this->access_cb = nullptr;
};

void BTSNChr::setAccessCb(ble_gatt_access_fn *access_cb)
{
    this->access_cb = access_cb;
};

ble_gatt_chr_def *BTSNChr::getDefinition()
{
    ble_gatt_chr_def *characteristic = (ble_gatt_chr_def *)malloc(sizeof(ble_gatt_chr_def));
    ble_uuid128_t *CHARACTERISTIC_UUID = ble_uuid_init_from_str(this->id);

    ble_gatt_dsc_def *descriptors = nullptr;
    const uint8_t num_descriptors = this->descriptors.size();
    if (num_descriptors > 0)
    {
        descriptors = (ble_gatt_dsc_def *)malloc(sizeof(ble_gatt_dsc_def) * (num_descriptors + 1));
        for (int i = 0; i < num_descriptors; i++)
        {
            ble_gatt_dsc_def *descriptor = this->descriptors[i]->getDefinition();
            memcpy(descriptors + i, characteristic, sizeof(ble_gatt_dsc_def));
            free(descriptor);
        }
        descriptors[num_descriptors] = {0};
    }

    characteristic[0] = {
        .uuid = &(*CHARACTERISTIC_UUID).u,
        .access_cb = this->access_cb,
        .arg = nullptr,
        .descriptors = descriptors,
        .flags = this->flags,
        .min_key_size = this->min_key_size,
        .val_handle = nullptr,
    };
    return characteristic;
}

void BTSNChr::setReadable(bool readable)
{
    if (readable)
    {
        this->flags |= BLE_GATT_CHR_F_READ;
    }
    else
    {
        this->flags &= ~BLE_GATT_CHR_F_READ;
    }
};

void BTSNChr::setWritable(bool writable)
{
    if (writable)
    {
        this->flags |= BLE_GATT_CHR_F_WRITE;
    }
    else
    {
        this->flags &= ~BLE_GATT_CHR_F_WRITE;
    }
};