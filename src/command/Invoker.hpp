#pragma once

#include <QVariant>
#include <QFutureSynchronizer>
#include <QtConcurrent>


namespace Xeth{


class NullCommandArguments
{};


template<class Notifier>
class Invoker
{

    public:

        Invoker(Notifier &);
        ~Invoker();

        template<class Command, class Arguments>
        QVariant invoke(Command &, const Arguments &);

        template<class Command>
        QVariant invoke(Command &);

        template<class Command>
        QVariant invokeAsync(Command &);

        template<class Command, class Arguments>
        QVariant invokeAsync(Command &, const Arguments &);

        void waitToComplete();

    private:
        Invoker(const Invoker &);

        template<class Command, class Arguments>
        QVariant execute(Command &, const Arguments &);

        template<class Command>
        QVariant execute(Command &, const NullCommandArguments &);

        template<class Command, class Request>
        QVariant executeAsync(Command &, const Request &, const QString &uid);

    protected:
        Notifier &_notifier;
        QFutureSynchronizer<QVariant> _futures;
};



}


#include "Invoker.ipp"
