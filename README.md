# MiniChatTest

这是一个多人聊天的测试项目。

***

客户端基于Qt6.7.2构建（好几年没用Qt了）

***

服务端基于VS2022+vcpkg构建，使用的库为：
- lua
- libhv
- cjson
- spdlog
- libmariadb

***

Windows服务端部署方式（server目录)

1. 打开cmd，执行下面命令(powershell执行bat需手动加上.\)
```
createResourcesLink.bat（可能需要管理员权限）
cd vcpkg
bootstrap-vcpkg.bat
```

2. 回到server目录，右键使用Visual Studio打开，并编译（vpckg会自动下载第三方库，如果下载不了请使用代理）

3. 在Mariadb里新建utf8编码的chat数据库，并导入chat.sql

4. 运行server.exe

Linux没测试，理论上类似Windows的编译方式（并没有使用过多的平台相关的代码）

***

本测试项目局限性比较大，例如：
- 聊天内容没有加密（容易被截取数据）
- 密码明文保存（没去用crypto库）
- 没有更复杂的功能（单纯测试登陆注册和多人聊天）

***

禁止任何人将仓库以及代码转到以下平台：
* Gitee
* 码云（几个月前批量复制github仓库）
* 其它平台（需授权）

***

仅供学习交流，禁止商业用途。
