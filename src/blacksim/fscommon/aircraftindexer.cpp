#include "aircraftindexer.h"

namespace BlackSim
{
    namespace FsCommon
    {
        CAircraftIndexer::CAircraftIndexer(QObject *parent) : QObject(parent) {}

        int CAircraftIndexer::read(const QString directory)
        {
            if (!directory.isEmpty()) indexer().m_entries.changeDirectory(directory);
            int n = indexer().m_entries.read();
            emit indexer().entriesRead(n);
            return n;
        }

        QFuture<int> CAircraftIndexer::readInBackground(const QString directory)
        {
            QFuture<int> f = QtConcurrent::run(CAircraftIndexer::read, directory);
            return f;
        }
    }
}
