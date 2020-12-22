add_subdirectory(DBoW2_Origin)

add_executable(OriginCompare OriginCompare.cpp)
target_link_libraries(OriginCompare DBoW2 DBoW2::origin)

add_executable(DbQuery DbQuery.cpp)
target_link_libraries(DbQuery DBoW2)
