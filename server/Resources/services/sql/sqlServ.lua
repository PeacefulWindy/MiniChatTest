local _M={}
local LoginDefine=require "LoginDefine"

function _M.login(sessionId,reqData)
    local msg=
    {
        code=-1
    }

    if reqData.user and reqData.pwd then
        local sqlData=MYSQL:exec("select pwd from account where user=? limit 1;",reqData.user)

        if not sqlData or not next(sqlData) or sqlData[1].pwd ~= reqData.pwd then
            msg.code=LoginDefine.LoginCode.ErrorUserOrPwd
        else
            msg.code=LoginDefine.LoginCode.Ok
        end
    end

    LuaAPI.ret(sessionId,"login",API.Json_Encode(msg))
end

function _M.register(sessionId,reqData)
    local msg=
    {
        code=-1,
    }

    if reqData.user and reqData.pwd then
        local sqlData=MYSQL:exec("select id from account where user=? limit 1;",reqData.user)

        if sqlData and next(sqlData) then
            msg.code=LoginDefine.RegisterCode.Exist
        else
            local ret=MYSQL:exec("insert into account(user,pwd) values(?,?)",reqData.user,reqData.pwd)
            if not ret then
                msg.code=LoginDefine.RegisterCode.Failed
            else
                msg.code=LoginDefine.RegisterCode.Ok
            end
        end
    end

    LuaAPI.ret(sessionId,"register",API.Json_Encode(msg))
end

return _M