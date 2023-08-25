// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_FS9_DIRECTPLAYUTILS_H
#define BLACKSIMPLUGIN_FS9_DIRECTPLAYUTILS_H

namespace BlackSimPlugin::Fs9
{
    //! Safely release a COM allocated object
    template <class T>
    void SafeRelease(T *&pT)
    {
        if (pT) { pT->Release(); }
        pT = nullptr;
    }

    //! Safely delete an allocated pointer
    template <class T>
    void SafeDelete(T *&pT)
    {
        delete pT;
        pT = nullptr;
    }

    //! Safely delete an allocated array
    template <class T>
    void SafeDeleteArray(T *&pT)
    {
        delete[] pT;
        pT = nullptr;
    }
}

#endif
