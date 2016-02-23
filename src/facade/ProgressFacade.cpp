#include "ProgressFacade.hpp"

namespace Xeth{


ProgressFacade::ProgressFacade(Synchronizer &synchronizer, Notifier &notifier) :
    _synchronizer(synchronizer)
{}


QVariant ProgressFacade::getScan() const
{
    return QVariant::fromValue(_synchronizer.getScanProgress());
}


QVariant ProgressFacade::getSync() const
{
    return QVariant::fromValue(_synchronizer.getChainProgress());
}



}
