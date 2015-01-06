#ifndef __NFC_MIF_H
#define __NFC_MIF_H

#include <string>

namespace nfcpp
{
    class NFCImpl;

    // A libnfc device
    class INFCDevice
    {
    public:
        virtual bool is_open() const = 0;
        virtual bool open() = 0;
        virtual bool close() = 0;
        virtual const std::string& name() const = 0;
    };

    // NFC library wrapper
    class NFC
    {
        public:
            NFC();
            virtual ~NFC();

            size_t get_device_count() const;
            int get_device_name(int index, std::string &name) const;
            INFCDevice* get_device(int index) const;
            const std::string& get_libnfc_version() const;

        private:
            NFCImpl* data;
    };

};


#endif
