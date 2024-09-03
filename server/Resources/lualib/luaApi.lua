LuaAPI=
{
    protocol=
    {
        Request=1,
        Response=2,
        Socket=3,
        Timer=4,
    },

    protocolFuncs={},
    coroSessions={},
    requestSessions={},
    coroSessionAutoId=1,
    sockets={},
}

function formatStringTableValue(datas)
    local tb={}
    for _,it in pairs(datas)do
        table.insert(tb,tostring(it))
    end

    return tb
end

function LuaAPI.dumpTable(tb,name,layer)
    if not name then
        name=""
    end

    if not layer then
        layer=0
    end

    if layer == 0 then
        LuaPrint("==========",name," Start==========")
    end

    for k,v in pairs(tb)do
        local spaceTb={}
        for i=1,layer do
            table.insert(spaceTb,"\t")
        end

        if type(v) == "table" then
            LuaPrint(name..table.concat(spaceTb," ").."("..type(k)..")"..k.."=>")
            LuaAPI.dumpTable(v,name,layer+1)
        else
            LuaPrint(name..table.concat(spaceTb," ").."("..type(k)..")"..k.."=("..type(v)..")"..tostring(v))
        end
    end

    if layer == 0 then
        LuaPrint("==========",name," End==========")
        if DEBUG_MODE then
            LuaPrint(debug.traceback())
        end
    end
end

function LuaAPI.debug(...)
    local args=formatStringTableValue({...})
    API.LogDebug(table.concat(args," "))
    if DEBUG_MODE then
        LuaPrint(debug.traceback())
    end
end

function LuaAPI.info(...)
    local args=formatStringTableValue({...})
    API.LogInfo(table.concat(args," "))
    if DEBUG_MODE then
        LuaPrint(debug.traceback())
    end
end

function LuaAPI.warning(...)
    local args=formatStringTableValue({...})
    API.LogWarning(table.concat(args," "))
    if DEBUG_MODE then
        LuaPrint(debug.traceback())
    end
end

function LuaAPI.error(...)
    local args=formatStringTableValue({...})
    API.LogError(table.concat(args," "))
    if DEBUG_MODE then
        LuaPrint(debug.traceback())
    end
end

LuaPrint=print
print=LuaAPI.info

function LuaAPI.call(nodeId,serviceId,cmd,data,callFunc)
    local sessionId=LuaAPI.coroSessionAutoId
    LuaAPI.coroSessionAutoId=LuaAPI.coroSessionAutoId+1
    local co=coroutine.create(function(...)
        callFunc(...)
    end)

    LuaAPI.coroSessions[sessionId]=co

    API.Service_Send(LuaAPI.protocol.Request,nodeId,serviceId,sessionId,cmd,#data,data)
end

function LuaAPI.ret(sessionId,cmd,data)
    if not LuaAPI.requestSessions[sessionId] then
        return
    end

    local info=LuaAPI.requestSessions[sessionId]
    LuaAPI.requestSessions[sessionId]=nil

    API.Service_Send(LuaAPI.protocol.Response,info.nodeId, info.serviceId, sessionId, cmd,#data,data)
end

function LuaAPI.registerProtocol(protocol,func)
    if not protocol or protocol == LuaAPI.protocol.Response then
        return
    end

    LuaAPI.protocolFuncs[protocol]=func
end

function LuaAPI.onMsg(protocol,fromNodeId,fromServiceId,sessionId,cmd,data)
    local sender=
    {
        nodeId=fromNodeId,
        serviceId=fromServiceId,
    }

    if protocol == LuaAPI.protocol.Response then
        if not sessionId or sessionId <= 0 then
            return
        end

        local co=LuaAPI.coroSessions[sessionId]
        if not co then
            return
        end

        local status,err=coroutine.resume(co,data)
        if not status then
            LuaAPI.error(err)
        end
    elseif protocol == LuaAPI.protocol.Request then
        if not LuaAPI.protocolFuncs[LuaAPI.protocol.Request] then
            print(SERVICE_NAME,"not register socket protocol!")
            return
        end

        if sessionId then
            LuaAPI.requestSessions[sessionId]=sender
        end
        
        local func=LuaAPI.protocolFuncs[LuaAPI.protocol.Request]
        local status,err = pcall(func,sessionId,cmd,data)
        if not status then
            LuaAPI.error(err)
        end
    elseif protocol == LuaAPI.protocol.Socket then
        if not sessionId then
            return
        end

        for _,it in pairs(LuaAPI.sockets)do
            if it:getStrId() == cmd and it.isAutoRead then
                local status,err = pcall(it.onRead,it,sessionId,data)
                if not status then
                    LuaAPI.error(err)
                end
                
                return
            end
        end

        if not LuaAPI.protocolFuncs[LuaAPI.protocol.Socket] then
            print(SERVICE_NAME,"not register socket protocol!")
            return
        end

        local func=LuaAPI.protocolFuncs[LuaAPI.protocol.Socket]
        local status,err = pcall(func,sessionId,cmd,data)
        if not status then
            LuaAPI.error(err)
        end
    elseif LuaAPI.protocolFuncs[protocol] then
        local func=LuaAPI.protocolFuncs[protocol]
        local status,err = pcall(func,sender,sessionId,cmd,data)
        if not status then
            LuaAPI.error(err)
        end
    end
end