
namespace Xeth{

template<class Criterion, class Arg1>
void ScanCriteria::addCriterion(size_t minBlock, const Arg1 &arg1)
{
    addCriterion(minBlock, new Criterion(arg1));
}

template<class Criterion, class Arg1, class Arg2>
void ScanCriteria::addCriterion(size_t minBlock, const Arg1 &arg1, const Arg2 &arg2)
{
    addCriterion(minBlock, new Criterion(arg1, arg2));
}

template<class Criterion, class Arg1, class Arg2, class Arg3>
void ScanCriteria::addCriterion(size_t minBlock, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
{
   addCriterion(minBlock, new Criterion(arg1, arg2, arg3));
}

template<class BlockChain, class Progress>
size_t ScanCriteria::process(BlockChain &blockchain, QJsonArray &result, Progress &progress)
{
    if(!_criteria.size())
    {
        return 0;
    }

    size_t height = blockchain.getHeight();
    size_t minBlock = _criteria.begin()->first;

    if(height < minBlock)
    {
        return height;
    }

    std::map<size_t, Container::iterator> mappedCriteria;

    size_t minIndex = minBlock;

    //3 10 10 11 12 13 14

    for(Container::iterator it=_criteria.begin(), end=_criteria.end(); it!=end; ++it)
    {
        if(minIndex < it->first)
        {
            mappedCriteria.insert(std::make_pair(minIndex, it));
            minIndex = it->first;
        }
    }


    mappedCriteria.insert(std::make_pair(height, _criteria.end()));

    progress.setRange(minBlock, height);

    size_t blockIndex = minIndex;
    BigInt blockReward("5000000000000000000");

    try
    {

        for(std::map<size_t, Container::iterator>::iterator it=mappedCriteria.begin(), end=mappedCriteria.end(); it!=end; ++it)
        {

            for(; blockIndex < it->first; blockIndex++)
            {
                size_t prevSize = result.size();
                Block block = blockchain.getBlock(blockIndex);

                std::string miner = block.getMiner();
                std::string blockHash = block.getHash();
                time_t blockTime = block.getTimestamp();

                for(Container::iterator cIt=_criteria.begin(),cEnd=it->second; cIt!=cEnd; ++cIt)
                {
                    cIt->second->processHeader(blockIndex, blockHash, miner, blockReward, blockTime, result);
                }

                TransactionCollection transactions = block.getTransactions();

                for(TransactionCollection::Iterator tIt=transactions.begin(), tEnd=transactions.end(); tIt!=tEnd; ++tIt)
                {
                    std::string sender = tIt->getSender();
                    std::string receiver = tIt->getReceiver();
                    std::string txid = tIt->getHash();
                    BigInt amount = tIt->getAmount();
                    std::string data = tIt->getInput();

                    for(Container::iterator cIt=_criteria.begin(),cEnd=it->second; cIt!=cEnd; ++cIt)
                    {
                        cIt->second->processTransaction(txid, sender, receiver, amount, data, blockTime, result);
                    }
                }

                progress.next();
                size_t resultSize = result.size();
                if(prevSize != resultSize)
                {
                    size_t numItems = resultSize - prevSize;
                    QJsonArray::const_iterator resultEnd = result.constEnd();
                    emit Data(blockIndex, resultEnd- numItems, resultEnd);
                }

                boost::this_thread::interruption_point();
            }

        }
    }
    catch(boost::thread_interrupted &)
    {}

    for(Container::iterator it=_criteria.begin(), end=_criteria.end(); it!=end; ++it)
    {
        it->first = blockIndex;
    }

    return blockIndex;

}



}