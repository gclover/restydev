
package.path = package.path .. ";" .. os.getenv("HOME") .. "/lua/?.lua"

local cjson = require "cjson"
local file = io.open(os.getenv("HOME") .. "/conf/backend.json", "r")
local content = cjson.decode(file:read("*all"))
file.close()

local config = ngx.shared.config
for name, value in pairs(content) do
	config:set(name, value)
end
