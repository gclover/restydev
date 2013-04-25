
--package.path = package.path .. ";" .. os.getenv("HOME") .. "/lua/?.lua";

local beanstalk = require('resty.beanstalkd')
local redis = require("resty.redis")
local conf = ngx.shared.config

local db = redis:new()
db:set_timeout(1000) 
local ok, err = db:connect(conf:get("redis_host"), conf:get("redis_port"))

local res, err = db:get("k1")
ngx.header.content_type = "text/html"
ngx.say(res)

db:set_keepalive(0, 10)

local queue = beanstalk:new()
local ok, err = queue:connect(conf:get("beandstalk_host"), conf:get("beanstalk_port"))
--local ok, err = queue:use("default")
local id, err = queue:put("job1")


--[[
local ok, err = instance:connect(host, port);
if not ok then
    ngx.log(ngx.ERR, err);
    ngx.exit(ngx.HTTP_SERVICE_UNAVAILABLE);
end

instance:set("name", "laowang");

local name = instance:get("name")
]]
