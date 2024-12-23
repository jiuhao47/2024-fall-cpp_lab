cmake_minimum_required(VERSION 3.1.0)
project(liveness_example)
find_package(LLVM REQUIRED CONFIG)
# HINTS ${LLVM_DIR} ${LLVM_DIR}/lib/cmake/llvm
# 	                NO_DEFAULT_PATH)

include_directories(${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS} SYSTEM)
link_directories(${LLVM_LIBRARY_DIRS})
message(STATUS "LLVM_LIB DIR : ${LLVM_LIBRARY_DIRS}")
set(LLVM_LINK_COMPONENTS
  LLVMCore
  LLVMIRReader
  LLVMPasses
  )

message(STATUS "LLVM LIBS : ${LLVM_LINK_COMPONENTS}")
# Support plugins.
file(GLOB SOURCE "./*.cpp")
add_executable(liveness_example ${SOURCE})

target_link_libraries(liveness_example
	${LLVM_LINK_COMPONENTS}
	)
