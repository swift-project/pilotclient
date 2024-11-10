# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(sound SHARED
        notificationplayer.h
        sampleprovider/sinusgenerator.h
        sampleprovider/samples.cpp
        sampleprovider/simplecompressoreffect.h
        sampleprovider/resourcesoundsampleprovider.cpp
        sampleprovider/resourcesoundsampleprovider.h
        sampleprovider/equalizersampleprovider.cpp
        sampleprovider/resourcesound.cpp
        sampleprovider/bufferedwaveprovider.h
        sampleprovider/samples.h
        sampleprovider/volumesampleprovider.h
        sampleprovider/pinknoisegenerator.h
        sampleprovider/mixingsampleprovider.h
        sampleprovider/sampleprovider.h
        sampleprovider/sawtoothgenerator.cpp
        sampleprovider/resourcesound.h
        sampleprovider/volumesampleprovider.cpp
        sampleprovider/sawtoothgenerator.h
        sampleprovider/pinknoisegenerator.cpp
        sampleprovider/bufferedwaveprovider.cpp
        sampleprovider/mixingsampleprovider.cpp
        sampleprovider/simplecompressoreffect.cpp
        sampleprovider/equalizersampleprovider.h
        sampleprovider/sinusgenerator.cpp
        tonepair.h
        selcalplayer.cpp
        dsp/SimpleGate.h
        dsp/SimpleGate.cpp
        dsp/biquadfilter.cpp
        dsp/SimpleHeader.h
        dsp/SimpleComp.h
        dsp/SimpleLimit.cpp
        dsp/SimpleLimitProcess.inl
        dsp/SimpleGain.h
        dsp/SimpleComp.cpp
        dsp/SimpleLimit.h
        dsp/biquadfilter.h
        dsp/SimpleEnvelope.h
        dsp/SimpleCompProcess.inl
        dsp/SimpleGateProcess.inl
        dsp/SimpleEnvelope.cpp
        selcalplayer.h
        wav/wavfile.h
        wav/wavfile.cpp
        threadedtonepairplayer.cpp
        notificationplayer.cpp
        tonepair.cpp
        swiftsoundexport.h
        audioutilities.h
        threadedtonepairplayer.h
        audioutilities.cpp
        codecs/opusencoder.cpp
        codecs/opusdecoder.cpp
        codecs/opusdecoder.h
        codecs/opusencoder.h
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
        externals_opus
)

install(TARGETS sound
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY share/sounds DESTINATION ${CMAKE_INSTALL_PREFIX}/share/)
