using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SECUMAILLib;
using System.Threading;
using System.Net.Sockets;
using System.Net;
using System.IO;

namespace callsafebox
{
    public class GetOSystem
    {
        private const string Windows2000 = "5.0";
        private const string WindowsXP = "5.1";
        private const string Windows2003 = "5.2";
        private const string Windows2008 = "6.0";
        private const string Windows7 = "6.1";
        private const string Windows8OrWindows81 = "6.2";
        private const string Windows10 = "10.0";

        private string OSystemName;

        public void setOSystemName(string oSystemName)
        {
            this.OSystemName = oSystemName;
        }

        public string getOSystemName()
        {
            return this.OSystemName;
        }

        public  GetOSystem()
        {
            switch (System.Environment.OSVersion.Version.Major + "." + System.Environment.OSVersion.Version.Minor)
            {
                case Windows2000:
                    setOSystemName("Windows2000");
                    break;
                case WindowsXP:
                    setOSystemName("WindowsXP");
                    break;
                case Windows2003:
                    setOSystemName("Windows2003");
                    break;
                case Windows2008:
                    setOSystemName("Windows2008");
                    break;
                case Windows7:
                    setOSystemName("Windows7");
                    break;
                case Windows8OrWindows81:
                    setOSystemName("Windows8.OrWindows8.1");
                    break;
                case Windows10:
                    setOSystemName("Windows10");
                    break;
            }
        }
    }


    class Program
    {
        private static byte[] result = new byte[1024]; 
        private static int myProt = 60000;  //端口 
        static Socket serverSocket;
        static SecuMail ss = new SecuMail();
        static GetOSystem os = new GetOSystem();
        static string OSystemName = os.getOSystemName();//获取虚拟机系统版本
        

        static void Main(string[] args) 
        { 
          //服务器IP地址 
          //IPAddress ip = IPAddress.Parse("127.0.0.1");//获取本机ip 
          serverSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
          serverSocket.Bind(new IPEndPoint(IPAddress.Any, myProt)); //绑定IP地址：端口 
          serverSocket.Listen(5);  //设定最多10个排队连接请求 
          Console.WriteLine("启动监听{0}成功", serverSocket.LocalEndPoint.ToString()); 
          //通过Clientsoket发送数据 
          Thread myThread = new Thread(ListenClientConnect); 
          myThread.Start(); 
        }

        /// <summary> 
        /// 监听客户端连接 
        /// </summary> 
        private static void ListenClientConnect()
        {
            while (true)
            {
                try
                {
                    Socket clientSocket = serverSocket.Accept();
                    //clientSocket.Send(Encoding.ASCII.GetBytes("Server Say Hello"));
                    Thread receiveThread = new Thread(ReceiveMessage);
                    receiveThread.Start(clientSocket);
                }
                catch(Exception e)
                {
                    WriteLog(e.Message + "listen");
                }
            }
        }

        /// <summary>
        /// 关闭进程
        /// </summary>
        /// <param name="processName">进程名</param>
        private static void KillProcess(string processName)
        {
            //获得进程对象，以用来操作  
            System.Diagnostics.Process myproc = new System.Diagnostics.Process();
            //得到所有打开的进程   
            try
            {
                //获得需要杀死的进程名  
                foreach (System.Diagnostics.Process thisproc in System.Diagnostics.Process.GetProcessesByName(processName))
                {
                    //立即杀死进程   www.2cto.com
                    thisproc.Kill();
                }
            }
            catch (Exception Exc)
            {
                throw new Exception("", Exc);
            }
        }
        public static void WriteLog(string str)//写日志
        {

            StreamWriter stream;
            string path = "C:\\Users\\Public\\CloudTerm";
            string content = "";
            if (!Directory.Exists(path))
            {
                Directory.CreateDirectory(path);
            }
            stream = new StreamWriter(path + "\\callsafebox.log", true, Encoding.Default);
            content = GetLocalTime() + ":" + str;
            stream.Write(content);
            stream.Write("\r\n");
            stream.Flush();
            stream.Close();
        }
        public static string GetLocalTime()//获取系统当前时间
        {
            string currentTime = DateTime.Now.ToLocalTime().ToString("yyyy-MM-dd HH:mm:ss");
            return currentTime;
        }
        /// <summary> 
        /// 接收消息 
        /// </summary> 
        /// <param name="clientSocket"></param> 
        private static void ReceiveMessage(object clientSocket)
        {
            
            ss.Initialize(0);
            Socket myClientSocket = (Socket)clientSocket;
            while (true)
            {
                try
                {
                    int receiveNumber = myClientSocket.Receive(result);
                    int ret = ss.HasOnlineToken();
                    if (ret == 0)
                    {
                        if (OSystemName == "Windows7")  //判断系统是win7还是xp
                        {
                            KillProcess("KLTknWs");
                        }
                        else if (OSystemName == "WindowsXP")
                        {
                            KillProcess("KLTknSvr");
                        }
                        Console.WriteLine("offline");
                        WriteLog("offline");
                        myClientSocket.Send(Encoding.ASCII.GetBytes("offline"));
                    }
                    else
                    {
                        Console.WriteLine("online");
                        WriteLog("online");
                        myClientSocket.Send(Encoding.ASCII.GetBytes("online"));
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                    WriteLog(ex.Message + "receive");
                    myClientSocket.Shutdown(SocketShutdown.Both);//为发送和接收禁用socket
                    myClientSocket.Close();
                    break;
                }
            }
        } 
    }
}



