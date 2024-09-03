local TcpServer=require "tcpServer"
local ChatRegister=require "ChatRegister"

function start()
    local tcpServer=TcpServer.new("127.0.0.1",60000)
    tcpServer:registerMsgId(ChatRegister.Client)
end

function stop()
    
end