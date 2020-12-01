add_subdirectory(DBoW2_Origin)

add_executable(LoadVocabulary LoadVocabulary.cpp)
target_link_libraries(LoadVocabulary DBoW2)
