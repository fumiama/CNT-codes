using System;
using System.Threading;
using System.Windows.Forms;

namespace FileTransfer
{
    public partial class FormMain : Form
    {
        private bool run = true;
        //参数: myip
        public FormMain(string[] args)
        {
            InitializeComponent();
            new Thread(new ThreadStart(PipeListener)).Start();
            textBoxMyIP.Text = args[0];
        }

        //progress-send:进度
        //progress-recv:进度
        //action-recv:对方IP
        //ack-send/fin-send
        //ack-recv/fin-recv
        private void PipeListener()
        {
            while(run)
            {
                string line = Console.ReadLine();
                if(line.StartsWith("progress-"))
                {
                    int p = int.Parse(line.Substring(line.IndexOf(':') + 1));
                    if (line.Substring(line.IndexOf('-') + 1).StartsWith("send")) progressBarSend.Value = p;
                    else progressBarRecv.Value = p;
                } else if(line.StartsWith("action-recv"))
                {
                    textBoxPeerIP.Text = line.Substring(line.IndexOf(':') + 1);
                    textBoxRecvNotice.Text = "是";
                    buttonRecvAction.Enabled = true;
                    buttonRecvPause.Enabled = true;
                } else if (line == "ack-send")
                {
                    textBoxSendACK.Text = "否";
                    textBoxSending.Text = "是";
                    buttonSendPause.Enabled = true;
                } else if(line == "ack-recv")
                {
                    textBoxRecvNotice.Text = "否";
                    textBoxRecving.Text = "是";
                } else if(line == "fin-send")
                {
                    buttonSendPause.Enabled = false;
                    textBoxSending.Text = "否";
                    progressBarSend.Value = 0;
                } else if (line == "fin-recv")
                {
                    buttonRecvAction.Enabled = false;
                    buttonRecvPause.Enabled = false;
                    textBoxRecving.Text = "否";
                    progressBarRecv.Value = 0;
                }
            }
        }

        private void buttonPickSend_Click(object sender, EventArgs e)
        {
            //选择文件
            OpenFileDialog dialog = new OpenFileDialog();
            dialog.Multiselect = false;//该值确定是否可以选择多个文件
            dialog.Title = "请选择文件";
            dialog.Filter = "所有文件(*.*)|*.*";
            if (dialog.ShowDialog() == DialogResult.OK) textBoxPickSend.Text = dialog.FileName;
        }

        private void buttonPickRecv_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog dialog = new FolderBrowserDialog();
            if (dialog.ShowDialog() == DialogResult.OK) textBoxPickRecv.Text = dialog.SelectedPath;
        }

        private void FormMain_FormClosed(object sender, FormClosedEventArgs e)
        {
            run = false;
        }

        private void buttonSendAction_Click(object sender, EventArgs e)
        {
            if (textBoxPickSend.Text == "") MessageBox.Show("文件为空", "提示");
            else
            {
                Console.WriteLine("action-send:" + textBoxPickSend.Text + "-->" + textBoxPeerIP.Text);
                textBoxSendACK.Text = "是";
            }
        }

        private void buttonSendPause_Click(object sender, EventArgs e)
        {
            Console.WriteLine("pause-send");
            buttonSendPause.Enabled = false;
        }

        private void buttonRecvPause_Click(object sender, EventArgs e)
        {
            Console.WriteLine("pause-recv");
            buttonRecvPause.Enabled = false;
        }

        private void buttonRecvAction_Click(object sender, EventArgs e)
        {
            if (textBoxPickRecv.Text == "") MessageBox.Show("文件为空", "提示");
            else
            {
                Console.WriteLine("action-recv:" + textBoxPickRecv.Text);
                textBoxRecvNotice.Text = "否";
            }
        }
    }
}
