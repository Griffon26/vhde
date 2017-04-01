cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_EXTENSIONS off)

project (VHDE)

find_package(PkgConfig)

pkg_check_modules(CLUTTERMM REQUIRED cluttermm-1.0)
pkg_search_module(GTKMM REQUIRED gtkmm-3.0 gtkmm-2.4)

include_directories(${CLUTTERMM_INCLUDE_DIRS}
                    ${GTKMM_INCLUDE_DIRS}
                    src/clutter
                    src/common
                    src/gui
                    src/layout
                    src/vhdl)
link_directories(${CLUTTERMM_LIBRARY_DIRS}
                 ${GTKMM_LIBRARY_DIRS})
link_libraries(${CLUTTERMM_LIBRARIES}
               ${GTKMM_LIBRARIES})

set(SRCS src/main.cpp
         src/clutter/clutter_block.cpp
         src/clutter/clutter_port.cpp
         src/clutter/clutter_scaling_text.cpp
         src/clutter/clutter_wire.cpp
         src/gui/gui_block.cpp
         src/gui/gui_component.cpp
         src/gui/gui_instance.cpp
         src/gui/gui_port.cpp
         src/gui/gui_signal.cpp
         src/layout/layout_block.cpp
         src/layout/layout_component.cpp
         src/layout/layout_instance.cpp
         src/layout/layout_port.cpp
         src/layout/layout_signal.cpp
         src/vhdl/vhdl_architecture.cpp
         src/vhdl/vhdl_component.cpp
         src/vhdl/vhdl_entity.cpp
         src/vhdl/vhdl_generic.cpp
         src/vhdl/vhdl_instance.cpp
         src/vhdl/vhdl_interface.cpp
         src/vhdl/vhdl_port.cpp
         src/vhdl/vhdl_signal.cpp
         src/vhdl/vhdl_type.cpp)

add_executable(vhde ${SRCS})

target_compile_features(vhde PRIVATE cxx_noexcept
                                     cxx_override)

