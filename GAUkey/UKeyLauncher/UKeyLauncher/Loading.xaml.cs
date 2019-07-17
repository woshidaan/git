using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace UKeyLauncher
{
    /// <summary>
    /// Loading.xaml 的交互逻辑
    /// </summary>
    public partial class Loading : Window
    {
        private GifImage gifImage; 
        public Loading()
        {
            InitializeComponent();

            this.gifImage = new GifImage("loading.gif");
            this.gifImage.Width = 100;
            this.gifImage.Height = 100;
            this.Content = this.gifImage;
            this.gifImage.StartAnimate();
        }
    }
}
