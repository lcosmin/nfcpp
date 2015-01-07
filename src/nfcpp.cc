#include <nfc/nfc.h>
#include <freefare.h>
#include <string.h>
#include <string>
#include <vector>

#include "nfcpp.h"

using namespace std;


namespace nfcpp
{

static const size_t MAX_DEVICES = 16;


class NFCDeviceInternal : NFCDevice
{
public:
    NFCDeviceInternal(nfc_context* context, nfc_connstring& str) : dev(0), ctx(context)
    {
        memcpy(conn_str, str, sizeof(str));
    }

    virtual ~NFCDeviceInternal()
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

    nfc_device* device() const
    {
    	return dev;
    }

private:
    nfc_device* dev;
    nfc_context* ctx;
    nfc_connstring conn_str;
    std::string dev_name;
};


class NFCTagInternal : NFCTag
{
public:
	NFCTagInternal(MifareTag t) : tag(t)
	{
	}

	virtual ~NFCTagInternal()
	{
		if (tag)
		{
			freefare_free_tag(tag);
			tag = NULL;
		}
	}

	std::string name() {
		return std::string(freefare_get_tag_friendly_name(tag));
	}

	std::string uid() {
		return std::string(freefare_get_tag_uid(tag));
	}

	TagType type() {
		switch (freefare_get_tag_type(tag))
		{
		case ULTRALIGHT:
			return NFC_TAG_MIFARE_ULTRALIGHT;
		case ULTRALIGHT_C:
			return NFC_TAG_MIFARE_ULTRALIGHT_C;
		case CLASSIC_1K:
			return NFC_TAG_MIFARE_CLASSIC_1K;
		case CLASSIC_4K:
			return NFC_TAG_MIFARE_CLASSIC_4K;
		case DESFIRE:
			return NFC_TAG_MIFARE_DESFIRE;
		default:
			return NFC_TAG_UNKNOWN;
		}
	}

protected:
	MifareTag tag;
};


namespace mifare {

	class MifareClassicTagInternal : MifareClassicTag, NFCTagInternal
	{
		public:
			bool connect()
			{
				return mifare_classic_connect(tag) == 0;
			}

			bool disconnect()
			{
				return mifare_classic_disconnect(tag) == 0;
			}

			bool authenticate(int block, MifareClassicKey key, enum MifareClassicKeyType type)
			{

			}

			bool read(int block, MifareClassicDataBlock* data)
			{

			}

			bool write(int block, MifareClassicDataBlock data)
			{

			}
	};

}

class NFCTagReaderInternal : NFCTagReader
{
public:
	NFCTagReaderInternal(NFCDeviceInternal& d) : dev(d)
	{
		if (!dev.is_open())
			throw "device is not open";
	}

	~NFCTagReaderInternal()
	{
		clear_tags();
	}

	vector<NFCTag*> get_tags()
	{
		if (tags.size())
			clear_tags();

		// read tags
		MifareTag* mf_tags = freefare_get_tags(dev.device());

		if (!mf_tags)
			throw "cannot list tags";

		for (int i = 0; mf_tags[i]; i++)
		{
			tags.push_back(new NFCTagInternal(mf_tags[i]));
		}

		vector<NFCTag*> res;
		for (vector<NFCTagInternal*>::const_iterator it = tags.begin(); it != tags.end(); ++it)
			res.push_back(reinterpret_cast<NFCTag*>(*it));
		return res;
	}
private:

	void clear_tags()
	{
		// clear any listed tags
		for (vector<NFCTagInternal*>::iterator it = tags.begin(); it != tags.end(); ++it)
			delete (*it);
		tags.clear();
	};

	NFCDeviceInternal& dev;
	vector<NFCTagInternal*> tags;
};



struct NFCImpl
{
    nfc_context* ctx;
    size_t device_count;
    std::string libnfc_version;

    nfc_connstring device_names[MAX_DEVICES];
    NFCDeviceInternal * devices[MAX_DEVICES];


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
        data->devices[i] = new NFCDeviceInternal(data->ctx, data->device_names[i]);
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

NFCDevice* NFC::get_device(int index) const
{
    return reinterpret_cast<NFCDevice *>(data->devices[index]);
}

NFCTagReader* NFC::get_tag_reader(int index)
{
	NFCTagReaderInternal* reader = new NFCTagReaderInternal(*data->devices[index]);

	return reinterpret_cast<NFCTagReader*>(reader);
}


}
