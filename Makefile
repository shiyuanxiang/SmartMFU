TOP = SimTop
BUILD_DIR = ./build
TOP_V = $(BUILD_DIR)/$(TOP).v
SCALA_FILE = $(shell find ./src/main/scala -name '*.scala')
TEST_FILE = $(shell find ./src/test/scala -name '*.scala')

SIMTOP = top.SimTop
SIM_TOP = SimTop
SIM_TOP_V = $(BUILD_DIR)/$(SIM_TOP).v
SIM_ARGS =
$(SIM_TOP_V): $(SCALA_FILE)
	mkdir -p $(@D)
	mill Smart.test.runMain top.$(SIM_TOP) -td $(@D) --full-stacktrace --output-file $(@F)

# -c:$(SIMTOP):-o:$(@D)/$(@F).conf $(SIM_ARGS)

.DEFAULT_GOAL = emu



EMU_CSRC_DIR = $(abspath ./src/test/csrc)
# EMU_VSRC_DIR = $(abspath ./src/test/vsrc)
EMU_CXXFILES = $(shell find $(EMU_CSRC_DIR) -name "*.cpp")
# EMU_VFILES = $(shell find $(EMU_VSRC_DIR) -name "*.v" -or -name "*.sv")

VERILATOR_FLAGS = --top-module $(SIM_TOP) \
	--assert \
	--output-split 500 \
	--output-split-cfuncs 500 \
	-I$(abspath $(BUILD_DIR)) \
	-CFLAGS "-O3 -static -g -Wall -I$(EMU_CSRC_DIR)" \
	-LDFLAGS "-lpthread -lSDL2 -ldl"

EMU_MK := $(BUILD_DIR)/emu-compile/V$(SIM_TOP).mk
EMU_DEPS :=  $(EMU_CXXFILES)
# $(EMU_VFILES)
EMU_HEADERS := $(shell find $(EMU_CSRC_DIR) -name "*.h")
EMU = $(BUILD_DIR)/emu

$(EMU_MK): $(SIM_TOP_V) | $(EMU_DEPS)
	@mkdir -p $(@D)
	verilator --cc --exe $(VERILATOR_FLAGS) \
		-o $(abspath $(EMU)) -Mdir $(@D) $^ $(EMU_DEPS)

$(EMU): $(EMU_MK) $(EMU_DEPS) $(EMU_HEADERS)
	$(MAKE) VM_PARRLLEL_BUILDS=1 -C $(dir $(EMU_MK)) -f $(abspath $(EMU_MK))

emu: $(EMU)
	@$(EMU)

clean:
	rm -rf build/