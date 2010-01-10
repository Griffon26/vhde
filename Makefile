.PHONY: clean

CPPFLAGS=-g -O0 \
	 `pkg-config cluttermm-1.0 --cflags` \
	 `pkg-config libglademm-2.4 --cflags`
LDFLAGS=`pkg-config cluttermm-1.0 --libs` \
	`pkg-config libglademm-2.4 --libs`

%.d: %.cpp
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< | sed "s/$*.o/& $@/g" > $@'

SOURCES= main.cpp \
	 clutter_port.cpp \
	 gui_block.cpp \
	 gui_instance.cpp \
	 layout_block.cpp \
	 layout_instance.cpp \
	 layout_signal.cpp \
	 vhdl_architecture.cpp \
	 vhdl_component.cpp \
	 vhdl_entity.cpp \
	 vhdl_generic.cpp \
	 vhdl_instance.cpp \
	 vhdl_interface.cpp \
	 vhdl_port.cpp \
	 vhdl_signal.cpp \
	 vhdl_type.cpp

OBJECTS=$(patsubst %.cpp,%.o,$(filter %.cpp,$(SOURCES)))
DEPS=$(SOURCES:.cpp=.d)

main: $(OBJECTS)

clean:
	rm -f main $(OBJECTS) $(DEPS)

include $(DEPS)


