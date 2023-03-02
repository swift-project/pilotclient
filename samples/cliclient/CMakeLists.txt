add_executable(samples_cliclient
        main.cpp
        reader.cpp
        reader.h
        samplecliclient.h
        )
target_link_libraries(samples_cliclient core misc Qt::Core)
