
#main 
add_executable(damage main.cpp math.cpp raster.cpp)

#require pkg-config support
find_package(PkgConfig REQUIRED)

#required pkg-config libs
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMG REQUIRED SDL2_image)

#add include dirs
include_directories(${SDL2_INCLUDE_DIRS} ${SDL_IMG_INCLUDE_DIRS})

#set link libraries
target_link_libraries(damage ${SDL2_LIBRARIES} ${SDL2_IMG_LIBRARIES})

#add custom lib flags
add_definitions(-std=c++11 -O3)
