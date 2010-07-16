ifdef FASTDEP_BIN
generate-depends = \
$(FASTDEP_BIN) \
$(foreach dir,$(INCLUDEDIRS),-I $(dir)) \
--remakedeptarget=$@ \
$(foreach class,$($(call classgroup,fastdep,$(*F))),$(@D)/$(class).cc) > $@
else
generate-depends = \
set -e; \
g++ -MM \
$(foreach dir,$(INCLUDEDIRS),-I $(dir)) \
$(foreach class,$($(call classgroup,fastdep,$(*F))),$(@D)/$(class).cc) \
	| sed 's/\(.*\)\.o[ :]*/$(@D)\/\1.o $(subst /,\/,$@) : /g' > $@; \
	[ -s $@ ] || rm -f $@
endif
