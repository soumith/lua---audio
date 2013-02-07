CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
CMAKE_POLICY(VERSION 2.6)
FIND_PACKAGE(Torch REQUIRED)
FIND_PATH(SOX_INCLUDE_DIR sox.h)
FIND_LIBRARY(SOX_LIBRARIES sox)
message ("SOX_INCLUDE_DIR: ${SOX_INCLUDE_DIR}")
message ("SOX_LIBRARIES: ${SOX_LIBRARIES}")

FIND_PATH(FFTW_INCLUDE_DIR fftw3.h)
FIND_LIBRARY(FFTW_LIBRARIES fftw3)
message ("FFTW_INCLUDE_DIR: ${FFTW_INCLUDE_DIR}")
message ("FFTW_LIBRARIES: ${FFTW_LIBRARIES}")


#if (SOX_FOUND)
    SET(src sox.c)
    include_directories (${SOX_INCLUDE_DIR})
    ADD_TORCH_PACKAGE(sox "${src}" "${luasrc}" "Audio Processing")
    TARGET_LINK_LIBRARIES(sox luaT TH ${SOX_LIBRARIES})
#else (SOX_FOUND)
#    message ("WARNING: Could not find SOX libraries, audio I/O wrapper will not be installed")
#endif (SOX_FOUND)


include_directories (${FFTW_INCLUDE_DIR})
SET(src audio.c)
SET(luasrc init.lua voice.mp3)
ADD_TORCH_PACKAGE(audio "${src}" "${luasrc}" "Audio Processing")
TARGET_LINK_LIBRARIES(audio luaT TH ${SOX_LIBRARIES} ${FFTW_LIBRARIES})
