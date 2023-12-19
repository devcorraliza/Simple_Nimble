#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "esp_event.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "SimpleNimble/btsn.h"

static const char *TAG_BLE = "BLE"; // For loggin

char dataReadBLE[20] = "Data";
char dataWriteBLE[20];
static int gatt_svc_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

uint8_t ble_addr_type;
void ble_app_advertise(void);

void device_write(struct ble_gatt_access_ctxt *ctxt)
{
    char *incoming_data = (char *)ctxt->om->om_data;

    uint8_t om_len = OS_MBUF_PKTLEN(ctxt->om);
    memset(dataWriteBLE, 0, sizeof(dataWriteBLE));
    memcpy(dataWriteBLE, incoming_data, om_len);
    dataWriteBLE[om_len] = '\0';
    ESP_LOGI(TAG_BLE, "Mensaje entrante (primeros %d caracteres): %s\n", om_len, dataWriteBLE);
}
// os_mbuf_free(ctxt->om);

void device_read(struct ble_gatt_access_ctxt *ctxt, const char *data)
{
    os_mbuf_append(ctxt->om, data, strlen(data));
}

int gatt_svc_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{

    switch (ctxt->op)
    {
    case BLE_GATT_ACCESS_OP_READ_CHR:
        if (conn_handle != BLE_HS_CONN_HANDLE_NONE)
        {
            ESP_LOGI(TAG_BLE, "Lectura de característica; conn_handle=%d attr_handle=%d\n", conn_handle, attr_handle);
        }
        else
        {
            ESP_LOGI(TAG_BLE, "Lectura de característica por NimBLE stack; attr_handle=%d\n", attr_handle);
        }
        device_read(ctxt, dataReadBLE);
        return 0;
        break;

    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        if (conn_handle != BLE_HS_CONN_HANDLE_NONE)
        {
            ESP_LOGI(TAG_BLE, "Escritura de característica; conn_handle=%d attr_handle=%d", conn_handle, attr_handle);
        }
        else
        {
            ESP_LOGI(TAG_BLE, "Escritura de característica por NimBLE stack; attr_handle=%d", attr_handle);
        }
        device_write(ctxt);
        return 0;
        break;
    }

    return BLE_ATT_ERR_UNLIKELY;
}

/**
 * @brief Callback function to handle BLE GAP events.
 *
 * This function handles various BLE GAP events such as connection, disconnection, advertising completion, and subscription.
 *
 * @note Make sure to include the necessary header files and dependencies in your project.
 *
 * @param event Pointer to the BLE GAP event structure.
 * @param arg Argument provided during event registration.
 * @return int Returns 0 to indicate success.
 */
int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    printf("dcorraliza gap_event...");
    switch (event->type)
    {
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI("GAP", "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK!" : "FAILED!");
        if (event->connect.status != 0)
        {
            // start advertising again!
            ble_app_advertise();
        }
        break;
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI("GAP", "BLE GAP EVENT");
        ble_app_advertise();
        break;
    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI("GAP", "BLE GAP EVENT");
        ble_app_advertise();
        break;
    case BLE_GAP_EVENT_SUBSCRIBE:
        ESP_LOGI("GAP", "BLE GAP EVENT");
        break;
    default:
        break;
    }
    return 0;
}

/**
 * @brief Function to start advertising the BLE services.
 *
 * This function configures the advertisement fields and parameters, then starts advertising the BLE services.
 *
 * @note Make sure to include the necessary header files and dependencies in your project.
 *
 * @note The 'ble_svc_gap_device_name()' function should be defined in your project to retrieve the BLE device name.
 *
 * @param ble_addr_type The BLE address type to be used for advertising.
 */
void ble_app_advertise(void)
{
    printf("dcorraliza advertising...\n");
    struct ble_hs_adv_fields fields;
    memset(&fields, 0, sizeof(fields));
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_DISC_LTD;
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
    fields.name = (uint8_t *)ble_svc_gap_device_name();
    fields.name_len = strlen(ble_svc_gap_device_name());
    fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&fields);
    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
}

/**
 * @brief Callback function called on BLE synchronization event.
 *
 * This function is called when the BLE stack has synchronized with the host.
 * It configures the BLE address type and starts advertising the services.
 *
 * @note Make sure to include the necessary header files and dependencies in your project.
 *
 * @note The 'ble_app_advertise()' function should be defined in your project to start advertising the services.
 */
void ble_app_on_sync(void)
{
    printf("on_sync...");
    // ble_addr_t addr;
    // ble_hs_id_gen_rnd(1, &addr);
    // ble_hs_id_set_rnd(addr.val);
    ble_hs_id_infer_auto(0, &ble_addr_type); // determines automatic address.
    ble_app_advertise();                     // start advertising the services -->
}

extern "C"
{
    void app_main()
    {
        BTSN *btsn = new BTSN();
        nvs_flash_init();
        btsn->init("Esp32_BLE");
        BTSNSvc *btsnSvc = btsn->addService("0f4be08b-893c-4097-a3c5-5e7cfcd27370");
        BTSNChr *btsnChr1 = btsnSvc->addCharacteristic("01867914-3fb9-cabd-3a4f-57ade32bd33f");
        btsnChr1->setAccessCb(gatt_svc_access);
        btsnChr1->setReadable(false);
        BTSNChr *btsnChr2 = btsnSvc->addCharacteristic("01867914-3fb9-cabd-3a4f-57ade32bd33e");
        btsnChr2->setAccessCb(gatt_svc_access);
        btsnChr2->setWritable(false);
        btsn->setSync_cb(ble_app_on_sync);
        btsn->start();
    }
}
