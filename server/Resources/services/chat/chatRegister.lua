local ChatServ=require "chatServ"

local _M=
{
    Client=
    {
        [10000]={func=ChatServ.login},
        [10001]={func=ChatServ.logout},
        [10002]={func=ChatServ.register},
        [10010]={func=ChatServ.sendMsg},
    },
    Server=
    {
        
    }
}

return _M