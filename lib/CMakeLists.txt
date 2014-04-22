include_directories(
	${LLVM_INCLUDE_DIRS} 
	${PROJECT_SOURCE_DIR}/include
	)

link_directories(${LLVM_LIBRARY_DIRS})

add_definitions(-Wall -fPIC)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${LLVM_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${LLVM_CPP_FLAGS}")

add_library(IFDup SHARED
	#llvm/Dominators.cpp
	#llvm/PostDominators.cpp
	ParIFDuplica/ParIFDuplica.cpp
	ShortcutDetector/ShortcutDetector.cpp
	InsDuplica/RedundAnalysis.cpp
	InsDuplica/InsDuplica.cpp
	)
