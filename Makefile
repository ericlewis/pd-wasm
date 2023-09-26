HEAP_SIZE      = 8388208
STACK_SIZE     = 61800

PRODUCT = pd-wasm.pdx

# Locate the SDK
SDK = ${PLAYDATE_SDK_PATH}
ifeq ($(SDK),)
SDK = $(shell egrep '^\s*SDKRoot' ~/.Playdate/config | head -n 1 | cut -c9-)
endif

ifeq ($(SDK),)
$(error SDK path not found; set ENV value PLAYDATE_SDK_PATH)
endif

# List C source files here
SRC = Source/main.c
SRC += Source/wasm3/m3_api_libc.c
SRC += Source/wasm3/m3_api_tracer.c
SRC += Source/wasm3/m3_bind.c
SRC += Source/wasm3/m3_code.c
SRC += Source/wasm3/m3_compile.c
SRC += Source/wasm3/m3_core.c
SRC += Source/wasm3/m3_env.c
SRC += Source/wasm3/m3_exec.c
SRC += Source/wasm3/m3_function.c
SRC += Source/wasm3/m3_info.c
SRC += Source/wasm3/m3_module.c
SRC += Source/wasm3/m3_parse.c

# List all user directories here
UINCDIR = Source
UINCDIR += Source/wasm3

# List user asm files
UASRC =

# List all user C define here, like -D_DEBUG=1
UDEFS = -Wl,-y,_write

# Define ASM defines here
UADEFS =

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =

include $(SDK)/C_API/buildsupport/common.mk

