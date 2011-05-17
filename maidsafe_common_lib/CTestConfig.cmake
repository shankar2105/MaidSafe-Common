## This file should be placed in the root directory of your project.
## Then modify the CMakeLists.txt file in the root directory of your
## project to incorporate the testing dashboard.
## # The following are required to uses Dart and the Cdash dashboard
##   ENABLE_TESTING()
##   INCLUDE(CTest)
set(CTEST_PROJECT_NAME "maidsafe-common")
set(CTEST_NIGHTLY_START_TIME "00:01:00 GMT")
set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "dash.maidsafe.net")
set(CTEST_DROP_LOCATION "/submit.php?project=Maidsafe-Common")
set(CTEST_DROP_SITE_CDASH TRUE)
IF(CMAKE_CL_64)
  SET(BUILDNAME Win-x64-MSBuild)
ENDIF()
