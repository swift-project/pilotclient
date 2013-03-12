/***************************************************************************
 *   Copyright (C) 2013 by Roland Winklmeier                               *
 *   roland.m.winklmeier@googlemail.com                                    *
 *                                                                         *
 *   For license information see LICENSE in the root folder of the         *
 *   source code.                                                          *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Lesser General Public License for more details.                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/qglobal.h>

#include <blackmisc/debug.h>
#include "driver_fsx.h"

extern "C"
{
    Q_DECL_EXPORT ISimulator* BB_createISimulatorInstance ()
    {
        return new CDriverFSX;
    }

    Q_DECL_EXPORT quint32 BB_InterfaceVersionMajor ()
    {
        return ISimulator::InterfaceVersionMajor;
    }

    Q_DECL_EXPORT quint32 BB_InterfaceVersionMinor ()
    {
        return ISimulator::InterfaceVersionMinor;
    }
}

CDriverFSX::CDriverFSX()
{

}

void CDriverFSX::setLibraryContext(BlackMisc::IContext *context)
{
#ifdef Q_OS_WIN
    bAssert(!BlackMisc::IContext::isContextInitialised());
#endif

    m_libraryContext = new BlackMisc::CLibraryContext(*context);
}
