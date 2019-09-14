#ifndef CRYPTODTOMODE_H
#define CRYPTODTOMODE_H

#include "msgpack.hpp"

enum class CryptoDtoMode
{
    Undefined = 0,
    None = 1,
    AEAD_ChaCha20Poly1305 = 2
};

MSGPACK_ADD_ENUM(CryptoDtoMode);

#endif // CRYPTODTOMODE_H
