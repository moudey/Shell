using System;
using System.Windows.Forms;

namespace Nilesoft
{
    static class Program
    {
        /// <summary>
        ///  The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            Application.SetHighDpiMode(HighDpiMode.SystemAware);
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            
            if(args.Length == 0)
				Application.Run(new frmGroups());
			else
			{	
				if(args[0] == "-compact")
					Application.Run(new frmMain());
				else //if(args[0] == "-full")
					Application.Run(new frmGroups());
			}
        }
    }
}
