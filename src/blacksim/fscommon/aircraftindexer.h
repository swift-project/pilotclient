#ifndef BLACKSIM_FSCOMMON_AIRCRAFTINDEXER_H
#define BLACKSIM_FSCOMMON_AIRCRAFTINDEXER_H

#include "aircraftcfgentrieslist.h"
#include <QObject>
#include <QtConcurrent/QtConcurrent>

namespace BlackSim
{
    namespace FsCommon
    {
        /*!
         * \brief Indexer for all modelsThe CAircraftIndexer class
         */
        class CAircraftIndexer : QObject
        {
            Q_OBJECT

        private:
            //! Constructor
            CAircraftIndexer(QObject *parent = nullptr);

            CAircraftCfgEntriesList m_entries; //!< all entries

        signals:
            //! Number of entries read
            void entriesRead(int number);

        public:
            //! Single entity indexer
            static CAircraftIndexer &indexer()
            {
                static CAircraftIndexer indexer;
                return indexer;
            }

            //! Read for directory or re-read
            static int read(const QString directory = "");

            //! Read in background
            static QFuture<int> readInBackground(const QString directory = "");

        };
    }
}
#endif // guard
