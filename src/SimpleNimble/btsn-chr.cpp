#include "btsn-chr.h"

BTSNChr::BTSNChr(const char id[37])
{
    memcpy(this->id, id, 37);
    this->flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE;
    this->min_key_size = 16U;
    this->on_read_fn = nullptr;
};

void BTSNChr::onReadRequest(btsnChr_on_read_fn *fn)
{
    this->on_read_fn = fn;
};

void BTSNChr::onWriteRequest(btsnChr_on_write_fn *fn)
{
    this->on_write_fn = fn;
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
        .access_cb = &this->access_cb,
        .arg = this,
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
        this->flags |= BLE_GATT_CHR_F_WRITE_ENC;
    }
    else
    {
        this->flags &= ~BLE_GATT_CHR_F_WRITE_ENC;
    }
};

int BTSNChr::access_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR)
    {
        if (((BTSNChr *)arg)->on_read_fn != nullptr)
        {
            const SizedPointer sptr = ((BTSNChr *)arg)->on_read_fn();
            os_mbuf_append(ctxt->om, sptr.ptr, sptr.size);
        }
    }
    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR)
    {
        const SizedPointer sptr = {.ptr = ctxt->om->om_data, .size = OS_MBUF_PKTLEN(ctxt->om)};

        if (((BTSNChr *)arg)->on_write_fn != nullptr)
        {
            ((BTSNChr *)arg)->on_write_fn(sptr);
        }
    }
    // printf("\naccess_cb called %i %i %i %i %i\n", ctxt->op, BLE_GATT_ACCESS_OP_READ_CHR,
    //        BLE_GATT_ACCESS_OP_WRITE_CHR, BLE_GATT_ACCESS_OP_READ_DSC, BLE_GATT_ACCESS_OP_WRITE_DSC);
    return 0;
};