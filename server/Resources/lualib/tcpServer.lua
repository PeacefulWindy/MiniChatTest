local Class=require "class"
local _M=Class()

function _M:ctor(host,port)
    self.ptr=API.TcpServer_New(host,port)
    LuaAPI.sockets[self.ptr]=self
    self.isAutoRead=true

    self.msgMap={}
end

function _M:destroy()
    LuaAPI.sockets[self.ptr]=nil
    API.TcpServer_Destroy(self.ptr)
end

function _M:getStrId()
    return tostring(self.ptr)
end

function _M:onRead(fd,rawData)
    local data=API.Json_Decode(rawData)
    if not data or not data.msgId or not data.data then
        local ret=
        {
            err="Invalid Request"
        }

        self:send(fd,API.Json_Encode(ret))
    end

    if not self.msgMap[data.msgId] then
        LuaAPI.warning(SERVICE_NAME,"not register msg id:",data.msgId)
        self:send(fd,API.Json_Encode(errRet))
        return
    end

    local func=self.msgMap[data.msgId].func
    local status,err=pcall(func,self,fd,data.data)
    if not status then
        LuaAPI.error(err)
    end
end

function _M:registerMsgId(msgIdMap)
    if not msgIdMap or type(msgIdMap) ~= "table" then
        return
    end

    for k,v in pairs(msgIdMap)do
        if type(k) ~= "number" then
            LuaAPI.warning("invalid register msg id:",k)
            return
        end

        if not v or type(v) ~= "table" or not v.func or type(v.func) ~= "function" then
            LuaAPI.warning("invalid register msg id func:",k)
            return
        end

        self.msgMap[k]=v
    end
end

function _M:send(fd,data)
    API.TcpServer_Send(self.ptr,fd,#data,data)
end

function _M:closeFd(fd)
    API.TcpServer_CloseFd(self.ptr,fd)
end

return _M