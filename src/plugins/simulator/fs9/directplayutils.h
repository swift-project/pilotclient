/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
