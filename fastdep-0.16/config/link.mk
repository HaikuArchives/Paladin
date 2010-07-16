#
# link_cc macro
#
#  arguments :
#    binaryname
#    linker flags
#    object files
#
#  example use : $(call link_cc, a.out, -O2, main.o menu.o load.o save.o)
#

ifeq ($(HOST_SYSTEM), cygwin)
link_cc = echo $(3) > linkfiles; g++ $(2) -o $(1) @linkfiles
else
link_cc = g++ $(2) -o $(1) $(3)
endif
