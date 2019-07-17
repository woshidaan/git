using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Interop;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Threading;

namespace UKeyLauncher
{
	/// <summary>
	/// MainWindow.xaml 的交互逻辑
	/// </summary>

	public partial class MainWindow : Window
	{
#region GlobalDifinition
        [DllImport("WinLockDll.dll", EntryPoint = "Desktop_Show_Hide", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Desktop_Show_Hide(Boolean flag);

        [DllImport("WinLockDll.dll", EntryPoint = "TaskSwitching_Enable_Disable", CallingConvention = CallingConvention.Cdecl)]
        public static extern int TaskSwitching_Enable_Disable(Boolean flag);

        [DllImport("WinLockDll.dll", EntryPoint = "TaskManager_Enable_Disable", CallingConvention = CallingConvention.Cdecl)]
        public static extern int TaskManager_Enable_Disable(Boolean flag);

        [DllImport("UKeyLauncherDll.dll", EntryPoint = "Taskbar_Show_Hide", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Taskbar_Show_Hide();

        [DllImport("UKeyLauncherDll.dll", EntryPoint = "UKey_InitComComponents", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool UKey_InitComComponents();

        [DllImport("UKeyLauncherDll.dll", EntryPoint = "UKey_KoalLogin", CallingConvention = CallingConvention.Cdecl)]
        public static extern int UKey_KoalLogin(string strPswd);

        [DllImport("UKeyLauncherDll.dll", EntryPoint = "UKey_GetUserName", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr UKey_GetUserName();

        [DllImport("UKeyLauncherDll.dll", EntryPoint = "UKey_IsAdmin", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool UKey_IsAdmin(string strName);

        [DllImport("UKeyLauncherDll.dll", EntryPoint = "UKey_CTLogin", CallingConvention = CallingConvention.Cdecl)]
        public static extern int UKey_CTLogin(string strName);

        [DllImport("UKeyLauncherDll.dll", EntryPoint = "UKey_GetKeyStatus", CallingConvention = CallingConvention.Cdecl)]
        public static extern void UKey_GetKeyStatus();
        [DllImport("UKeyLauncherDll.dll", EntryPoint = "sendFailMessage", CallingConvention = CallingConvention.Cdecl)]
        public static extern void sendFailMessage(string strName);

        //int m_PswdCount = 3;    //密码重试次数,初始为3次
        //bool m_bUKey = true;   //UKey插入的标志位
        string m_UserName;      //记录使用UKey登录的用户名

        Log log = new Log();

        private KeyboardHookLib _keyboardHook = null;
        //private Loading dlg = new Loading(); 

#endregion
		public MainWindow()
		{
			this.InitializeComponent();

			// 在此点下面插入创建对象所需的代码。

            //全屏
            this.WindowState = System.Windows.WindowState.Normal;
            this.WindowStyle = System.Windows.WindowStyle.None;
            this.ResizeMode = System.Windows.ResizeMode.NoResize;
            //this.Topmost = true;

            this.Left = 0.0;
            this.Top = 0.0;
            this.Width = System.Windows.SystemParameters.PrimaryScreenWidth;
            this.Height = System.Windows.SystemParameters.PrimaryScreenHeight;
            
		}

        //private void Loading(object obj)
        //{
        //    if((int)obj == 0)
        //        dlg.Show();
        //    else
        //        dlg.Hide();
        //}

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            log.WriteLog("window_loaded begin");
            //添加消息相应钩子
            HwndSource hwndSource = PresentationSource.FromVisual(this) as HwndSource;
            if (null != hwndSource)
                hwndSource.AddHook(new HwndSourceHook(this.WndProc));

            // 隐藏桌面按钮
            Desktop_Show_Hide(false);
            log.WriteLog("desktop_hide");
            //禁用任务管理器、windows热键
            TaskSwitching_Enable_Disable(false);
            log.WriteLog("taskswitch_disable");
            TaskManager_Enable_Disable(false);
            log.WriteLog("taskmanager_disable");
            //Taskbar_Show_Hide();
            // log.WriteLog("taskbar_hide");
            //安装勾子
            _keyboardHook = new KeyboardHookLib();
            _keyboardHook.InstallHook(this.OnKeyPress);
            log.WriteLog("UKey_InitComComponents begin");
            //启动UKeyMonitor
            UKey_InitComComponents();
            log.WriteLog("UKey_InitComComponents end");
            //  System.Threading.Thread.Sleep(3);
            //  this.Window.Close();
            Thread thread = new Thread(UKey_GetKeyStatus);
            thread.Start();
            log.WriteLog("thread begin");
            //Thread loadingthread = new Thread(Loading);
            //loadingthread.Start();
        }

        protected virtual IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            switch (msg)
            {
                case 0x0401:    //UKey已经插入                  
                    break;
                case 0x0402:    //UKey已拔出,锁定计算机
                    //m_bUKey = false;
                    //禁用快捷键，任务管理器
                    
                    Desktop_Show_Hide(false);
                    TaskSwitching_Enable_Disable(false);
                    TaskManager_Enable_Disable(false);

                    Password.Clear();
                    Btn_Login.IsEnabled = true;
                    Btn_Unlock.IsEnabled = true;
                    Password.IsEnabled = true;

                    this.Window.Show();
                    
                    break;
                case 0x0403:    //Cttray已初始化完毕,隐藏界面
                    Password.Clear();
                    Btn_Login.IsEnabled = true;
                    Btn_Unlock.IsEnabled = true;
                    Password.IsEnabled = true;

                    this.Window.Hide();
                    break;
            }
            return IntPtr.Zero;
        }

        //private void login(object obj)
        //{
        //    Action action1 = () =>
        //    {
        //        dlg.Hide();
        //    };

        //    Action action2 = () =>
        //    {
        //        Password.Clear();
        //        Btn_Login.IsEnabled = true;
        //        Btn_Unlock.IsEnabled = true;
        //        Btn_Config.IsEnabled = true;
        //        Password.IsEnabled = true;            
        //    };

        //    Action action3 = () =>
        //    {
        //        this.Window.Hide();
        //    };

        //    Action<string> action4 = (strMes) =>
        //    {
        //        System.Windows.MessageBox.Show(strMes);  
        //    };

        //    if (!UKey_KoalLogin())
        //    {
        //        dlg.Dispatcher.BeginInvoke(action1);
        //        dlg.Dispatcher.BeginInvoke(action2);
        //        dlg.Dispatcher.BeginInvoke(action4, "格尔云网关登陆失败！");
        //        return;
        //    }

        //    m_UserName = Marshal.PtrToStringAnsi(UKey_GetUserName());

        //    if (0 == (int)obj)
        //    {
        //        if (0 != UKey_CTLogin(m_UserName))
        //        {
        //            dlg.Dispatcher.BeginInvoke(action1);
        //            dlg.Dispatcher.BeginInvoke(action2);
        //            dlg.Dispatcher.BeginInvoke(action4, "登录失败！");
        //            return;
        //        }
        //        dlg.Dispatcher.BeginInvoke(action1);
        //        return;
        //    }
        //    else 
        //    {
        //        if (!UKey_IsAdmin(m_UserName))
        //        {
        //            dlg.Dispatcher.BeginInvoke(action1);
        //            dlg.Dispatcher.BeginInvoke(action2);

        //            dlg.Dispatcher.BeginInvoke(action4,"您不是管理员,不能解锁!");             
        //            return;
        //        }

        //        //显示桌面
        //        //Desktop_Show_Hide(true);
        //        //启用快捷键、任务管理器
        //        TaskSwitching_Enable_Disable(true);
        //        TaskManager_Enable_Disable(true);

        //        //取消勾子
        //        //if (_keyboardHook != null) _keyboardHook.UninstallHook();

        //        dlg.Dispatcher.BeginInvoke(action1);
        //        dlg.Dispatcher.BeginInvoke(action3);
        //        return;
        //    }
        //}

        private void Login(object obj)
        {
            if ("" == Password.Password)
            {
                System.Windows.MessageBox.Show("PIN码不能为空！");
                return;
            }

            //dlg.Show();
            //启动HDPClient.exe
            int res = UKey_KoalLogin(Password.Password);
            Password.Clear();

            m_UserName = Marshal.PtrToStringAnsi(UKey_GetUserName());
            switch (res)
            {
                case 0:
                    //Thread t = new Thread(new ParameterizedThreadStart(login));                   
                    //t.Start((int)obj);                    
                    //dlg.Show();

                    Btn_Login.IsEnabled = false;
                    Btn_Unlock.IsEnabled = false;
                    Password.IsEnabled = false;  


                    if (0 == (int)obj)
                    {
                        if (0 != UKey_CTLogin(m_UserName))
                        {
                            System.Windows.MessageBox.Show("登陆失败！");
                            //dlg.Hide();
                            return;
                        }
                    }
                    else
                    {
                        if (!UKey_IsAdmin(m_UserName))
                        {
                            System.Windows.MessageBox.Show("您不是管理员,不能解锁!");
                            Password.Clear();
                            Btn_Login.IsEnabled = true;
                            Btn_Unlock.IsEnabled = true;
                            Password.IsEnabled = true;
                            //dlg.Hide();
                            return;
                        }

                        //启用快捷键、任务管理器
                        TaskSwitching_Enable_Disable(true);
                        TaskManager_Enable_Disable(true);
                        Desktop_Show_Hide(true);
                        //取消勾子
                        if (_keyboardHook != null) _keyboardHook.UninstallHook();
                        this.Window.Hide();
                    }
                    //Thread.Sleep(3000);
                    break;
                case 1:
                    //dlg.Hide();
                    log.WriteLog("send failed message");
                    sendFailMessage(m_UserName);
                    System.Windows.MessageBox.Show("PIN码错误,请重新输入，若输错超过3次UKEY可能被锁定！");
                    break;

                case 2:
                    //dlg.Hide();
                    System.Windows.MessageBox.Show("UKEY未插入，请插入UKEY！");
                    break;

                case 3:
                    //dlg.Hide();
                    System.Windows.MessageBox.Show("未知错误，请联系管理员！");
                    break;

                case 4:
                    //dlg.Hide();
                    System.Windows.MessageBox.Show("格尔云网关登陆失败！");
                    break;

                //case 5:
                //    System.Windows.MessageBox.Show("登陆失败！");
                //    break;

                default:
                    //dlg.Hide();
                    System.Windows.MessageBox.Show("未知错误，请联系管理员！");
                    break;
            }
            //dlg.Hide();
            return;
        }

        private void Btn_Login_Click(object sender, RoutedEventArgs e)
        {
            Login(0);
        }      

        private void Btn_Unlock_Click(object sender, RoutedEventArgs e)
        {
            Login(1);               
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            TaskSwitching_Enable_Disable(true);
            TaskManager_Enable_Disable(true);
            Desktop_Show_Hide(true);
        }

        public void OnKeyPress(KeyboardHookLib.HookStruct hookStruct, out bool handle)
        {
            handle = false; //预设不拦截任何键

            //截获alt+f4 
            if (hookStruct.vkCode == (int)Keys.F4 && (int)System.Windows.Forms.Control.ModifierKeys == (int)Keys.Alt)
            {
                handle = true;
            }

        }
	}
}



