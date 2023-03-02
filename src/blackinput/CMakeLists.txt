add_library(input SHARED
        blackinputexport.h
        joystick.cpp
        joystick.h
        keyboard.cpp
        keyboard.h
)

if(WIN32)
    target_sources(input PRIVATE
        win/joystickwindows.cpp
        win/joystickwindows.h
        win/keyboardwindows.cpp
        win/keyboardwindows.h
    )
    target_compile_definitions(input PRIVATE DIRECTINPUT_VERSION=0x0800)
    target_link_libraries(input PUBLIC dxguid ole32 dinput8 User32)
elseif(APPLE)
    target_sources(input PRIVATE
        macos/joystickmacos.mm
        macos/joystickmacos.h
        macos/keyboardmacos.mm
        macos/keyboardmacos.h
        macos/macosinpututils.h
        macos/macosinpututils.mm
    )
    target_link_libraries(input PUBLIC "-framework CoreFoundation" "-framework ApplicationServices" "-framework Foundation" "-framework AppKit" "-framework IOKit")
else()
    target_sources(input PRIVATE
        linux/joysticklinux.cpp
        linux/joysticklinux.h
        linux/keyboardlinux.cpp
        linux/keyboardlinux.h
    )
endif()

target_include_directories(input PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(input
        PUBLIC
            Qt::Core
            misc
)

target_compile_definitions(input PRIVATE BUILD_BLACKINPUT_LIB)

install(TARGETS input
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
