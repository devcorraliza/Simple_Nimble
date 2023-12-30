#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "nvs_flash.h"
#include "SimpleNimble/btsn.h"

SizedPointer test1()
{
    printf("\ntest1 using arg\n");
    const char *dataReadBLE = "Hola Majo";
    return (SizedPointer){(void *)dataReadBLE, strlen(dataReadBLE)};
}
void test2(SizedPointer sptr)
{
    printf("\ntest2 %i\n", sptr.size);
    for (int i = 0; i < sptr.size; i++)
    {
        printf("%c", ((char *)sptr.ptr)[i]);
    }
    printf("\n");
}
extern "C"
{
    void app_main()
    {
        const int rc = nvs_flash_init();
        printf("nvs_flash_init rc=%i\n", rc);

        BTSN *btsn = new BTSN("Esp32_BLE");
        BTSNSvc *btsnSvc = btsn->addService("0f4be08b-893c-4097-a3c5-5e7cfcd27370");
        BTSNChr *btsnChr1 = btsnSvc->addCharacteristic("01867914-3fb9-cabd-3a4f-57ade32bd33f");
        btsnChr1->onReadRequest(test1);
        btsnChr1->onWriteRequest(test2);
        BTSNChr *btsnChr2 = btsnSvc->addCharacteristic("01867914-3fb9-cabd-3a4f-57ade32bd33e");
        btsnChr2->onReadRequest(test1);
        btsnChr2->onWriteRequest(test2);
        btsn->setPasskey(666);
        btsn->start();
    }
}
