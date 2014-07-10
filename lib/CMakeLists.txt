include_directories(
	${LLVM_INCLUDE_DIRS} 
	${PROJECT_SOURCE_DIR}/include
	)

link_directories(${LLVM_LIBRARY_DIRS})

add_definitions(-Wall -Wno-unused-variable -fPIC)
if(ENABLE_DEBUG)
   add_definitions(-DENABLE_DEBUG)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${LLVM_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${LLVM_CPP_FLAGS}")

add_library(IFDup SHARED
	ShortcutDetector.cpp
	ParIFDuplica.cpp
	RedundAnalysis.cpp
	InsDuplica.cpp
   LockInst.cpp
	)
