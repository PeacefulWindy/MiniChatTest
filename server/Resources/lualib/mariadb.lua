local Class=require"class"

local _M=Class()

function _M:ctor()
    self.ptr=API.Mariadb_New()
end

function _M:destroy()
    API.Mariadb_Destroy(self.ptr)
end

function _M:connect(host,port,user,pwd,db)
    return API.Mariadb_Connect(self.ptr,host,port,user,pwd,db)
end

function _M:exec(sql,...)
    local args={...}

    local stmt=API.Mariadb_StmtNew(self.ptr)
    if not stmt then
        return
    end

    if not API.Mariadb_Prepare(stmt,sql) then
        return
    end

    local ret=API.Mariadb_Exec(stmt,args)

    API.Mariadb_StmtDestroy(stmt)
    return ret
end

return _M