add_executable(samples_weatherdata
        main.cpp
        reader.cpp
        reader.h
        sampleweatherdata.h
        weatherdataprinter.cpp
        weatherdataprinter.h
        )
target_link_libraries(samples_weatherdata core misc Qt::Core)
