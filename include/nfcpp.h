#ifndef __NFC_MIF_H
#define __NFC_MIF_H

#include <string>
#include <vector>

namespace nfcpp
{
    class NFCImpl;

    // A libnfc device
    class NFCDevice
    {
    public:
        virtual bool is_open() const = 0;
        virtual bool open() = 0;
        virtual bool close() = 0;
        virtual const std::string& name() const = 0;
    };

    enum TagType
	{
    	NFC_TAG_MIFARE_ULTRALIGHT,
		NFC_TAG_MIFARE_ULTRALIGHT_C,
		NFC_TAG_MIFARE_CLASSIC_1K,
		NFC_TAG_MIFARE_CLASSIC_4K,
		NFC_TAG_MIFARE_DESFIRE,
		NFC_TAG_UNKNOWN
	};


    class NFCTag
    {
    public:
    	virtual TagType type() = 0;
    	virtual std::string name() = 0;
    	virtual std::string uid() = 0;
    };


    class NFCTagReader
    {
    public:
        virtual std::vector<NFCTag*> get_tags() = 0;
    };


    // NFC library wrapper
    class NFC
    {
        public:
            NFC();
            virtual ~NFC();

            size_t get_device_count() const;
            int get_device_name(int index, std::string &name) const;
            NFCDevice* get_device(int index) const;
            NFCTagReader* get_tag_reader(int index);
            const std::string& get_libnfc_version() const;

        private:
            NFCImpl* data;
    };
};


#endif
