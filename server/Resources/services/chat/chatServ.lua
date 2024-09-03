local _M={}
local LoginDefine=require "LoginDefine"

local _P=
{
    onlineClients={}
}

function _M.login(socket,fd,reqData)
    if not reqData.user or not reqData.pwd then
        return
    end

    local sqlServiceId=API.Service_GetService("sql")
    LuaAPI.call(0,sqlServiceId,"login",API.Json_Encode(reqData),function(retData)
        local data=API.Json_Decode(retData)
        assert(data.code and data.code > 0)

        local msg=
        {
            msgId=10000,
            data=
            {
                code=data.code
            }
        }

        socket:send(fd,API.Json_Encode(msg))

        if msg.data.code == LoginDefine.LoginCode.Ok then
            if _P.onlineClients[reqData.user] then
                local outMsg=
                {
                    msgId=10001,
                    data
                    {
                        code=_P.LogoutCode.Repeat
                    },
                }
                socket:send(_P.onlineClients[reqData.user].fd,API.Json_Encode(outMsg))
            end
    
            _P.onlineClients[fd]=
            {
                user=reqData.user,
            }
        end
    end)
end

function _M.logout(socket,fd,reqData)
    if not _P.onlineClients[fd] then
        return
    end

    local msg=
    {
        msgId=10001,
        data=
        {
            code=LoginDefine.LogoutCode.Ok
        },
    }

    socket:send(fd,API.Json_Encode(msg))
    socket:closeFd(fd)
    _P.onlineClients[fd]=nil
end

function _M.register(socket,fd,reqData)
    if not reqData.user or not reqData.pwd then
        return
    end

    local sqlServiceId=API.Service_GetService("sql")
    LuaAPI.call(0,sqlServiceId,"register",API.Json_Encode(reqData),function(retData)
        local data=API.Json_Decode(retData)
        assert(data.code and data.code > 0)

        local msg=
        {
            msgId=10002,
            data=
            {
                code=data.code
            }
        }

        socket:send(fd,API.Json_Encode(msg))
        socket:closeFd(fd)
    end)
end

function _M.sendMsg(socket,fd,reqData)
    if not reqData.msg and #reqData.msg <= 0 then
        return
    end

    local msg=
    {
        msgId=10010,
        data=
        {
            time=os.time(),
            user=_P.onlineClients[fd].user,
            msg=reqData.msg,
        }
    }

    for anotherClientFd,_ in pairs(_P.onlineClients)do
        socket:send(anotherClientFd,API.Json_Encode(msg))
    end
end

return _M