PROJECT_ROOT := $(shell pwd)

PROJECT_NAME := customblas

PROJECT_LIB := customblas

MK_CONFIG ?= Makefile.config
ifeq ($(wildcard $(MK_CONFIG)),)
$(error Cannot find the config file $(MK_CONFIG))
endif

include $(MK_CONFIG)

# Common compilation flags
COMMON_FLAGS += -Wall -Wno-sign-compare -std=c++11 -O3

INCLUDE_DIRS += include

# CXX_SRC compilation flags
CXX_FLAGS += $(foreach include,$(INCLUDE_DIRS),-I$(include))

# Linking flags
LD_FLAGS += $(foreach lib,$(LIBRARIES),-l$(lib))
LD_FLAGS += $(foreach lib,$(LIBRARY_DIRS),-L$(lib))

LINK_FLAGS := -l$(PROJECT_LIB) -Lbuild/lib -Wl,-rpath,$(PROJECT_ROOT)/build/lib
LINK_FLAGS += $(foreach lib,$(LIBRARY_DIRS),-Wl,-rpath,$(lib))

## Find all the source files
# CXX_SRCS are all the source files stored in src folder
CXX_SRCS := $(shell find src -name "*.cpp")

# TOOL_SRCS are all the source files of tools that dependent on this library,
# all these source files are stored in tools folder, after compilation, each 
# of these file will generate an executable file in build/tools folder
TOOL_SRCS := $(shell find tools -name "*.cpp")

## Derive all the generated files
DYNAMIC_NAME := build/lib/lib$(PROJECT_LIB).so
STATIC_NAME := build/lib/lib$(PROJECT_LIB).a
CXX_OBJS := ${CXX_SRCS:%.cpp=build/%.o}
TOOL_BINS := ${TOOL_SRCS:%.cpp=build/%}
DEPS := ${CXX_OBJS:%.o=%.d}
DEPS += ${TOOL_BINS:%=%.d}

ALL_FILES := $(CXX_OBJS) $(TOOL_BINS) $(DYNAMIC_NAME) $(STATIC_NAME) $(DEPS)
MK_DIRS := $(dir $(ALL_FILES))

CXX_FLAGS += -MMD -MP

## Compilation start
all: lib tools

lib: $(DYNAMIC_NAME) $(STATIC_NAME)

tools: $(TOOL_BINS)

build/%:%.cpp $(DYNAMIC_NAME) $(STATIC_NAME)
	@mkdir -p $(MK_DIRS)
	@echo CXX/LD -o $@
	$(Q)$(CXX) -fPIC -o $@ $< $(COMMON_FLAGS) $(CXX_FLAGS) $(LINK_FLAGS) $(LD_FLAGS)


$(DYNAMIC_NAME):$(CXX_OBJS)
	@mkdir -p $(MK_DIRS)
	@echo LD -o $@
	$(Q)$(CXX) -fPIC -shared -o $@ $(CXX_OBJS) $(COMMON_FLAGS) $(LD_FLAGS)

$(STATIC_NAME):$(CXX_OBJS)
	@mkdir -p $(MK_DIRS)
	@echo AR -o $@
	$(Q)$(AR) rcs $@ $(CXX_OBJS)

build/%.o:%.cpp
	@mkdir -p $(MK_DIRS)
	@echo CXX $<
	$(Q)$(CXX) -fPIC -c -o $@ $< $(COMMON_FLAGS) $(CXX_FLAGS)


clean:
	rm -f $(ALL_FILES)
	rm -rf build

-include $(DEPS)
