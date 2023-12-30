#include <vector>
#include "utils.h"
#include "host/ble_hs.h"
#include "btsn-dsc.h"

struct SizedPointer
{
    void *ptr;
    size_t size;
};

typedef SizedPointer btsnChr_on_read_fn();
typedef void btsnChr_on_write_fn(SizedPointer data);
class BTSNChr
{
public:
    BTSNChr(const char id[37]);
    ble_gatt_chr_def *getDefinition();
    void setReadable(bool readable);
    void setWritable(bool writable);
    void onReadRequest(btsnChr_on_read_fn *fn);
    void onWriteRequest(btsnChr_on_write_fn *fn);

private:
    char id[37];
    ble_gatt_chr_flags flags;
    uint8_t min_key_size;
    std::vector<BTSNDsc *> descriptors;
    btsnChr_on_read_fn *on_read_fn;
    btsnChr_on_write_fn *on_write_fn;
    static int access_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);
};