#include <vector>
#include "host/ble_hs.h"
#include "btsn-chr.h"

class BTSNSvc
{
public:
    BTSNSvc(const char id[37]);
    BTSNChr *addCharacteristic(const char id[37]);
    void setSecondary();
    ble_gatt_svc_def *getDefinition();

private:
    uint8_t type;
    char id[37];

    std::vector<BTSNChr *> characteristics;
};
