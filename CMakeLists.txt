cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_EXTENSIONS off)

# -Wall only works for gcc or clang, but let's fix that when someone starts
# complaining.
# Also disable deprecation warnings, because there is no point in fixing them
# when I'm planning to migrate to GSK (the successor of clutter) once it's 
# available widely enough in Gtk.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-deprecated-declarations")

project (VHDE)

set(ANTLR_RUNTIME antlr4-cpp-runtime)
get_filename_component(ANTLR_RUNTIME_PREFIX ${ANTLR_RUNTIME}-prefix ABSOLUTE)

include(ExternalProject)
externalproject_add(${ANTLR_RUNTIME}
    PREFIX                ${ANTLR_RUNTIME_PREFIX}
    GIT_REPOSITORY        https://github.com/antlr/antlr4.git
    GIT_TAG               "4.7"
    UPDATE_COMMAND        ""
    PATCH_COMMAND         sed -i "s,FILES LICENSE.txt,FILES ../../LICENSE.txt," runtime/Cpp/CMakeLists.txt
    SOURCE_SUBDIR         runtime/Cpp
    CMAKE_ARGS            -DCMAKE_INSTALL_PREFIX=${ANTLR_RUNTIME_PREFIX}
)

find_package(PkgConfig)

pkg_check_modules(CLUTTERMM REQUIRED cluttermm-1.0)
pkg_search_module(GTKMM REQUIRED gtkmm-3.0 gtkmm-2.4)

include_directories(${CLUTTERMM_INCLUDE_DIRS}
                    ${GTKMM_INCLUDE_DIRS}
                    ${ANTLR_RUNTIME_PREFIX}/include/antlr4-runtime
                    src/clutter
                    src/common
                    src/gui
                    src/layout
                    src/parsers
                    src/parsers/generated
                    src/vhdl)
link_directories(${CLUTTERMM_LIBRARY_DIRS}
                 ${GTKMM_LIBRARY_DIRS}
                 ${ANTLR_RUNTIME_PREFIX}/lib)
link_libraries(${CLUTTERMM_LIBRARIES}
               ${GTKMM_LIBRARIES}
               ${ANTLR_RUNTIME_PREFIX}/lib/libantlr4-runtime.so)

set(SRCS src/main.cpp
         src/project.cpp
         src/clutter/clutter_block.cpp
         src/clutter/clutter_port.cpp
         src/clutter/clutter_scaling_text.cpp
         src/clutter/clutter_wire.cpp
         src/gui/gui_block.cpp
         src/gui/gui_component.cpp
         src/gui/gui_instance.cpp
         src/gui/gui_port.cpp
         src/gui/gui_signal.cpp
         src/layout/layout_architecture.cpp
         src/layout/layout_block.cpp
         src/layout/layout_component.cpp
         src/layout/layout_file.cpp
         src/layout/layout_instance.cpp
         src/layout/layout_port.cpp
         src/layout/layout_signal.cpp
         src/parsers/generated/vhdlBaseVisitor.cpp
         src/parsers/generated/vhdlLexer.cpp
         src/parsers/generated/vhdlParser.cpp
         src/parsers/generated/vhdlVisitor.cpp
         src/parsers/parse_layout.cpp
         src/parsers/parse_vhdl.cpp
         src/vhdl/vhdl_architecture.cpp
         src/vhdl/vhdl_component.cpp
         src/vhdl/vhdl_entity.cpp
         src/vhdl/vhdl_file.cpp
         src/vhdl/vhdl_fragment.cpp
         src/vhdl/vhdl_instance.cpp
         src/vhdl/vhdl_interface.cpp
         src/vhdl/vhdl_port.cpp
         src/vhdl/vhdl_signal.cpp
         src/vhdl/vhdl_type.cpp)

add_executable(vhde ${SRCS})

target_compile_features(vhde PRIVATE cxx_generic_lambdas
                                     cxx_noexcept
                                     cxx_override
                                     cxx_range_for
                       )

add_dependencies(vhde ${ANTLR_RUNTIME})

