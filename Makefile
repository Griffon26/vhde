.PHONY: clean

CPPFLAGS=`pkg-config cluttermm-0.9 --cflags` \
	 `pkg-config libglademm-2.4 --cflags`
LDFLAGS=`pkg-config cluttermm-0.9 --libs` \
	`pkg-config libglademm-2.4 --libs`

SOURCES= main.cpp \
	 triangle_actor.cc \
	 layout_block.cpp \
	 layout_port.cpp \
	 layout_signal.cpp \
	 vhdl_architecture.cpp \
	 vhdl_generic.cpp \
	 vhdl_instance.cpp \
	 vhdl_interface.cpp \
	 vhdl_port.cpp \
	 vhdl_signal.cpp \
	 vhdl_type.cpp

OBJECTS=$(patsubst %.cpp,%.o,$(filter %.cpp,$(SOURCES))) \
	$(patsubst %.cc,%.o,$(filter %.cc,$(SOURCES)))

main: $(OBJECTS)

clean:
	rm -f main $(OBJECTS)

