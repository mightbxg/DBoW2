set(HDRS
  include/origin/BowVector.h           include/origin/FBrief.h
  include/origin/QueryResults.h        include/origin/TemplatedDatabase.h   include/origin/FORB.h
  include/origin/DBoW2.h               include/origin/FClass.h              include/origin/FeatureVector.h
  include/origin/ScoringObject.h       include/origin/TemplatedVocabulary.h)
set(SRCS
  src/BowVector.cpp     src/FBrief.cpp        src/FORB.cpp
  src/FeatureVector.cpp src/QueryResults.cpp  src/ScoringObject.cpp)
add_library(origin STATIC ${HDRS} ${SRCS})
add_library(DBoW2::origin ALIAS origin)
target_link_libraries(origin PUBLIC ${OpenCV_LIBS})
target_include_directories(origin
    PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include")
