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

#ifndef DRIVER_FSX_H
#define DRIVER_FSX_H

#include <blackmisc/context.h>
#include <simulator.h>

class CDriverFSX : public ISimulator
{
public:
    CDriverFSX();

    virtual void setLibraryContext(BlackLib::IContext *context);

    virtual int init() { return 0; }

protected:

    BlackMisc::CLibraryContext *m_libraryContext;
};

#endif // DRIVER_FSX_H
