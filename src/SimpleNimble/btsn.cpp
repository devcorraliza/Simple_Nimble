#include "btsn.h"
#include "esp_nimble_hci.h"
#include "esp_bt.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "esp_log.h"

void BTSN::init(const char *device_name)
{
    esp_nimble_hci_init();
    nimble_port_init();
    ESP_ERROR_CHECK(ble_svc_gap_device_name_set(device_name));
    ble_svc_gap_init();
    ble_svc_gatt_init();
}

BTSNSvc *BTSN::addService(const char id[37])
{
    BTSNSvc *service = new BTSNSvc(id);
    this->services.push_back(service);
    return service;
};

void BTSN::setSync_cb(const ble_hs_sync_fn *fn)
{
    ble_hs_cfg.sync_cb = fn;
}

void BTSN::start()
{
    ble_gatt_svc_def *services = (ble_gatt_svc_def *)malloc(sizeof(ble_gatt_svc_def) * (this->services.size() + 1));
    for (int i = 0; i < this->services.size(); i++)
    {
        ble_gatt_svc_def *service = this->services[i]->getDefinition();
        memcpy(services + i, service, sizeof(ble_gatt_svc_def));
        free(service);
    };
    services[this->services.size()] = {.type = 0};
    ble_gatts_count_cfg(services);
    ble_gatts_add_svcs(services);
    nimble_port_run();
    this->configure_ble_max_power();
}

// Función para configurar BLE a máxima potencia
void BTSN::configure_ble_max_power()
{
    // Configuración de potencia de transmisión para cada tipo
    esp_err_t result;

    result = esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_CONN_HDL0, ESP_PWR_LVL_P7);
    if (result == ESP_OK)
    {
        ESP_LOGI("power", "Configured ESP_BLE_PWR_TYPE_CONN_HDL0 to maximum");
    }
    else
    {
        ESP_LOGE("power", "Failed to configure ESP_BLE_PWR_TYPE_CONN_HDL0");
    }

    result = esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_CONN_HDL1, ESP_PWR_LVL_P7);
    if (result == ESP_OK)
    {
        ESP_LOGI("power", "Configured ESP_BLE_PWR_TYPE_CONN_HDL1 to maximum");
    }
    else
    {
        ESP_LOGE("power", "Failed to configure ESP_BLE_PWR_TYPE_CONN_HDL1");
    }

    // Repite este patrón para otros tipos de potencia

    // Finalmente, configura la potencia de publicidad
    result = esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P7);
    if (result == ESP_OK)
    {
        ESP_LOGI("power", "Configured ESP_BLE_PWR_TYPE_ADV to maximum");
    }
    else
    {
        ESP_LOGE("power", "Failed to configure ESP_BLE_PWR_TYPE_ADV");
    }

    // Finalmente, configura la potencia de publicidad
    result = esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_SCAN, ESP_PWR_LVL_P7);
    if (result == ESP_OK)
    {
        ESP_LOGI("power", "Configured ESP_BLE_PWR_TYPE_SCAN to maximum");
    }
    else
    {
        ESP_LOGE("power", "Failed to configure ESP_BLE_PWR_TYPE_SCAN");
    }

    // Finalmente, configura la potencia de publicidad
    result = esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P7);
    if (result == ESP_OK)
    {
        ESP_LOGI("power", "Configured ESP_BLE_PWR_TYPE_DEFAULT to maximum");
    }
    else
    {
        ESP_LOGE("power", "Failed to configure ESP_BLE_PWR_TYPE_DEFAULT");
    }
}