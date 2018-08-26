# CMake definitions for SoftFM

cmake_minimum_required(VERSION 2.4)
project(SoftDefinedBlocks)

find_package(Threads)
find_package(PkgConfig)
find_package(ALSA REQUIRED)

# Find RTL-SDR library (for linking).
pkg_check_modules(PKG_RTLSDR librtlsdr)
find_path(RTLSDR_INCLUDE_DIR rtl-sdr.h
          HINT ${PKG_RTLSDR_INCLUDE_DIRS})
find_library(RTLSDR_LIBRARY librtlsdr.a
             HINT ${PKG_RTLSDR_LIBRARY_DIRS})

# Find libusb
pkg_check_modules(PKG_LIBUSB libusb-1.0)
find_path(LIBUSB_INCLUDE_DIR libusb.h
          HINT ${PKG_LIBUSB_INCLUDE_DIRS}
          PATH_SUFFIXES libusb-1.0)
find_library(LIBUSB_LIBRARY usb-1.0
             HINT ${PKG_LIBUSB_LIBRARY_DIRS})

if(RTLSDR_INCLUDE_DIR AND RTLSDR_LIBRARY)
    message(STATUS "Found librtlsdr: ${RTLSDR_INCLUDE_DIR}, ${RTLSDR_LIBRARY}")
else()
    message(WARNING "Can not find Osmocom RTL-SDR library")
    message("Try again with environment variable PKG_CONFIG_PATH")
    message("or with -DRTLSDR_INCLUDE_DIR=/path/rtlsdr/include")
    message("        -DRTLSDR_LIBRARY=/path/rtlsdr/lib/librtlsdr.a")
endif()

set(RTLSDR_INCLUDE_DIRS ${RTLSDR_INCLUDE_DIR} ${LIBUSB_INCLUDE_DIR})
set(RTLSDR_LIBRARIES    ${RTLSDR_LIBRARY} ${LIBUSB_LIBRARY})

# Compiler flags.
set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -O2 -ffast-math -ftree-vectorize ${EXTRA_FLAGS}")

#add_executable(softfm)

#add_subdirectory(Main)
#add_subdirectory(ProcessBlocks)


if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)


add_executable(SoftDefinedBlocks
	Main/BlockSet.cpp
	Main/main.cpp
	Main/Misc.cpp 
	Main/SeqControl.cpp 
	Main/Variables.cpp
	Main/Error.cpp
	Main/ProcessScript.cpp
	Main/SimpleParser.cpp
	ProcessBlocks/downsample.cpp
	ProcessBlocks/FineTuner.cpp
	ProcessBlocks/IQFFT.cpp
	ProcessBlocks/lowPassRC.cpp
	ProcessBlocks/FFT.cpp
	ProcessBlocks/highPassIIR.cpp
	ProcessBlocks/lowPassFIR.cpp
	ProcessBlocks/PhaseDiscriminator.cpp
	Server/ClientConnect.cpp
	Server/ServerSocket.cpp
	Server/simpleFFT.cpp
	Server/Socket.cpp
	SourceBlocks/RtlSdrsource.cpp  
	SourceBlocks/sourceRtlSdr.cpp
	SinkBlocks/AudioOutput.cpp  
	SinkBlocks/sinkAudio.cpp
	Widgets/iWidget.cpp
	Widgets/WidgetCommStats.cpp
	Widgets/WidgetMenu.cpp
	Widgets/WidgetSlider.cpp
	Widgets/WidgetButton.cpp
	Widgets/WidgetContainer.cpp
	Widgets/WidgetRadio.cpp
	Widgets/WidgetText.cpp
	Widgets/WidgetCommon.cpp
	Widgets/WidgetGraph.cpp
	Widgets/WidgetSelect.cpp
	Widgets/WidgetVarList.cpp
	Testing/test_main.cpp )


include_directories(
    ${RTLSDR_INCLUDE_DIRS}
    ${ALSA_INCLUDE_DIRS}
    ${EXTRA_INCLUDES}
    Main)

target_link_libraries(SoftDefinedBlocks
    ${CMAKE_THREAD_LIBS_INIT}
    ${RTLSDR_LIBRARIES}
    ${ALSA_LIBRARIES}
    ${EXTRA_LIBS} )

install(TARGETS SoftDefinedBlocks DESTINATION bin)

