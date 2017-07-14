cmake_minimum_required(VERSION 3.2.2)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
       "Choose the type of build, options are: Debug Release."
       FORCE)
endif(NOT CMAKE_BUILD_TYPE)

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
    GIT_REPOSITORY        https://github.com/dutchedge/antlr4.git
    GIT_TAG               "4.7-unique_ptr"
    UPDATE_COMMAND        ""
    PATCH_COMMAND         mv "${ANTLR_RUNTIME_PREFIX}/src/${ANTLR_RUNTIME}" "${ANTLR_RUNTIME_PREFIX}/src/${ANTLR_RUNTIME}_original" &&
                          mv "${ANTLR_RUNTIME_PREFIX}/src/${ANTLR_RUNTIME}_original/runtime/Cpp" "${ANTLR_RUNTIME_PREFIX}/src/${ANTLR_RUNTIME}" &&
                          cd "${ANTLR_RUNTIME_PREFIX}/src" && patch -p0 < ../../antlr4-cpp-runtime-4.7-only-build-shared.patch
    CMAKE_ARGS            -DCMAKE_INSTALL_PREFIX=${ANTLR_RUNTIME_PREFIX}
                          -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                          -DWITH_LIBCXX=Off
)

find_package(PkgConfig)

pkg_check_modules(CLUTTERMM REQUIRED cluttermm-1.0)
pkg_check_modules(CLUTTERGTKMM REQUIRED clutter-gtkmm-1.0)
pkg_search_module(GTKMM REQUIRED gtkmm-3.0 gtkmm-2.4)

include_directories(${CLUTTERMM_INCLUDE_DIRS}
                    ${CLUTTERGTKMM_INCLUDE_DIRS}
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
                 ${CLUTTERGTKMM_LIBRARY_DIRS}
                 ${GTKMM_LIBRARY_DIRS}
                 ${ANTLR_RUNTIME_PREFIX}/lib)
link_libraries(${CLUTTERMM_LIBRARIES}
               ${CLUTTERGTKMM_LIBRARIES}
               ${GTKMM_LIBRARIES}
               ${ANTLR_RUNTIME_PREFIX}/lib/libantlr4-runtime.so)

set(SRCS src/main.cpp
         src/architecture_stage_updater.cpp
         src/entity_stage_updater.cpp
         src/project.cpp
         src/project_treeview_updater.cpp
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
         src/vhde_application.cpp
         src/vhde_window.cpp
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

# This definition enables a work-around for
# https://bugzilla.gnome.org/show_bug.cgi?id=783362
add_definitions(-DCLUTTER_GTKMM_BUG)

target_compile_features(vhde PRIVATE cxx_generic_lambdas
                                     cxx_noexcept
                                     cxx_override
                                     cxx_range_for
                       )

add_dependencies(vhde ${ANTLR_RUNTIME})

