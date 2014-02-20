#ifndef BLACKMISC_CFILEDELETER_H
#define BLACKMISC_CFILEDELETER_H

#include <QObject>
#include <QDebug>

namespace BlackMisc
{

    /*!
     * \brief Utility class, deleting files when it is destroyed
     */
    class CFileDeleter
    {

    private:
        QStringList m_fileNames;

        //! \brief File deleter
        CFileDeleter() {}

        //! \brief add given file for deletion
        void addFile(const QString &file);

    public:

        //! \brief file deleter
        static CFileDeleter &fileDeleter() { static CFileDeleter f; return f;}

        //! \brief add a file (name)
        static void addFileForDeletion(const QString &file);

        //! \brief Destructor
        ~CFileDeleter();
    };
}

#endif // guard
