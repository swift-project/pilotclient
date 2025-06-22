# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(sound SHARED
        audioutilities.cpp
        audioutilities.h
        codecs/opusdecoder.cpp
        codecs/opusdecoder.h
        codecs/opusencoder.cpp
        codecs/opusencoder.h
        dsp/SimpleComp.cpp
        dsp/SimpleComp.h
        dsp/SimpleCompProcess.inl
        dsp/SimpleEnvelope.cpp
        dsp/SimpleEnvelope.h
        dsp/SimpleGain.h
        dsp/SimpleGate.cpp
        dsp/SimpleGate.h
        dsp/SimpleGateProcess.inl
        dsp/SimpleHeader.h
        dsp/SimpleLimit.cpp
        dsp/SimpleLimit.h
        dsp/SimpleLimitProcess.inl
        dsp/biquadfilter.cpp
        dsp/biquadfilter.h
        notificationplayer.cpp
        notificationplayer.h
        sampleprovider/bufferedwaveprovider.cpp
        sampleprovider/bufferedwaveprovider.h
        sampleprovider/equalizersampleprovider.cpp
        sampleprovider/equalizersampleprovider.h
        sampleprovider/mixingsampleprovider.cpp
        sampleprovider/mixingsampleprovider.h
        sampleprovider/pinknoisegenerator.cpp
        sampleprovider/pinknoisegenerator.h
        sampleprovider/resourcesound.cpp
        sampleprovider/resourcesound.h
        sampleprovider/resourcesoundsampleprovider.cpp
        sampleprovider/resourcesoundsampleprovider.h
        sampleprovider/sampleprovider.h
        sampleprovider/samples.cpp
        sampleprovider/samples.h
        sampleprovider/sawtoothgenerator.cpp
        sampleprovider/sawtoothgenerator.h
        sampleprovider/simplecompressoreffect.cpp
        sampleprovider/simplecompressoreffect.h
        sampleprovider/sinusgenerator.cpp
        sampleprovider/sinusgenerator.h
        sampleprovider/volumesampleprovider.cpp
        sampleprovider/volumesampleprovider.h
        selcalplayer.cpp
        selcalplayer.h
        swiftsoundexport.h
        threadedtonepairplayer.cpp
        threadedtonepairplayer.h
        tonepair.cpp
        tonepair.h
        wav/wavfile.cpp
        wav/wavfile.h
        )

# Copy resources
add_custom_command(TARGET sound
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different share ${PROJECT_BINARY_DIR}/out/share
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(sound PRIVATE BUILD_SWIFT_SOUND_LIB)

target_include_directories(sound PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(sound
        PUBLIC
        Qt::Core
        config
        Qt::Multimedia
        misc
        Opus::opus
)

install(TARGETS sound
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY share/sounds DESTINATION ${CMAKE_INSTALL_PREFIX}/share/)
