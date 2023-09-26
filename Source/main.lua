import "CoreLibs/object"
import "CoreLibs/graphics"
import "wasm"

local gfx <const> = playdate.graphics
local index = 10
local result = 0
local time = 0
local module = wasm:load("fib32.wasm")

playdate.display.setScale(2)

function playdate.AButtonDown()
	gfx.drawTextAligned("Calculating...", 100, 120-20, kTextAlignment.center)
	playdate.display.flush()
	playdate.resetElapsedTime()
	result = module.fib(index)
	time = playdate.getElapsedTime()
end

function playdate.upButtonDown()
	index += 1
end

function playdate.downButtonDown()
	index = math.max(index - 1, 1)
end

function playdate.update()
	gfx.clear()
	gfx.drawText(index, 10, 10)
	gfx.drawTextAligned(time.."s", 200-10, 10, kTextAlignment.right)
	gfx.drawTextAligned(result, 100, 60, kTextAlignment.center)
end
