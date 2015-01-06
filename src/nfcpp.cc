#include <nfc/nfc.h>
#include <string.h>
#include <string>

#include "nfcpp.h"

namespace nfcpp
{

static const size_t MAX_DEVICES = 16;


class NFCDevice : INFCDevice
{
public:
    NFCDevice(nfc_context* context, nfc_connstring& str) : dev(0), ctx(context)
    {
        memcpy(conn_str, str, sizeof(str));
    }

    virtual ~NFCDevice()
    {
        if (dev)
            close();
    }

    bool is_open() const
    {
        return dev != NULL;
    }

    bool open()
    {
        dev = nfc_open(ctx, conn_str);
        return dev != NULL;
    }

    bool close()
    {
        if (!dev)
            return false;

        nfc_close(dev);
        dev = NULL;
        return true;
    }

    const std::string& name() const
    {
        return dev_name;
    }

private:
    nfc_device* dev;
    nfc_context* ctx;
    nfc_connstring conn_str;
    std::string dev_name;
};


struct NFCImpl
{
    nfc_context* ctx;
    size_t device_count;
    std::string libnfc_version;

    nfc_connstring device_names[MAX_DEVICES];
    NFCDevice* devices[MAX_DEVICES];


    NFCImpl() : ctx(0), device_count(0)
    {
        memset(device_names, 0, sizeof(device_names[0]) * MAX_DEVICES);
        memset(devices, 0, sizeof(devices[0]) * MAX_DEVICES);
    };
};



NFC::NFC()
{
    data = new NFCImpl;

    // initialize context
    nfc_init(&data->ctx);

    if (!data->ctx)
        throw "cannot initialize NFC";

    // get version
    data->libnfc_version.assign(nfc_version());

    // list devices
    data->device_count = nfc_list_devices(data->ctx, data->device_names, MAX_DEVICES);

    // ..and create objects for each
    for (size_t i = 0; i < data->device_count; i++)
    {
        data->devices[i] = new NFCDevice(data->ctx, data->device_names[i]);
    }

}


NFC::~NFC()
{
    if (data)
    {
        for (int i = 0; i < MAX_DEVICES; i++)
        {
            if (data->devices[i])
            {
                delete data->devices[i];
            }
        }

        if (data->ctx)
            nfc_exit(data->ctx);
    }
}

const std::string& NFC::get_libnfc_version() const
{
    return data->libnfc_version;
}

size_t NFC::get_device_count() const
{
    return data->device_count;
}

int NFC::get_device_name(int index, std::string &name) const
{
    if (index < 0 || index >= data->device_count)
        return -1;

    name.assign(data->device_names[index]);
    return 0;
}

INFCDevice* NFC::get_device(int index) const
{
    return reinterpret_cast<INFCDevice*>(data->devices[index]);
}

}