#include "btsn.h"
#include "esp_nimble_hci.h"
#include "esp_bt.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "esp_log.h"

static const char *TAG_BTSN = "BTSN"; // For loggin
uint32_t BTSN::passkey = 0;
BTSN::BTSN(const char *device_name)
{
    this->device_name = (char *)device_name;
}

BTSNSvc *BTSN::addService(const char id[37])
{
    BTSNSvc *service = new BTSNSvc(id);
    this->services.push_back(service);
    return service;
};

void BTSN::start()
{
    ble_gatt_svc_def *services = (ble_gatt_svc_def *)malloc(sizeof(ble_gatt_svc_def) * (this->services.size() + 1));
    for (int i = 0; i < this->services.size(); i++)
    {
        ble_gatt_svc_def *service = this->services[i]->getDefinition();
        memcpy(services + i, service, sizeof(ble_gatt_svc_def));
        free(service);
    };
    services[this->services.size()] = {0};

    esp_nimble_hci_init();
    nimble_port_init();
    ble_svc_gap_device_name_set(this->device_name);
    ble_svc_gap_init();
    ble_svc_gatt_init();
    ble_gatts_count_cfg(services);
    ble_gatts_add_svcs(services);
    if (passkey != 0)
    {
        printf("dcorraliza seting passkey\n");
        ble_hs_cfg.sm_bonding = 1;
        ble_hs_cfg.sm_mitm = 1;
        ble_hs_cfg.sm_sc = 1;
        ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_DISP_ONLY;
        ble_hs_cfg.sm_our_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC;
        ble_hs_cfg.sm_their_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC;
    }
    ble_hs_cfg.sync_cb = advertise;
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
        ESP_LOGI(TAG_BTSN, "Configured ESP_BLE_PWR_TYPE_CONN_HDL0 to maximum");
    }
    else
    {
        ESP_LOGE(TAG_BTSN, "Failed to configure ESP_BLE_PWR_TYPE_CONN_HDL0");
    }

    result = esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_CONN_HDL1, ESP_PWR_LVL_P7);
    if (result == ESP_OK)
    {
        ESP_LOGI(TAG_BTSN, "Configured ESP_BLE_PWR_TYPE_CONN_HDL1 to maximum");
    }
    else
    {
        ESP_LOGE(TAG_BTSN, "Failed to configure ESP_BLE_PWR_TYPE_CONN_HDL1");
    }

    // Repite este patrón para otros tipos de potencia

    // Finalmente, configura la potencia de publicidad
    result = esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P7);
    if (result == ESP_OK)
    {
        ESP_LOGI(TAG_BTSN, "Configured ESP_BLE_PWR_TYPE_ADV to maximum");
    }
    else
    {
        ESP_LOGE(TAG_BTSN, "Failed to configure ESP_BLE_PWR_TYPE_ADV");
    }

    // Finalmente, configura la potencia de publicidad
    result = esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_SCAN, ESP_PWR_LVL_P7);
    if (result == ESP_OK)
    {
        ESP_LOGI(TAG_BTSN, "Configured ESP_BLE_PWR_TYPE_SCAN to maximum");
    }
    else
    {
        ESP_LOGE(TAG_BTSN, "Failed to configure ESP_BLE_PWR_TYPE_SCAN");
    }

    // Finalmente, configura la potencia de publicidad
    result = esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P7);
    if (result == ESP_OK)
    {
        ESP_LOGI(TAG_BTSN, "Configured ESP_BLE_PWR_TYPE_DEFAULT to maximum");
    }
    else
    {
        ESP_LOGE(TAG_BTSN, "Failed to configure ESP_BLE_PWR_TYPE_DEFAULT");
    }
}

void BTSN::advertise(void)
{
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
    ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER, &adv_params, onEvent, NULL);
}
void print_addr(const void *addr)
{
    const uint8_t *u8p;

    u8p = (uint8_t *)addr;
    MODLOG_DFLT(INFO, "%02x:%02x:%02x:%02x:%02x:%02x",
                u8p[5], u8p[4], u8p[3], u8p[2], u8p[1], u8p[0]);
}
void bleprph_print_conn_desc(struct ble_gap_conn_desc *desc)
{
    MODLOG_DFLT(INFO, "handle=%d our_ota_addr_type=%d our_ota_addr=",
                desc->conn_handle, desc->our_ota_addr.type);
    print_addr(desc->our_ota_addr.val);
    MODLOG_DFLT(INFO, " our_id_addr_type=%d our_id_addr=",
                desc->our_id_addr.type);
    print_addr(desc->our_id_addr.val);
    MODLOG_DFLT(INFO, " peer_ota_addr_type=%d peer_ota_addr=",
                desc->peer_ota_addr.type);
    print_addr(desc->peer_ota_addr.val);
    MODLOG_DFLT(INFO, " peer_id_addr_type=%d peer_id_addr=",
                desc->peer_id_addr.type);
    print_addr(desc->peer_id_addr.val);
    MODLOG_DFLT(INFO, " conn_itvl=%d conn_latency=%d supervision_timeout=%d "
                      "encrypted=%d authenticated=%d bonded=%d\n",
                desc->conn_itvl, desc->conn_latency,
                desc->supervision_timeout,
                desc->sec_state.encrypted,
                desc->sec_state.authenticated,
                desc->sec_state.bonded);
}

int BTSN::onEvent(struct ble_gap_event *event, void *arg)
{
    switch (event->type)
    {
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI(TAG_BTSN, "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK!" : "FAILED!");
        if (event->connect.status != 0)
        {
            // start advertising again!
            advertise();
        }
        if (1 == 1) // passkey != 0)
        {
            ble_gap_security_initiate(event->connect.conn_handle);
        }
        break;
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG_BTSN, "BLE_GAP_EVENT_DISCONNECT");
        advertise();
        break;
    case BLE_GAP_EVENT_CONN_UPDATE:
        ESP_LOGI(TAG_BTSN, "BLE_GAP_EVENT_CONN_UPDATE");
        break;
    case BLE_GAP_EVENT_CONN_UPDATE_REQ:
        ESP_LOGI(TAG_BTSN, "BLE_GAP_EVENT_CONN_UPDATE_REQ");
        *event->conn_update_req.self_params = *event->conn_update_req.peer_params;
        break;
    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI(TAG_BTSN, "BLE_GAP_EVENT_ADV_COMPLETE");
        advertise();
        break;
    case BLE_GAP_EVENT_SUBSCRIBE:
        ESP_LOGI(TAG_BTSN, "BLE_GAP_EVENT_SUBSCRIBE");
        break;

    case BLE_GAP_EVENT_ENC_CHANGE:
        /* Encryption has been enabled or disabled for this connection. */
        struct ble_gap_conn_desc desc;
        int rc;
        ESP_LOGI(TAG_BTSN, "encryption change event; status=%d ",
                 event->enc_change.status);
        rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
        assert(rc == 0);
        bleprph_print_conn_desc(&desc);
        ESP_LOGI(TAG_BTSN, "\n");
        break;
    case BLE_GAP_EVENT_PASSKEY_ACTION:
        ESP_LOGI("GAP", "PASSKEY_ACTION_EVENT started");

        if (event->passkey.params.action == BLE_SM_IOACT_DISP)
        {
            static ble_sm_io pkey = {};
            pkey.action = event->passkey.params.action;
            pkey.passkey = passkey; // This is the passkey to be entered on peer
            ESP_LOGI("GAP", "Enter passkey %" PRIu32 "on the peer side", pkey.passkey);
            ble_sm_inject_io(event->passkey.conn_handle, &pkey);
        }
        break;

    default:
        ESP_LOGI(TAG_BTSN, "BLE_GAP_EVENT_UNKNOWN %i", event->type);
        break;
    }
    return 0;
}

void BTSN::setPasskey(uint32_t value)
{
    passkey = value;
}