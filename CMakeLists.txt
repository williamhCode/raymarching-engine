cmake_minimum_required(VERSION 3.5...3.28)

project(
  RaymarchEngine
  VERSION 0.1.0
  DESCRIPTION "Raymarching Engine"
  LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # clangd

# deps ---------------------------
# set(Boost_USE_STATIC_LIBS ON)
# find_package(Boost 1.86.0 REQUIRED COMPONENTS
#   process
#   program_options
# )

add_subdirectory(deps/webgpu_tools EXCLUDE_FROM_ALL)

# raymarch target -------------------------
set(APP_SRC
	main.cpp

  gfx/context.cpp
  gfx/pipeline.cpp
  gfx/camera.cpp

  utils/logger.cpp
)
list(TRANSFORM APP_SRC PREPEND "src/")

add_executable(raymarch ${APP_SRC})

if (NOT EMSCRIPTEN)
  target_compile_definitions(raymarch PRIVATE
    ROOT_DIR="${PROJECT_SOURCE_DIR}"
  )
else()
  target_compile_definitions(raymarch PRIVATE
    ROOT_DIR=""
  )
endif()

target_include_directories(raymarch PRIVATE 
  ${PROJECT_SOURCE_DIR}/src
)

if (NOT DEFINED ENV{LLVM_PATH})
  execute_process(COMMAND brew --prefix llvm OUTPUT_VARIABLE LLVM_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(ENV{LLVM_PATH} ${LLVM_PATH})
endif()

target_link_libraries(raymarch PRIVATE 
  webgpu_tools
)

if (NOT EMSCRIPTEN) 
  target_link_libraries(raymarch PRIVATE 
    # https://github.com/llvm/llvm-project/issues/77653
    # stack unwinding breaking and linking breaks without this
    "-L$ENV{LLVM_PATH}/lib/c++ -L$ENV{LLVM_PATH}/lib/unwind -lunwind"
  )
endif()


target_compile_options(raymarch PRIVATE
  -Wall -Wextra -Wshadow -pedantic 
  -Wno-missing-field-initializers
  -Wno-deprecated-declarations
  # jthread clang
  -fexperimental-library
  -Wno-error=narrowing
  # --embed-dir=${PROJECT_SOURCE_DIR}/res
)

if (EMSCRIPTEN)
  execute_process(COMMAND em++ --cflags OUTPUT_VARIABLE EM_CFLAGS)
  string(STRIP "${EM_CFLAGS}" EM_CFLAGS)
  set_target_properties(raymarch PROPERTIES COMPILE_FLAGS "${EM_CFLAGS}")

  set_target_properties(raymarch PROPERTIES 
    SUFFIX ".html"
    LINK_DEPENDS "${PROJECT_SOURCE_DIR}/res/*"
  )

  target_link_options(raymarch PRIVATE 
    --preload-file "${PROJECT_SOURCE_DIR}/res@/res"
  )
endif()

if (XCODE)
  target_compile_definitions(raymarch PRIVATE
    XCODE=1
  )
  set_target_properties(raymarch PROPERTIES
    XCODE_GENERATE_SCHEME ON
    XCODE_SCHEME_ENABLE_GPU_FRAME_CAPTURE_MODE "Metal"
    XCODE_SCHEME_ENVIRONMENT "PATH=$ENV{PATH};DAWN_DEBUG_BREAK_ON_ERROR=1"
    XCODE_SCHEME_ADDRESS_SANITIZER ON
    XCODE_SCHEME_UNDEFINED_BEHAVIOUR_SANITIZER ON
  )
  set(CMAKE_XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS "-o linker-signed")
  set(CMAKE_XCODE_ATTRIBUTE_COMPILER_INDEX_STORE_ENABLE "No")
endif()
