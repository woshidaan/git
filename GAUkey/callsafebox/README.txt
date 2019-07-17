1.查看虚拟机是否安装了Microsoft .NET Framework 4 ，如果没有安装，安装包为dotNetFx40_Full_x86_x64.exe
2.以管理员身份运行cmd，调出“命令提示符”窗口，调用regsvr32.exe SecuMail.ocx将SecuMail.ocx进行注册
如：regsvr32.exe C:\SecuMail.ocx,注意一定得是管理员身份打开cmd
3.运行callukey程序，运行之后没有任何反应，可以在任务管理器中查看进程是否启动
