using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace UKeyLauncher
{
    class Log
    {
        public void WriteLog(string text)
        {
            string strLogPath = "C:\\Users\\Public\\CloudTerm\\UkeyLauncher1.log";
            if (!File.Exists(strLogPath))
            {
                FileStream fs1 = new FileStream(strLogPath, FileMode.Create, FileAccess.Write);//创建写入文件
                StreamWriter sw1 = new StreamWriter(fs1, Encoding.Default);
                sw1.WriteLine("'" + DateTime.Now.ToString() + text + "'");
                sw1.Close();
                fs1.Close();
            }
            else
            {
                FileStream fs = new FileStream(strLogPath, FileMode.Append);
                StreamWriter sw = new StreamWriter(fs, Encoding.Default);
                sw.WriteLine("'" + DateTime.Now.ToString() + text + "'");
                sw.Close();
                fs.Close();
            }

        }
    }
}
