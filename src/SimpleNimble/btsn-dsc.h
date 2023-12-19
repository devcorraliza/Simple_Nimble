#include "utils.h"
#include "host/ble_hs.h"

class BTSNDsc
{
public:
    BTSNDsc(const char id[37]);
    ble_gatt_dsc_def *getDefinition();
    void setReadable(bool readable);
    void setWritable(bool writable);
    void setAccessCb(ble_gatt_access_fn *access_cb);

private:
    char id[37];
    ble_gatt_access_fn *access_cb;
    uint8_t att_flags;
    uint8_t min_key_size;
};