#include <QFile>
#include "filedeleter.h"

namespace BlackMisc
{
    void CFileDeleter::addFile(const QString &file)
    {
        if (file.isEmpty()) return;
        if (!this->m_fileNames.contains(file)) this->m_fileNames.append(file);
    }

    void CFileDeleter::addFileForDeletion(const QString &file)
    {
        CFileDeleter::fileDeleter().addFile(file);
    }

    CFileDeleter::~CFileDeleter()
    {
        foreach(const QString fn, this->m_fileNames)
        {
            QFile f(fn);
            f.remove();
        }
    }
}
