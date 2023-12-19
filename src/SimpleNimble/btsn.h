#include <vector>
#include "btsn-svc.h"

class BTSN
{
public:
    void init(const char *device_name = "BLE");
    BTSNSvc *addService(const char id[37]);
    void setSync_cb(const ble_hs_sync_fn *fn);
    void start();

private:
    std::vector<BTSNSvc *> services;
    void configure_ble_max_power();
    ble_gatt_svc_def *getDefinition();
};
