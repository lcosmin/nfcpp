#ifndef __NFC_MIF_H
#define __NFC_MIF_H

#include <string>
#include <vector>

namespace nfcpp
{
	enum TagType
	{
		NFC_TAG_MIFARE_ULTRALIGHT,
		NFC_TAG_MIFARE_ULTRALIGHT_C,
		NFC_TAG_MIFARE_CLASSIC_1K,
		NFC_TAG_MIFARE_CLASSIC_4K,
		NFC_TAG_MIFARE_DESFIRE,
		NFC_TAG_UNKNOWN
	};


    // A libnfc device
    class NFCDevice
    {
    public:
        virtual bool is_open() const = 0;
        virtual bool open() = 0;
        virtual bool close() = 0;
        virtual const std::string& name() const = 0;
    };

    // A generic tag read with the NFC card reader
    class NFCTag
    {
		public:
			virtual TagType type() = 0;
			virtual std::string name() = 0;
			virtual std::string uid() = 0;
    };


    namespace mifare
	{
    	typedef unsigned char MiCDataBlock[16];
    	typedef unsigned char MiCKey[6];
    	enum MiCKeyType { A, B };

		class MifareClassicTag : NFCTag
		{
			public:
				virtual bool connect();
				virtual bool disconnect();
				virtual bool authenticate(int block, MiCKey key, enum MiCKeyType type);
				virtual bool read(int block, MiCDataBlock* data);
				virtual bool write(int block, MiCDataBlock data);
		};
    };



    // A device that reads cards... really need this abstraction ?
    class NFCTagReader
    {
    public:
        virtual std::vector<NFCTag*> get_tags() = 0;
    };


    // NFC library wrapper
    class NFCImpl;
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
