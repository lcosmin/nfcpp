#include <iostream>
#include <vector>

#include "nfcpp.h"

using namespace std;

int main()
{
    nfcpp::NFC a;

    size_t count = a.get_device_count();

    for (size_t i = 0 ; i < count ; i++)
    {
        string s;
        a.get_device_name(i, s);
        cout << "device: " << s << endl;
    }

    nfcpp::NFCDevice * d = a.get_device(0);

    if (!d)
    {
        cout << "no NFC device found!" << endl;
        return 1;
    }

    cout << "open device (" << d->name() << "): " << d->open() << endl;

    nfcpp::NFCTagReader* reader = a.get_tag_reader(0);

    vector<nfcpp::NFCTag*> tags = reader->get_tags();
    vector<nfcpp::NFCTag*>::iterator it = tags.begin();

    for (; it != tags.end(); ++it)
    {
    	nfcpp::NFCTag* tag = *it;
    	cout << "tag name: " << tag->name() << " uid: " << tag->uid() <<  endl;
    }

    delete reader;

    cout << "close device (" << d->name() << "): " << d->close() << endl;

    return 0;
}
