using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace Nilesoft
{
	public class frmGroups : Form
	{
		private IContainer components = null;
		private Button btn_hash;
		private TextBox txt_input;
		private TextBox txt_result;
		private TextBox txt_format;
		private CheckBox chkUperCase;
		private CheckBox chkIdent;

		public frmGroups()
		{
			InitializeComponent();
		}

		private void result(string[] a)
		{
			try
			{
				string result = "";
				foreach (string text in a)
				{
					if (!string.IsNullOrWhiteSpace(text))
					{
						string fmt = txt_format.Text.Trim();
						if (fmt.Length > 0)
							fmt = !chkUperCase.Checked ? (fmt + text) : (fmt.ToUpper() + text.ToUpper());
						else
							fmt = fmt = ((!chkUperCase.Checked) ? text : text.ToUpper());

						fmt = fmt.Replace(' ', '_').Replace('-', '_');

						result += $"constexpr auto {fmt} = {Hash.ToString(text.Trim())};\r\n";
					}
				}

				txt_result.Text = result;
			}
			catch
			{
				txt_input.Focus();
			}
		}

		private void btn_hash_Click(object sender, EventArgs e)
		{
			txt_result.Clear();
			result(txt_input.Lines);
		}

		private void txt_input_TextChanged(object sender, EventArgs e)
		{
			if (!string.IsNullOrEmpty(txt_result.Text))
			{
				txt_result.Clear();
			}
			if (!string.IsNullOrEmpty(txt_input.Text))
			{
				try
				{
					result(txt_input.Lines);
				}
				catch
				{
					txt_input.Focus();
				}
			}
		}
		private void txt_format_TextChanged(object sender, EventArgs e)
		{
			if (!string.IsNullOrEmpty(txt_result.Text))
			{
				txt_result.Clear();
			}

			if (!string.IsNullOrEmpty(txt_format.Text))
			{
				try
				{
					result(txt_input.Lines);
				}
				catch
				{
					txt_format.Focus();
				}
			}
		}

		private void chkIdent_CheckStateChanged(object sender, EventArgs e)
		{
			if(chkIdent.Checked)
			{
				if(!txt_format.Text.StartsWith("IDENT_", StringComparison.OrdinalIgnoreCase))
				{
					txt_format.Text = "IDENT_" + txt_format.Text;
				}
			}
			else
			{
				if(txt_format.Text.StartsWith("IDENT_", StringComparison.OrdinalIgnoreCase))
				{
					txt_format.Text = txt_format.Text.Remove(0, 6);
				}
			}
		}

		protected override void Dispose(bool disposing)
		{
			if (disposing && components != null)
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		private void InitializeComponent()
		{
			base.SuspendLayout();
			btn_hash = new Button()
			{
				Anchor = (AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right),
				Location = new Point(12, 349),
				Name = "btn_hash",
				Size = new Size(715, 36),
				TabIndex = 1,
				Text = "Hash",
				UseVisualStyleBackColor = true
			};

			btn_hash.Click += btn_hash_Click;

			txt_input = new TextBox()
			{
				Anchor = (AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right),
				Location = new Point(12, 48),
				Multiline = true,
				Name = "txt_input",
				ScrollBars = ScrollBars.Both,
				Size = new Size(321, 295),
				TabIndex = 2
			};

			txt_input.TextChanged += txt_input_TextChanged;

			txt_result = new TextBox()
			{
				Anchor = (AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Right),
				Location = new Point(339, 48),
				Multiline = true,
				Name = "txt_result",
				ReadOnly = true,
				ScrollBars = ScrollBars.Both,
				Size = new Size(388, 295),
				TabIndex = 3
			};

			txt_format = new TextBox()
			{
				Anchor = (AnchorStyles.Top | AnchorStyles.Left | AnchorStyles.Right),
				Location = new Point(12, 12),
				Name = "txt_format",
				Size = new Size(500, 20),
				TabIndex = 4,
				Text= "IDENT_"
			};

			txt_format.TextChanged += txt_format_TextChanged;

			chkUperCase = new CheckBox()
			{
				AutoSize = true,
				Checked = true,
				CheckState = CheckState.Checked,
				Location = new Point(650, 14),
				Name = "checkBox1",
				Size = new Size(82, 17),
				TabIndex = 5,
				Text = "Upper Case",
				UseVisualStyleBackColor = true,
				Anchor = (AnchorStyles.Top | AnchorStyles.Right),
			};

			chkIdent = new CheckBox()
			{
				AutoSize = true,
				Checked = true,
				CheckState = CheckState.Checked,
				Location = new Point(570, 14),
				Name = "checkBox2",
				Size = new Size(82, 17),
				TabIndex = 5,
				Text = "ident",
				UseVisualStyleBackColor = true,
				Anchor = (AnchorStyles.Top | AnchorStyles.Right),
			};

			chkIdent.CheckStateChanged += chkIdent_CheckStateChanged;

			base.Controls.Add(chkIdent);
			base.Controls.Add(chkUperCase);
			base.Controls.Add(txt_format);
			base.Controls.Add(txt_result);
			base.Controls.Add(txt_input);
			base.Controls.Add(btn_hash);

			base.AutoScaleDimensions = new SizeF(6f, 13f);
			base.AutoScaleMode = AutoScaleMode.Font;
			base.ClientSize = new Size(739, 397);

			base.Name = "frmGeroups";
			Text = "Hash";
			base.ResumeLayout(false);
			base.PerformLayout();
		}
	}
	
	public class frmMain : Form
	{
		private IContainer components = null;
		private Button button1;
		private TextBox textBox1;
		private TextBox textBox2;

		public frmMain()
		{
			InitializeComponent();
		}

		private void button1_Click(object sender, EventArgs e)
		{
			try
			{
				textBox2.Clear();
				if (!string.IsNullOrEmpty(textBox1.Text))
				{
					textBox2.Text = Hash.ToString(textBox1.Text);
				}
			}
			catch
			{
				textBox1.Focus();
			}
		}

		private void textBox1_TextChanged(object sender, EventArgs e)
		{
			if (!string.IsNullOrEmpty(textBox2.Text))
			{
				textBox2.Clear();
			}
			if (!string.IsNullOrEmpty(textBox1.Text))
			{
				try
				{
					textBox2.Text = Hash.ToString(textBox1.Text);
				}
				catch
				{
					textBox1.Focus();
				}
			}
		}

		protected override void Dispose(bool disposing)
		{
			if (disposing && components != null)
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		private void InitializeComponent()
		{
			base.SuspendLayout();

			button1 = new Button()
			{
				Location = new Point(197, 12),
				Name = "button1",
				Size = new Size(75, 46),
				TabIndex = 0,
				Text = "Hash",
				UseVisualStyleBackColor = true
			};

			button1.Click += button1_Click;
			
			textBox1 = new TextBox()
			{
				Location = new Point(12, 12),
				Name = "textBox1",
				Size = new Size(179, 20),
				TabIndex = 1
			};

			textBox1.TextChanged += textBox1_TextChanged;
			
			textBox2 = new TextBox()
			{
				Location = new Point(12, 38),
				Name = "textBox2",
				ReadOnly = true,
				Size = new Size(179, 20),
				TabIndex = 2
			};

			base.Controls.Add(button1);
			base.Controls.Add(textBox1);
			base.Controls.Add(textBox2);

			base.AutoScaleDimensions = new SizeF(6f, 13f);
			base.AutoScaleMode = AutoScaleMode.Font;
			base.ClientSize = new Size(284, 67);
			base.FormBorderStyle = FormBorderStyle.FixedToolWindow;
			base.StartPosition = FormStartPosition.CenterScreen;
			base.Name = "frmMain";
			Text = "Hash ident";
			base.ResumeLayout(false);
			base.PerformLayout();
		}
	}
}
