local _M={}

_M.LoginCode=
{
    Ok=1,
    ErrorUserOrPwd=2,
    Banner=3,
}

_M.LogoutCode=
{
    Ok=1,
    Repeat=2,
    Banner=3,
}

_M.RegisterCode=
{
    Ok=1,
    Exist=2,
    Failed=3,
}

return _M