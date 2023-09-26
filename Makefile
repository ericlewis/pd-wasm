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
SRC = src/main.c
SRC += src/wasm3/m3_api_libc.c
SRC += src/wasm3/m3_api_tracer.c
SRC += src/wasm3/m3_bind.c
SRC += src/wasm3/m3_code.c
SRC += src/wasm3/m3_compile.c
SRC += src/wasm3/m3_core.c
SRC += src/wasm3/m3_env.c
SRC += src/wasm3/m3_exec.c
SRC += src/wasm3/m3_function.c
SRC += src/wasm3/m3_info.c
SRC += src/wasm3/m3_module.c
SRC += src/wasm3/m3_parse.c

# List all user directories here
UINCDIR = src
UINCDIR += src/wasm3

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

