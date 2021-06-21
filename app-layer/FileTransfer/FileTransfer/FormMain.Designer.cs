
namespace FileTransfer
{
    partial class FormMain
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormMain));
            this.groupSend = new System.Windows.Forms.GroupBox();
            this.buttonSendPause = new System.Windows.Forms.Button();
            this.buttonSendAction = new System.Windows.Forms.Button();
            this.progressBarSend = new System.Windows.Forms.ProgressBar();
            this.labelSendProgress = new System.Windows.Forms.Label();
            this.buttonPickSend = new System.Windows.Forms.Button();
            this.textBoxPickSend = new System.Windows.Forms.TextBox();
            this.labelPickSend = new System.Windows.Forms.Label();
            this.textBoxSendACK = new System.Windows.Forms.TextBox();
            this.labelSendACK = new System.Windows.Forms.Label();
            this.groupRecv = new System.Windows.Forms.GroupBox();
            this.buttonRecvPause = new System.Windows.Forms.Button();
            this.textBoxRecvNotice = new System.Windows.Forms.TextBox();
            this.buttonRecvAction = new System.Windows.Forms.Button();
            this.labelRecvNotice = new System.Windows.Forms.Label();
            this.progressBarRecv = new System.Windows.Forms.ProgressBar();
            this.textBoxPickRecv = new System.Windows.Forms.TextBox();
            this.labelRecv = new System.Windows.Forms.Label();
            this.labelPickRecv = new System.Windows.Forms.Label();
            this.buttonPickRecv = new System.Windows.Forms.Button();
            this.groupInfo = new System.Windows.Forms.GroupBox();
            this.textBoxSending = new System.Windows.Forms.TextBox();
            this.textBoxPeerIP = new System.Windows.Forms.TextBox();
            this.textBoxRecving = new System.Windows.Forms.TextBox();
            this.textBoxMyIP = new System.Windows.Forms.TextBox();
            this.labelSending = new System.Windows.Forms.Label();
            this.labelPeerIP = new System.Windows.Forms.Label();
            this.labelRecving = new System.Windows.Forms.Label();
            this.labelIP = new System.Windows.Forms.Label();
            this.groupSend.SuspendLayout();
            this.groupRecv.SuspendLayout();
            this.groupInfo.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupSend
            // 
            this.groupSend.Controls.Add(this.buttonSendPause);
            this.groupSend.Controls.Add(this.buttonSendAction);
            this.groupSend.Controls.Add(this.progressBarSend);
            this.groupSend.Controls.Add(this.labelSendProgress);
            this.groupSend.Controls.Add(this.buttonPickSend);
            this.groupSend.Controls.Add(this.textBoxPickSend);
            this.groupSend.Controls.Add(this.labelPickSend);
            this.groupSend.Controls.Add(this.textBoxSendACK);
            this.groupSend.Controls.Add(this.labelSendACK);
            resources.ApplyResources(this.groupSend, "groupSend");
            this.groupSend.Name = "groupSend";
            this.groupSend.TabStop = false;
            // 
            // buttonSendPause
            // 
            resources.ApplyResources(this.buttonSendPause, "buttonSendPause");
            this.buttonSendPause.Name = "buttonSendPause";
            this.buttonSendPause.UseVisualStyleBackColor = true;
            this.buttonSendPause.Click += new System.EventHandler(this.buttonSendPause_Click);
            // 
            // buttonSendAction
            // 
            resources.ApplyResources(this.buttonSendAction, "buttonSendAction");
            this.buttonSendAction.Name = "buttonSendAction";
            this.buttonSendAction.UseVisualStyleBackColor = true;
            this.buttonSendAction.Click += new System.EventHandler(this.buttonSendAction_Click);
            // 
            // progressBarSend
            // 
            resources.ApplyResources(this.progressBarSend, "progressBarSend");
            this.progressBarSend.Name = "progressBarSend";
            // 
            // labelSendProgress
            // 
            resources.ApplyResources(this.labelSendProgress, "labelSendProgress");
            this.labelSendProgress.Name = "labelSendProgress";
            // 
            // buttonPickSend
            // 
            resources.ApplyResources(this.buttonPickSend, "buttonPickSend");
            this.buttonPickSend.Name = "buttonPickSend";
            this.buttonPickSend.UseVisualStyleBackColor = true;
            this.buttonPickSend.Click += new System.EventHandler(this.buttonPickSend_Click);
            // 
            // textBoxPickSend
            // 
            resources.ApplyResources(this.textBoxPickSend, "textBoxPickSend");
            this.textBoxPickSend.Name = "textBoxPickSend";
            // 
            // labelPickSend
            // 
            resources.ApplyResources(this.labelPickSend, "labelPickSend");
            this.labelPickSend.Name = "labelPickSend";
            // 
            // textBoxSendACK
            // 
            resources.ApplyResources(this.textBoxSendACK, "textBoxSendACK");
            this.textBoxSendACK.Name = "textBoxSendACK";
            // 
            // labelSendACK
            // 
            resources.ApplyResources(this.labelSendACK, "labelSendACK");
            this.labelSendACK.Name = "labelSendACK";
            // 
            // groupRecv
            // 
            this.groupRecv.Controls.Add(this.buttonRecvPause);
            this.groupRecv.Controls.Add(this.textBoxRecvNotice);
            this.groupRecv.Controls.Add(this.buttonRecvAction);
            this.groupRecv.Controls.Add(this.labelRecvNotice);
            this.groupRecv.Controls.Add(this.progressBarRecv);
            this.groupRecv.Controls.Add(this.textBoxPickRecv);
            this.groupRecv.Controls.Add(this.labelRecv);
            this.groupRecv.Controls.Add(this.labelPickRecv);
            this.groupRecv.Controls.Add(this.buttonPickRecv);
            resources.ApplyResources(this.groupRecv, "groupRecv");
            this.groupRecv.Name = "groupRecv";
            this.groupRecv.TabStop = false;
            // 
            // buttonRecvPause
            // 
            resources.ApplyResources(this.buttonRecvPause, "buttonRecvPause");
            this.buttonRecvPause.Name = "buttonRecvPause";
            this.buttonRecvPause.UseVisualStyleBackColor = true;
            this.buttonRecvPause.Click += new System.EventHandler(this.buttonRecvPause_Click);
            // 
            // textBoxRecvNotice
            // 
            resources.ApplyResources(this.textBoxRecvNotice, "textBoxRecvNotice");
            this.textBoxRecvNotice.Name = "textBoxRecvNotice";
            // 
            // buttonRecvAction
            // 
            resources.ApplyResources(this.buttonRecvAction, "buttonRecvAction");
            this.buttonRecvAction.Name = "buttonRecvAction";
            this.buttonRecvAction.UseVisualStyleBackColor = true;
            this.buttonRecvAction.Click += new System.EventHandler(this.buttonRecvAction_Click);
            // 
            // labelRecvNotice
            // 
            resources.ApplyResources(this.labelRecvNotice, "labelRecvNotice");
            this.labelRecvNotice.Name = "labelRecvNotice";
            // 
            // progressBarRecv
            // 
            resources.ApplyResources(this.progressBarRecv, "progressBarRecv");
            this.progressBarRecv.Name = "progressBarRecv";
            // 
            // textBoxPickRecv
            // 
            resources.ApplyResources(this.textBoxPickRecv, "textBoxPickRecv");
            this.textBoxPickRecv.Name = "textBoxPickRecv";
            // 
            // labelRecv
            // 
            resources.ApplyResources(this.labelRecv, "labelRecv");
            this.labelRecv.Name = "labelRecv";
            // 
            // labelPickRecv
            // 
            resources.ApplyResources(this.labelPickRecv, "labelPickRecv");
            this.labelPickRecv.Name = "labelPickRecv";
            // 
            // buttonPickRecv
            // 
            resources.ApplyResources(this.buttonPickRecv, "buttonPickRecv");
            this.buttonPickRecv.Name = "buttonPickRecv";
            this.buttonPickRecv.UseVisualStyleBackColor = true;
            this.buttonPickRecv.Click += new System.EventHandler(this.buttonPickRecv_Click);
            // 
            // groupInfo
            // 
            this.groupInfo.Controls.Add(this.textBoxSending);
            this.groupInfo.Controls.Add(this.textBoxPeerIP);
            this.groupInfo.Controls.Add(this.textBoxRecving);
            this.groupInfo.Controls.Add(this.textBoxMyIP);
            this.groupInfo.Controls.Add(this.labelSending);
            this.groupInfo.Controls.Add(this.labelPeerIP);
            this.groupInfo.Controls.Add(this.labelRecving);
            this.groupInfo.Controls.Add(this.labelIP);
            resources.ApplyResources(this.groupInfo, "groupInfo");
            this.groupInfo.Name = "groupInfo";
            this.groupInfo.TabStop = false;
            // 
            // textBoxSending
            // 
            resources.ApplyResources(this.textBoxSending, "textBoxSending");
            this.textBoxSending.Name = "textBoxSending";
            // 
            // textBoxPeerIP
            // 
            resources.ApplyResources(this.textBoxPeerIP, "textBoxPeerIP");
            this.textBoxPeerIP.Name = "textBoxPeerIP";
            // 
            // textBoxRecving
            // 
            resources.ApplyResources(this.textBoxRecving, "textBoxRecving");
            this.textBoxRecving.Name = "textBoxRecving";
            // 
            // textBoxMyIP
            // 
            resources.ApplyResources(this.textBoxMyIP, "textBoxMyIP");
            this.textBoxMyIP.Name = "textBoxMyIP";
            // 
            // labelSending
            // 
            resources.ApplyResources(this.labelSending, "labelSending");
            this.labelSending.Name = "labelSending";
            // 
            // labelPeerIP
            // 
            resources.ApplyResources(this.labelPeerIP, "labelPeerIP");
            this.labelPeerIP.Name = "labelPeerIP";
            // 
            // labelRecving
            // 
            resources.ApplyResources(this.labelRecving, "labelRecving");
            this.labelRecving.Name = "labelRecving";
            // 
            // labelIP
            // 
            resources.ApplyResources(this.labelIP, "labelIP");
            this.labelIP.Name = "labelIP";
            // 
            // FormMain
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupInfo);
            this.Controls.Add(this.groupRecv);
            this.Controls.Add(this.groupSend);
            this.Name = "FormMain";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.FormMain_FormClosed);
            this.groupSend.ResumeLayout(false);
            this.groupSend.PerformLayout();
            this.groupRecv.ResumeLayout(false);
            this.groupRecv.PerformLayout();
            this.groupInfo.ResumeLayout(false);
            this.groupInfo.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupSend;
        private System.Windows.Forms.GroupBox groupRecv;
        private System.Windows.Forms.GroupBox groupInfo;
        private System.Windows.Forms.Label labelRecvNotice;
        private System.Windows.Forms.Label labelIP;
        private System.Windows.Forms.Label labelRecving;
        private System.Windows.Forms.Label labelPeerIP;
        private System.Windows.Forms.TextBox textBoxRecving;
        private System.Windows.Forms.TextBox textBoxMyIP;
        private System.Windows.Forms.Label labelSending;
        private System.Windows.Forms.TextBox textBoxSending;
        private System.Windows.Forms.TextBox textBoxPeerIP;
        private System.Windows.Forms.Label labelSendACK;
        private System.Windows.Forms.TextBox textBoxSendACK;
        private System.Windows.Forms.TextBox textBoxRecvNotice;
        private System.Windows.Forms.TextBox textBoxPickSend;
        private System.Windows.Forms.Label labelPickSend;
        private System.Windows.Forms.Button buttonPickSend;
        private System.Windows.Forms.Label labelSendProgress;
        private System.Windows.Forms.ProgressBar progressBarSend;
        private System.Windows.Forms.Button buttonSendPause;
        private System.Windows.Forms.Button buttonSendAction;
        private System.Windows.Forms.Button buttonRecvPause;
        private System.Windows.Forms.Button buttonRecvAction;
        private System.Windows.Forms.ProgressBar progressBarRecv;
        private System.Windows.Forms.TextBox textBoxPickRecv;
        private System.Windows.Forms.Label labelRecv;
        private System.Windows.Forms.Label labelPickRecv;
        private System.Windows.Forms.Button buttonPickRecv;
    }
}

