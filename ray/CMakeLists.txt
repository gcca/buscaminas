find_package(raylib 6.0 QUIET)
if (NOT raylib_FOUND)
  enable_language(C)
  include(FetchContent)
  set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/6.0.tar.gz
    URL_HASH SHA256=2b3ee1e2120c7a0796b33062c7e9a694dd8a8caa56a96319ac8c8ecf54a90d0b)
  FetchContent_MakeAvailable(raylib)
endif()

add_executable(buscaminas-ray buscaminas-ray.cc)
target_compile_features(buscaminas-ray PRIVATE cxx_std_20)
target_link_libraries(buscaminas-ray PRIVATE buscaminas raylib)
