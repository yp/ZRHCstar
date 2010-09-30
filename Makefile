##############################################################
#               CMake Project Wrapper Makefile               #
############################################################## 
DEFAULT_STATUS := debug

ifndef STATUS
STATUS:=__undefined__status__
endif


ifneq ($(STATUS), debug)
ifneq ($(STATUS), release)

$(info No status specified. Using the default status "${DEFAULT_STATUS}")

STATUS:=${DEFAULT_STATUS}

endif
endif

SHELL := /bin/bash
RM    := rm -rf

all: ./build/${STATUS}/Makefile
	@ $(MAKE) -C build/${STATUS}

./build/${STATUS}/Makefile:
	@ (mkdir -p build/${STATUS} bin docs && \
		cd build/${STATUS} >/dev/null 2>&1 && \
		cmake -DCMAKE_BUILD_TYPE=${STATUS} ../.. \
	)

distclean:
	@- $(RM) ./bin/*
	@- $(RM) ./build/*
	@- $(RM) ./docs/*.html
	@- $(RM) ./docs/*.css
	@- $(RM) ./docs/*.png
	@- $(RM) ./docs/*.jpg
	@- $(RM) ./docs/*.gif
	@- $(RM) ./docs/*.tiff
	@- $(RM) ./docs/*.php
	@- $(RM) ./docs/search
	@- $(RM) ./docs/installdox


ifeq ($(findstring distclean,$(MAKECMDGOALS)),)

    $(MAKECMDGOALS): ./build/${STATUS}/Makefile
	@ $(MAKE) -C build/${STATUS} $(MAKECMDGOALS)

endif
