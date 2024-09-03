local SqlRegister =require "sqlRegister"
local mariadb=require "mariadb"

MYSQL=nil

function start()
    MYSQL=mariadb.new()
    if not MYSQL:connect("127.0.0.1",3306,"root","123456","chat") then
        return
    end

    LuaAPI.registerProtocol(LuaAPI.protocol.Request,function(sessionId,cmd,data)
        if not SqlRegister.Server or not SqlRegister.Server[cmd] then
            LuaAPI.error(cmd,"not register service cmd:",cmd)
            LuaAPI.ret(sessionId,cmd,API.Json_Encode({}))
            return
        end

        local info=SqlRegister.Server[cmd]
        if not info.func then
            LuaAPI.error(cmd,"not register service cmd func",cmd)
            LuaAPI.ret(sessionId,cmd,API.Json_Encode({}))
            return
        end

        local status,err=pcall(info.func,sessionId,API.Json_Decode(data))
        if not status then
            LuaAPI.error(err)
            LuaAPI.ret(sessionId,cmd,API.Json_Encode({}))
            return
        end
    end)
end

function stop()
    if MYSQL then
        MYSQL.destroy()
        MYSQL=nil
    end
end