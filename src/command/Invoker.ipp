
namespace Xeth{


template<class Notifier>
Invoker<Notifier>::Invoker(Notifier &notifier) :
    _notifier(notifier)
{}


template<class Notifier>
template<class Command, class Request>
QVariant Invoker<Notifier>::invoke(Command &command, const Request &request)
{
    try
    {
        return execute(command, request);
    }
    catch(std::exception &e)
    {
        _notifier.emitError(e.what());
    }
    catch(...)
    {}

    return QVariant::fromValue(false);
}


template<class Notifier>
template<class Command>
QVariant Invoker<Notifier>::invoke(Command &command)
{
    return invoke(command, NullCommandArguments());
}


template<class Notifier>
template<class Command, class Request>
QVariant Invoker<Notifier>::execute(Command &command, const Request &request)
{
    return command(request);
}



template<class Notifier>
template<class Command>
QVariant Invoker<Notifier>::execute(Command &command, const NullCommandArguments &)
{
    return command();
}


template<class Notifier>
template<class Command, class Request>
QVariant Invoker<Notifier>::executeAsync(Command &command, const Request &request, const QString &uid)
{
    QVariant result = invoke(command, request);
    _notifier.emitData("command", uid, "result", result);
    return result;
}

template<class Notifier>
template<class Command, class Request>
QVariant Invoker<Notifier>::invokeAsync(Command &command, const Request &request)
{
    QString uid = QUuid::createUuid().toString();
    _futures.addFuture(QtConcurrent::run(this, &Invoker<Notifier>::executeAsync<Command, Request>, command, request, uid));
    return uid;
}


template<class Notifier>
template<class Command>
QVariant Invoker<Notifier>::invokeAsync(Command &command)
{
    return invokeAsync(command, NullCommandArguments());
}


template<class Notifier>
void Invoker<Notifier>::waitToComplete()
{
    _futures.waitForFinished();
}



template<class Notifier>
Invoker<Notifier>::~Invoker()
{
    waitToComplete();
}


template<class Notifier>
Invoker<Notifier>::Invoker(const Invoker &)
{}



}
