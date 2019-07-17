using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Reflection;
using System.Diagnostics;
using Microsoft.Win32;
//using System.Windows.Forms;


namespace UKeyLauncher
{

    /// <summary>
    /// ����Hook������, by www.CSFramework.com 
    /// </summary>
    public class KeyboardHookLib
    {
        private const int WH_KEYBOARD_LL = 13; //����

        //���̴����¼�ί�� ,�������������ʱ���ö����ί�еķ���.
        private delegate int HookHandle(int nCode, int wParam, IntPtr lParam);

        //�ͻ��˼��̴����¼�
        public delegate void ProcessKeyHandle(HookStruct param, out bool handle);

        //����SetWindowsHookEx����ֵ
        private static int _hHookValue = 0;

        //���ӳ������¼�
        private HookHandle _KeyBoardHookProcedure; 

        //Hook�ṹ
        [StructLayout(LayoutKind.Sequential)]
        public class HookStruct
        {
            public int vkCode;
            public int scanCode;
            public int flags;
            public int time;
            public int dwExtraInfo;
        }

        //���ù��� 
        [DllImport("user32.dll")]
        private static extern int SetWindowsHookEx(int idHook, HookHandle lpfn, IntPtr hInstance, int threadId);

        //ȡ������ 
        [DllImport("user32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.StdCall)]
        private static extern bool UnhookWindowsHookEx(int idHook);

        //������һ������ 
        [DllImport("user32.dll")]
        private static extern int CallNextHookEx(int idHook, int nCode, int wParam, IntPtr lParam);

        //��ȡ��ǰ�߳�ID
        [DllImport("kernel32.dll")]
        private static extern int GetCurrentThreadId();

        //Gets the main module for the associated process.
        [DllImport("kernel32.dll")]
        private static extern IntPtr GetModuleHandle(string name);

        private IntPtr _hookWindowPtr = IntPtr.Zero;

        //������
        public KeyboardHookLib() { }

        //�ⲿ���õļ��̴����¼�
        private static ProcessKeyHandle _clientMethod = null;

        /// <summary>
        /// ��װ����
        /// </summary>
        /// <param name="hookProcess">�ⲿ���õļ��̴����¼�</param>
        public void InstallHook(ProcessKeyHandle clientMethod)
        {
            _clientMethod = clientMethod;

            // ��װ���̹��� 
            if (_hHookValue == 0)
            {
                _KeyBoardHookProcedure = new HookHandle(OnHookProc);

                _hookWindowPtr = GetModuleHandle(Process.GetCurrentProcess().MainModule.ModuleName);

                _hHookValue = SetWindowsHookEx(
                    WH_KEYBOARD_LL,
                    _KeyBoardHookProcedure,
                    _hookWindowPtr,
                    0);

                //������ù���ʧ��. 
                if (_hHookValue == 0) UninstallHook();
            }
        }

        //ȡ�������¼� 
        public void UninstallHook()
        {
            if (_hHookValue != 0)
            {
                bool ret = UnhookWindowsHookEx(_hHookValue);
                if (ret) _hHookValue = 0;
            }
        }

        //�����¼��ڲ�����,����_clientMethod����ת�����ͻ���Ӧ�á�
        private static int OnHookProc(int nCode, int wParam, IntPtr lParam)
        {
            if (nCode >= 0)
            {
                //ת���ṹ
                HookStruct hookStruct = (HookStruct)Marshal.PtrToStructure(lParam, typeof(HookStruct));

                if (_clientMethod != null)
                {
                    bool handle = false;
                    //���ÿͻ��ṩ���¼��������
                    _clientMethod(hookStruct, out handle);
                    if (handle) return 1; //1:��ʾ���ؼ���,return �˳�
                }
            }
            return CallNextHookEx(_hHookValue, nCode, wParam, lParam);
        }

    }
}
