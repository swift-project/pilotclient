// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_AFV_CRYPTO_CRYPTODTOMODE_H
#define BLACKCORE_AFV_CRYPTO_CRYPTODTOMODE_H

#include "msgpack.hpp"

namespace BlackCore::Afv::Crypto
{
    //! DTO mode
    enum class CryptoDtoMode
    {
        Undefined = 0, //!< undefined
        None = 1, //!< none
        AEAD_ChaCha20Poly1305 = 2 //!< AEAD
    };

} // ns

//! \private
MSGPACK_ADD_ENUM(BlackCore::Afv::Crypto::CryptoDtoMode);

#endif // guard
