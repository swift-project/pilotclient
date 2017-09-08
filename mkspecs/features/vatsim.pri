# This is in a separate file to allow easy patching.

# VATSIM client id as assigned by VATSIM Network team. The one below is a test key.
# It is working only with VATGER test server.
isEmpty(VATSIM_CLIENT_ID):VATSIM_CLIENT_ID = "0xb9ba"

# VATSIM private key, which belongs to the id above.
isEmpty(VATSIM_CLIENT_PRIVATE_KEY):VATSIM_CLIENT_PRIVATE_KEY = "727d1efd5cb9f8d2c28372469d922bb4"

# Comment the section below to build a version without VATSIM features enabled
BLACK_CONFIG *= SwiftVatsimSupport
