cmake_minimum_required(VERSION 3.13.4)
project(qt_viewer VERSION 1.0.0 LANGUAGES CXX)

# set(HOOPS_EXCHANGE_DIR "X:/HOOPS_Exchange_Publish_2021_SP2_U1" CACHE PATH "HOOPS Exchange installation")
# set(HOOPS_VISUALIZE_DIR "X:/HOOPS_Visualize_2021_SP2_U1" CACHE PATH "HOOPS Visualize installation")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(qt_viewer
  main.cpp
  Application.cpp
  BOMTableWidget.cpp
  HPSWidget.cpp
  MainWindow.cpp
  SceneGraphBuilder.cpp
)

include_directories(
  "${HOOPS_EXCHANGE_DIR}/include"
  "${HOOPS_VISUALIZE_DIR}/include"
)

if(UNIX AND NOT APPLE)
  add_link_options(-rpath "${HOOPS_VISUALIZE_DIR}/bin/linux64")
  target_link_directories(qt_viewer PRIVATE "${HOOPS_VISUALIZE_DIR}/bin/linux64")
elseif(APPLE)
  add_link_options(-rpath "${HOOPS_VISUALIZE_DIR}/bin/macos")
  target_link_directories(qt_viewer PRIVATE "${HOOPS_VISUALIZE_DIR}/bin/macos")
endif()

if(MSVC)
  target_link_libraries(qt_viewer PRIVATE
    Qt6::Widgets
    "${HOOPS_VISUALIZE_DIR}/lib/win64_v141d/hps_core.lib"
    "${HOOPS_VISUALIZE_DIR}/lib/win64_v141d/hps_sprk.lib"
    "${HOOPS_VISUALIZE_DIR}/lib/win64_v141d/hps_sprk_ops.lib"
  )

  add_custom_command(TARGET qt_viewer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${HOOPS_VISUALIZE_DIR}/bin/win64_v141d/hps_core.dll"
    "${HOOPS_VISUALIZE_DIR}/bin/win64_v141d/hps_sprk.dll"
    "${HOOPS_VISUALIZE_DIR}/bin/win64_v141d/hps_sprk_ops.dll"
    $<TARGET_FILE_DIR:qt_viewer>
  )
else()
  target_link_libraries(qt_viewer PRIVATE
    Qt6::Widgets
    hps_core
    hps_sprk
    hps_sprk_ops
    dl
  )
endif()