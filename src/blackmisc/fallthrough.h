/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_FALLTHROUGH_H
#define BLACKMISC_FALLTHROUGH_H

#include <QtGlobal>

//! \cond

// MSVC2017: [[fallthrough]] supported but only enabled with /std:c++17
// GCC7+: [[gnu::fallthrough]] for C++11/14, [[fallthrough]] for C++17
// XCode: Does not seem to work in current configuration

#if defined(Q_CC_GNU) && Q_CC_GNU > 700
    #if __has_cpp_attribute(fallthrough)
        #define BLACK_FALLTHROUGH [[gnu::fallthrough]]
    #endif
#else
    #define BLACK_FALLTHROUGH
#endif

//! \endcond

#endif
