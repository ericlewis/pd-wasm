#include "pd_api.h"
#include "wasm3.h"
#include "m3_function.h"
#include "m3_env.h"

// Constants and Macros
#define WASM_STACK_SIZE 4096
#define FATAL(msg, ...) { pd->system->logToConsole("[FATAL] " msg, ##__VA_ARGS__); return 0; }

// Global variables
PlaydateAPI* pd;
M3Result result;
IM3Runtime runtime;
IM3Environment env;

// Initialize WASM environment and runtime
static int w_setup() {
	result = m3Err_none;
	env = m3_NewEnvironment();
	if (!env) FATAL("m3_NewEnvironment failed");

	runtime = m3_NewRuntime(env, WASM_STACK_SIZE, NULL);
	if (!runtime) FATAL("m3_NewRuntime failed");

	return 1;
}

// Load WASM module from file
static IM3Module w_load_module(const char* fname) {
	// Open and read file
	SDFile *file = pd->file->open(fname, kFileRead | kFileReadData);
	pd->file->seek(file, 0, SEEK_END);
	size_t fsize = pd->file->tell(file);
	pd->file->seek(file, 0, SEEK_SET);

	uint8_t *wasm = malloc(fsize);
	size_t len = sizeof(uint8_t) * fsize;

	if(pd->file->read(file, wasm, (unsigned int)len) != fsize) {
		free(wasm);
		pd->file->close(file);
		return 0;
	}

	pd->file->close(file);

	// Parse and load module
	IM3Module module;
	result = m3_ParseModule(env, &module, wasm, len);
	if (result) FATAL("m3_ParseModule: %s", result);

	result = m3_LoadModule(runtime, module);
	if (result) FATAL("m3_LoadModule: %s", result);

	return module;
}

// Call a WASM function with varying number of arguments
static int w_call_func(IM3Function f, int numOfArgs, int *args) {
	switch (numOfArgs) {
		case 2: return m3_CallV(f, args[0]);
		case 3: return m3_CallV(f, args[0], args[1]);
		case 4: return m3_CallV(f, args[0], args[1], args[2]);
		case 5: return m3_CallV(f, args[0], args[1], args[2], args[3]);
		case 6: return m3_CallV(f, args[0], args[1], args[2], args[3], args[4]);
		case 7: return m3_CallV(f, args[0], args[1], args[2], args[3], args[4], args[5]);
		case 8: return m3_CallV(f, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
		case 9: return m3_CallV(f, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
		case 10: return m3_CallV(f, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8]);
		case 11: return m3_CallV(f, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9]);
		default: return m3_CallV(f);
	}
}

// Run a WASM function from Lua
static int c_run_function(lua_State *L) {
	// Get number of arguments and function name
	int numOfArgs = pd->lua->getArgCount();
	const char *functionName = pd->lua->getArgString(1);

	IM3Function f;
	const char *result = m3_FindFunction(&f, runtime, functionName);
	if (result) FATAL("m3_FindFunction: %s", result);

	int *args = NULL;
	if (numOfArgs > 1) {
		args = malloc((numOfArgs - 1) * sizeof(int));
		for (int i = 2; i <= numOfArgs; i++) {
			if (pd->lua->getArgType(i, NULL) == kTypeInt) {
				args[i-2] = pd->lua->getArgInt(i);
			} else if (pd->lua->getArgType(i, NULL) == kTypeFloat) {
				args[i-2] = pd->lua->getArgFloat(i);
			} else {
				// ?
			}
		}
	}

	result = w_call_func(f, numOfArgs, args);
	free(args);
	if (result) FATAL("m3_Call: %s", result);

	unsigned value = 0;
	result = m3_GetResultsV(f, &value);
	if (result) FATAL("m3_GetResults: %s", result);

	u8 returnType = GetFunctionReturnType(f, 0);

	switch (returnType) {
		case 1:
			pd->lua->pushInt(value);
			return 1;
		case 2:
			pd->lua->pushFloat(*(float*)&value);
			return 1;
		default:
			FATAL("Unknown return type");
	}

	return 0;
}

static int c_load_module(lua_State *L) {
	const char* path = pd->lua->getArgString(1);
	IM3Module module = w_load_module(path);

	int resultCount = module->numFunctions;
	pd->lua->pushInt(resultCount);
	for (int i = 0; i < module->numFunctions; i++) {
		pd->lua->pushString(module->functions[i].export_name);
		resultCount++;
	}

	return resultCount;
}

int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg) {
	(void)arg;
	if (event == kEventInit) {
		pd = playdate;
		w_setup();
	} else if (event == kEventInitLua) {
		const char* err;
		if (!playdate->lua->addFunction(c_load_module, "c_load_module", &err)) {
			FATAL("Failed to register Lua function: %s", err);
		}
		if (!playdate->lua->addFunction(c_run_function, "c_run_function", &err)) {
			FATAL("Failed to register Lua function: %s", err);
		}
	}
	return 0;
}
