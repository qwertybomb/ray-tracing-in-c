cmake_minimum_required(VERSION 3.17)
project(ray_tracing)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS  "-m64 -openmp -W3")

include_directories("C:\\DEV\\vcpkg\\installed\\x64-windows\\include")
link_directories(${PROJECT_SOURCE_DIR} "C:\\DEV\\vcpkg\\installed\\x64-windows\\lib" "C:\\DEV\\vcpkg\\installed\\x64-windows\\lib\\manual-link")
add_executable(ray_tracing ray_trace.c types.h vec.h texture.h sphere.h ray_trace.h ray.h moving_sphere.h material.h hittable_list.h hittable.h camera.h bvh.h aabb.h random.cc box.h aarect.h)

target_link_libraries(${PROJECT_NAME} SDL2main SDL2 libomp)
