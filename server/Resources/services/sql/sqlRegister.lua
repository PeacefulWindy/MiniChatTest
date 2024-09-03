local SqlServ=require "sqlServ"

local _M=
{
    Client=
    {

    },
    Server=
    {
        login={func=SqlServ.login},
        register={func=SqlServ.register},
    }
}

return _M