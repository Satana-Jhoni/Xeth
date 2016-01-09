#pragma once

#include <QVariant>
#include <QVariantMap>

#include "DataBase.hpp"


namespace Xeth{



class AddConfigCommand
{
    public:
        AddConfigCommand(DataBase &);

        QVariant operator()(const QVariantMap &);

    private:
        DataBase &_database;

};


}
