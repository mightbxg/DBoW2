add_subdirectory(DBoW2_Origin)

add_executable(OriginCompare OriginCompare.cpp)
target_link_libraries(OriginCompare DBoW2 DBoW2::origin)
