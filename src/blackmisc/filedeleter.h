#ifndef BLACKMISC_CFILEDELETER_H
#define BLACKMISC_CFILEDELETER_H

#include <QObject>
#include <QDebug>

namespace BlackMisc
{

    /*!
     * \brief Utility class, deleting files when it is destroyed
     */
    class CFileDeleter : public QObject
    {
        Q_OBJECT

    private:
        QStringList m_fileNames;

    public:
        /*!
         * \brief File deleter
         * \param parent
         */
        explicit CFileDeleter(QObject *parent = nullptr) : QObject(parent) {}

        //! \brief add given file for deletion
        void addFileForDeletion(const QString &file)
        {
            if (!this->m_fileNames.contains(file)) this->m_fileNames.append(file);
        }

        //! \brief Destructor
        ~CFileDeleter();
    };
}

#endif // guard
