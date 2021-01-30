# CMakeList.txt : CMake project for tesseractocr, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(project_name tesseract-ocr-demo)
project(${project_name})

include_directories(
	${CMAKE_SOURCE_DIR}/
	${CMAKE_SOURCE_DIR}/tesseract-ocr/include
	${CMAKE_BINARY_DIR}/tesseract-ocr/include
	${CMAKE_BINARY_DIR}/opencv-4.5.1
	${CMAKE_SOURCE_DIR}/opencv-4.5.1/include
	${CMAKE_SOURCE_DIR}/opencv-4.5.1/include/modules/calib3d/include
	${CMAKE_SOURCE_DIR}/opencv-4.5.1/include/modules/core/include
	${CMAKE_SOURCE_DIR}/opencv-4.5.1/include/modules/dnn/include
	${CMAKE_SOURCE_DIR}/opencv-4.5.1/include/modules/features2d/include
	${CMAKE_SOURCE_DIR}/opencv-4.5.1/include/modules/flann/include
	${CMAKE_SOURCE_DIR}/opencv-4.5.1/include/modules/gapi/include
	${CMAKE_SOURCE_DIR}/opencv-4.5.1/include/modules/highgui/include
	${CMAKE_SOURCE_DIR}/opencv-4.5.1/include/modules/imgcodecs/include
	${CMAKE_SOURCE_DIR}/opencv-4.5.1/include/modules/imgproc/include
	${CMAKE_SOURCE_DIR}/opencv-4.5.1/include/modules/ml/include
	${CMAKE_SOURCE_DIR}/opencv-4.5.1/include/modules/objdetect/include
	${CMAKE_SOURCE_DIR}/opencv-4.5.1/include/modules/photo/include
	${CMAKE_SOURCE_DIR}/opencv-4.5.1/include/modules/stitching/include
	${CMAKE_SOURCE_DIR}/opencv-4.5.1/include/modules/ts/include
	${CMAKE_SOURCE_DIR}/opencv-4.5.1/include/modules/video/include
	${CMAKE_SOURCE_DIR}/opencv-4.5.1/include/modules/videoio/include
	${CMAKE_SOURCE_DIR}/opencv-4.5.1/include/modules/world/include
	${CMAKE_SOURCE_DIR}/leptonica-1.79.0/include
)
if(CMAKE_CL_64)
	if (CMAKE_BUILD_TYPE STREQUAL Debug)
        set(LIB_EXT d)
		set(OPENCV_LEPTONICA_LIBRARY_DIRS 
		${CMAKE_SOURCE_DIR}/leptonica-1.79.0/lib/x64/Debug
		${CMAKE_SOURCE_DIR}/opencv-4.5.1/lib/x64/Debug
		)
	else()
        set(LIB_EXT )
		set(OPENCV_LEPTONICA_LIBRARY_DIRS 
		${CMAKE_SOURCE_DIR}/leptonica-1.79.0/lib/x64/Release
		${CMAKE_SOURCE_DIR}/opencv-4.5.1/lib/x64/Release
		)
	endif()
else(CMAKE_CL_64)
	if (CMAKE_BUILD_TYPE STREQUAL Debug)
        set(LIB_EXT d)
		set(OPENCV_LEPTONICA_LIBRARY_DIRS 
		${CMAKE_SOURCE_DIR}/leptonica-1.79.0/lib/Win32/Debug
		${CMAKE_SOURCE_DIR}/opencv-4.5.1/lib/Win32/Debug
		)
	else()
        set(LIB_EXT )
		set(OPENCV_LEPTONICA_LIBRARY_DIRS 
		${CMAKE_SOURCE_DIR}/leptonica-1.79.0/lib/Win32/Release
		${CMAKE_SOURCE_DIR}/opencv-4.5.1/lib/Win32/Release
		)
	endif()
endif(CMAKE_CL_64)
link_directories(
	${CMAKE_BINARY_DIR}/tesseract-ocr
	${OPENCV_LEPTONICA_LIBRARY_DIRS}
)
set(OPENCV_LEPTONICA_LIBRARIES 
	ade          
	giflib          
	leptonica
	libwebp${LIB_EXT}               
	opencv_imgproc451${LIB_EXT}   
	opencv_calib3d451${LIB_EXT}     
	opencv_ml451${LIB_EXT}
	IlmImf${LIB_EXT}        
	opencv_core451${LIB_EXT}       
	opencv_objdetect451${LIB_EXT}
	ittnotify${LIB_EXT}     
	opencv_dnn451${LIB_EXT}        
	opencv_photo451${LIB_EXT}
	libjpeg-turbo${LIB_EXT} 
	opencv_features2d451${LIB_EXT}  
	opencv_stitching451${LIB_EXT}
	libopenjp2${LIB_EXT}    
	opencv_flann451${LIB_EXT}     
	opencv_video451${LIB_EXT}
	libpng${LIB_EXT}        
	opencv_gapi451${LIB_EXT}       
	opencv_videoio451${LIB_EXT}
	libprotobuf${LIB_EXT}   
	opencv_highgui451${LIB_EXT}     
	quirc${LIB_EXT}
	libtiff${LIB_EXT}      
	opencv_imgcodecs451${LIB_EXT}  
	zlib${LIB_EXT}
)
# Add source to this project's executable.
add_executable (${project_name} "tesseract-ocr-demo.cpp")
add_library (${project_name}_dll SHARED 
	"tesseract-ocr-demo.cpp" 
	"tesseract-ocr-demo.def" 
	"tesseract-ocr-demo.rc"
)
if (MSVC)
	list(APPEND project_name_cflags /W4)
	list(APPEND project_name_defines WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0501)
	MESSAGE(STATUS "CMAKE_BUILD_TYPE=" ${CMAKE_BUILD_TYPE})

	set(CompilerFlags
		CMAKE_CXX_FLAGS
		CMAKE_CXX_FLAGS_DEBUG
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_C_FLAGS
		CMAKE_C_FLAGS_DEBUG
		CMAKE_C_FLAGS_RELEASE
		)
	foreach(CompilerFlag ${CompilerFlags})
		string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
	endforeach()
	
	target_compile_definitions(${project_name}
							PRIVATE ${project_name_defines}
							)
	target_compile_options(${project_name}
							PRIVATE ${project_name_cflags}
	)
	target_compile_definitions(${project_name}_dll
							PRIVATE ${project_name_defines} -DBUILD_DLL_LIBRARY=1
							)
	target_compile_options(${project_name}_dll
							PRIVATE ${project_name_cflags}
	)
else()
    set(CMAKE_C_FLAGS "$ENV{CFLAGS} ${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} ${CMAKE_CXX_FLAGS} -fPIC")
endif(MSVC)
target_link_libraries(${project_name} 
	PRIVATE libtesseract
	PRIVATE ${OPENCV_LEPTONICA_LIBRARIES}
)
target_link_libraries(${project_name}_dll 
	PRIVATE libtesseract
	PRIVATE ${OPENCV_LEPTONICA_LIBRARIES}
)

add_executable(ocr_app "ocr-app.cpp")
if (MSVC)
	list(APPEND project_name_cflags /W4)
	list(APPEND project_name_defines WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0501)
	MESSAGE(STATUS "CMAKE_BUILD_TYPE=" ${CMAKE_BUILD_TYPE})

	set(CompilerFlags
		CMAKE_CXX_FLAGS
		CMAKE_CXX_FLAGS_DEBUG
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_C_FLAGS
		CMAKE_C_FLAGS_DEBUG
		CMAKE_C_FLAGS_RELEASE
		)
	foreach(CompilerFlag ${CompilerFlags})
		string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
	endforeach()
	
	target_compile_definitions(ocr_app
							PRIVATE ${project_name_defines}
							)
	target_compile_options(ocr_app
							PRIVATE ${project_name_cflags}
	)
else()
    set(CMAKE_C_FLAGS "$ENV{CFLAGS} ${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} ${CMAKE_CXX_FLAGS} -fPIC")
endif(MSVC)
# TODO: Add tests and install targets if needed.
