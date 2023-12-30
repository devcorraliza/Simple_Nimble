#include <vector>
#include "btsn-svc.h"

class BTSN
{
public:
    BTSN(const char *device_name);
    BTSNSvc *addService(const char id[37]);
    void setPasskey(uint32_t passkey);
    void start();

private:
    char *device_name;
    static uint32_t passkey;

    std::vector<BTSNSvc *> services;
    void configure_ble_max_power();
    ble_gatt_svc_def *getDefinition();
    static void advertise();
    static int onEvent(struct ble_gap_event *event, void *arg);
};
