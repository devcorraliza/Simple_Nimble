#include <vector>
#include "utils.h"
#include "host/ble_hs.h"
#include "btsn-dsc.h"

class BTSNChr
{
public:
    BTSNChr(const char id[37]);
    ble_gatt_chr_def *getDefinition();
    void setReadable(bool readable);
    void setWritable(bool writable);
    void setAccessCb(ble_gatt_access_fn *access_cb);

private:
    char id[37];
    ble_gatt_access_fn *access_cb;
    ble_gatt_chr_flags flags;
    uint8_t min_key_size;
    std::vector<BTSNDsc *> descriptors;
};