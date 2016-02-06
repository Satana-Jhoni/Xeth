#pragma once


#include "database/DataBaseDirectory.hpp"
#include "database/TransactionStore.hpp"
#include "database/ScanIndexStore.hpp"
#include "database/AddressBookStore.hpp"
#include "database/ConfigStore.hpp"
#include "database/StealthPaymentStore.hpp"
#include "database/StealthKeyStore.hpp"
#include "database/EthereumKeyStore.hpp"

#include "EthereumKeyStorePath.hpp"

namespace Xeth{


class DataBase
{
    public:
        DataBase(const Settings &);
//        DataBase(const char *path);

        TransactionStore & getTransactions();
        ScanIndexStore & getScanIndex();
        AddressBookStore & getAddressBook();
        ConfigStore & getConfig();
        StealthPaymentStore & getStealthPayments();
        StealthKeyStore & getStealthKeys();
        EthereumKeyStore & getEthereumKeys();

        const TransactionStore & getTransactions() const;
        const ScanIndexStore & getScanIndex() const;
        const AddressBookStore & getAddressBook() const;
        const ConfigStore & getConfig() const;
        const StealthPaymentStore & getStealthPayments() const;
        const StealthKeyStore & getStealthKeys() const;
        const EthereumKeyStore & getEthereumKeys() const;

    private:
        DataBaseDirectory _directory;
        EthereumKeyStorePath _ethereumKeysPath;
        TransactionStore _transactions;
        ScanIndexStore _scanIndex;
        AddressBookStore _addressbook;
        ConfigStore _config;
        StealthPaymentStore _stealthPayments;
        StealthKeyStore _stealthKeys;
        EthereumKeyStore _ethereumKeys;

};



}