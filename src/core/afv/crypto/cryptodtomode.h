// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_AFV_CRYPTO_CRYPTODTOMODE_H
#define SWIFT_CORE_AFV_CRYPTO_CRYPTODTOMODE_H

#include "msgpack.hpp"

namespace swift::core::afv::crypto
{
    //! DTO mode
    enum class CryptoDtoMode
    {
        Undefined = 0, //!< undefined
        None = 1, //!< none
        AEAD_ChaCha20Poly1305 = 2 //!< AEAD
    };

} // namespace swift::core::afv::crypto

//! \private
MSGPACK_ADD_ENUM(swift::core::afv::crypto::CryptoDtoMode);

#endif // SWIFT_CORE_AFV_CRYPTO_CRYPTODTOMODE_H
