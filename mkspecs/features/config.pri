## Public Area ##

BLACK_CONFIG    *= BlackCore
BLACK_CONFIG    *= BlackGui
BLACK_CONFIG    *= BlackSound
BLACK_CONFIG    *= BlackInput
BLACK_CONFIG    *= Samples
BLACK_CONFIG    *= Unittests
BLACK_CONFIG    *= SwiftData
BLACK_CONFIG    *= SwiftCore
BLACK_CONFIG    *= SwiftGui
BLACK_CONFIG    *= SwiftLauncher
BLACK_CONFIG    *= FS9
BLACK_CONFIG    *= FSX
BLACK_CONFIG    *= XPlane
BLACK_CONFIG    *= ProfileRelease
BLACK_CONFIG    *= AssertsInRelease
#BLACK_CONFIG    *= AllowNoisyWarnings
BLACK_CONFIG    *= PackageInstaller
#BLACK_CONFIG    *= Static
BLACK_CONFIG    *= Doxygen
#BLACK_CONFIG    *= SwiftBeta
#BLACK_CONFIG    *= SwiftShipped

isEmpty(BLACK_EOL): BLACK_EOL = "20180101"

## Private Area ##

# include vatsim client id and key
include(vatsim.pri)

# Header based compiler macros
!contains(BLACK_CONFIG, BlackSound) { DEFINE_WITH_BLACKSOUND = "//" }
!contains(BLACK_CONFIG, BlackInput) { DEFINE_WITH_BLACKINPUT = "//" }
!contains(BLACK_CONFIG, BlackCore) { DEFINE_WITH_BLACKCORE = "//" }
!contains(BLACK_CONFIG, BlackGui) { DEFINE_WITH_BLACKGUI = "//" }
!contains(BLACK_CONFIG, SwiftData) { DEFINE_WITH_SWIFTDATA = "//" }
!contains(BLACK_CONFIG, SwiftGui) { DEFINE_WITH_SWIFTGUI = "//" }
!contains(BLACK_CONFIG, SwiftCore) { DEFINE_WITH_SWIFTCORE = "//" }
!contains(BLACK_CONFIG, FSX) { DEFINE_WITH_FSX = "//" }
!contains(BLACK_CONFIG, FS9) { DEFINE_WITH_FS9 = "//" }
!contains(BLACK_CONFIG, XPlane) { DEFINE_WITH_XPLANE = "//" }
!contains(BLACK_CONFIG, SwiftBeta) { DEFINE_SWIFT_BETA = "//" }
!contains(BLACK_CONFIG, SwiftShipped) { DEFINE_SWIFT_SHIPPED = "//" }
!contains(BLACK_CONFIG, SwiftVatsimSupport) { DEFINE_SWIFT_VATSIM_SUPPORT = "//" }

# Global compiler Macros
contains(BLACK_CONFIG, Static) { DEFINES *= WITH_STATIC }
contains(BLACK_CONFIG, AssertsInRelease) { DEFINES *= QT_FORCE_ASSERTS }
