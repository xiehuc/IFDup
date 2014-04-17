include_directories(
	${LLVM_INCLUDE_DIRS} 
	${PROJECT_SOURCE_DIR}/include
	)

link_directories(${LLVM_LIBRARY_DIRS})

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${LLVM_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${LLVM_CPP_FLAGS}")

add_library(IFDup SHARED
	InsDuplica/InsDuplica.cpp
	InsDuplica/RedundAnalysis.cpp
	ParIFDuplica/ParIFDuplica.cpp
	ShortcutDetector/ShortcutDetector.cpp
	)
