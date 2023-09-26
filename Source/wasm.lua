wasm = {
	modules = {},
	load = function(self, path)
		local ret;
		for moduleName in string.gmatch(path, "([^.wasm]+)") do
			if self.modules[moduleName] then
				print("module already loaded: "..moduleName)
				return
			end
			local moduleTable = table.pack(c_load_module(path))
			table.remove(moduleTable, 1)
			self.modules[moduleName] = {}
			for i = 1, #moduleTable do
				local funcName = moduleTable[i]
				self.modules[moduleName][funcName] = function(i)
					return c_run_function(funcName, i)
				end
			end
			ret = self.modules[moduleName]
		end
		return ret
	end,
}
