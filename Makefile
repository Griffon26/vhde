.PHONY: clean

SOURCES= layout_block.cpp \
	 layout_port.cpp \
	 layout_signal.cpp \
	 vhdl_architecture.cpp \
	 vhdl_generic.cpp \
	 vhdl_instance.cpp \
	 vhdl_interface.cpp \
	 vhdl_type.cpp \
	 vhdl_wire.cpp

ALL_EXE= \
	01_moving_rectangles_by_dragging \
	02_dragging_port_on_component \
	main

all: $(ALL_EXE)

%: %.cpp
	 g++ -g $^ -o $@ \
						`pkg-config cluttermm-0.9 --cflags --libs` \
						`pkg-config libglademm-2.4 --cflags --libs`

main: main.cpp triangle_actor.cc $(SOURCES)
	 g++ -g $^ -o $@ \
						`pkg-config cluttermm-0.9 --cflags --libs` \
						`pkg-config libglademm-2.4 --cflags --libs`

clean:
	rm -f $(ALL_EXE)

