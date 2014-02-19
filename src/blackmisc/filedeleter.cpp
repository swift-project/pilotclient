#include <QFile>
#include "filedeleter.h"

namespace BlackMisc
{
    CFileDeleter::~CFileDeleter()
    {
        foreach(const QString fn, this->m_fileNames)
        {
            QFile f(fn);
            f.remove();
        }
    }
}
