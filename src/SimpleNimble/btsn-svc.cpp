#include "btsn-svc.h"

BTSNSvc::BTSNSvc(const char id[37])
{
    memcpy(this->id, id, 37);
    this->type = BLE_GATT_SVC_TYPE_PRIMARY;
};

ble_gatt_svc_def *BTSNSvc::getDefinition()
{
    ble_gatt_svc_def *service = (ble_gatt_svc_def *)malloc(sizeof(ble_gatt_svc_def));
    ble_uuid128_t *SERVICE_UUID = ble_uuid_init_from_str(this->id);

    ble_gatt_chr_def *characteristics = nullptr;

    const uint8_t num_characteristics = this->characteristics.size();

    if (num_characteristics > 0)
    {
        characteristics = (ble_gatt_chr_def *)malloc(sizeof(ble_gatt_chr_def) * (num_characteristics + 1));

        for (int i = 0; i < num_characteristics; i++)
        {
            ble_gatt_chr_def *characteristic = this->characteristics[i]->getDefinition();
            memcpy(characteristics + i, characteristic, sizeof(ble_gatt_chr_def));
            free(characteristic);
        }
        characteristics[num_characteristics] = {0};
    }

    service[0] = {
        .type = this->type,
        .uuid = &(*SERVICE_UUID).u,
        .includes = nullptr,
        .characteristics = characteristics,
    };
    return service;
};

BTSNChr *BTSNSvc::addCharacteristic(const char id[37])
{
    BTSNChr *characteristic = new BTSNChr(id);
    this->characteristics.push_back(characteristic);
    return characteristic;
};

void BTSNSvc::setSecondary()
{
    this->type = BLE_GATT_SVC_TYPE_SECONDARY;
};
