ifeq ($(DEBUGSYMBOLS),yes)
CFLAGS+=-g
endif
ifeq ($(OPTIMIZE),yes)
CFLAGS+=-O2
endif
