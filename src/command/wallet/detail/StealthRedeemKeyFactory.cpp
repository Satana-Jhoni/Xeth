#include "StealthRedeemKeyFactory.hpp"


namespace Xeth{


StealthRedeemKeyFactory::StealthRedeemKeyFactory(DataBase &database) :
    _database(database)
{}


EthereumKey StealthRedeemKeyFactory::create(const QJsonObject &payment, const std::string &password)
{
    return create(payment, password, password);
}

EthereumKey StealthRedeemKeyFactory::create(const QJsonObject &payment, const std::string &masterPassword, const std::string &keyPassword)
{
    return create(payment["stealth"].toString().toStdString(), payment["secret"].toString().toStdString(), masterPassword, keyPassword);
}

EthereumKey StealthRedeemKeyFactory::create(const std::string &stealth, const std::string &secret, const std::string &password)
{
    return create(stealth, secret, password, password);
}


EthereumKey StealthRedeemKeyFactory::create(const std::string &stealth, const std::string &secret, const std::string &masterPassword, const std::string &keyPassword)
{
    StealthKey stealthKey = _database.getStealthKeys().get(stealth.c_str());
    Ethereum::Stealth::SharedSecret secretData = Literal<Ethereum::Stealth::SharedSecret>(secret);
    Ethereum::Stealth::RedeemKeyFactory redeemFactory;
    CipherFactory cipherFactory;

    return redeemFactory.create(stealthKey.getSpendPrivateKeys()[0].unlock(masterPassword), secretData, cipherFactory.create(), keyPassword);
}

}
