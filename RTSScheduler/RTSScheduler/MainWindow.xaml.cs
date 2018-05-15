using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.IO;
using System.Windows;
using RTSScheduler.Scheduler;

namespace RTSScheduler
{
    /// <summary>
    /// Interaktionslogik für MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, INotifyPropertyChanged
    {
        ObservableCollection<Task> Tasks;
        RTSScheduler.Scheduler.Scheduler scheduler;

        public event PropertyChangedEventHandler PropertyChanged;

        public RTSScheduler.Scheduler.Scheduler Scheduler
        {
            get { return scheduler; }
            private set
            {
                if (scheduler != value)
                {
                    scheduler = value;
                    OnPropertyChanged("Scheduler");
                }
            }
        }

        public MainWindow()
        {
            InitializeComponent();
            Tasks = new ObservableCollection<Task>();

            this.DataContext = this;
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            using (StreamReader rd = new StreamReader("Tasks.json"))
            {
                Tasks = new ObservableCollection<Task>(
                    Newtonsoft.Json.JsonConvert.DeserializeObject<Task[]>(
                        rd.ReadToEnd() 
                    )
                );
            }

            Scheduler = new DeadlineMonotonicScheduler(Tasks);
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            using (StreamWriter wrt = new StreamWriter("ScheduledTasks.json"))
            {
                wrt.Write(Newtonsoft.Json.JsonConvert.SerializeObject(Tasks, Newtonsoft.Json.Formatting.Indented));
                wrt.Flush();
            }
            using (StreamWriter wrt = new StreamWriter("Output.txt"))
            {
                foreach (Process p in Scheduler.Processes)
                {
                    wrt.Write(p.Task.Name.ToString().PadRight(12).Substring(0, 12) + ":");
                    wrt.WriteLine(p.HistogramString);
                }
                wrt.Write("Utilization".PadRight(12).Substring(0, 12) + ":");
                for (int i = 0; i < Scheduler.CurrentCycle; i++)
                {
                    bool idle = true;
                    foreach (Process p in Scheduler.Processes)
                    {
                        if (p.StatusHistogram[i] == Process.Status.Running)
                        {
                            wrt.Write(p.Task.Name.ToString().PadRight(1).Substring(0, 1));
                            idle = false;
                            break;
                        }
                    }
                    if (idle)
                    {
                        wrt.Write(" ");
                    }
                }
            }
        }

        private void ButtonComplete_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                scheduler.schedule();
            }
            catch (ProcessNotFinishedException pe)
            {
                OPA rta = new OPA(Scheduler.Processes);
                MessageBoxResult res = MessageBox.Show(pe.ToString() + "\n\n" + rta.ToString() + "\n\nOverwrite Priorities?", pe.CausingProcess.Task.Name, MessageBoxButton.YesNo);
                if (res == MessageBoxResult.Yes)
                {
                    Scheduler = rta;
                    Scheduler.Processes.ToList().ForEach(p => p.Reset());
                }
                //MessageBox.Show(pe.ToString() , pe.CausingProcess.Task.Name);
            }
        }

        private void ButtonTick_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                scheduler.doTick();
            }
            catch (ProcessNotFinishedException pe)
            {
                OPA rta = new OPA(Scheduler.Processes);
                MessageBoxResult res = MessageBox.Show(pe.ToString() + "\n\n" + rta.ToString() + "\n\nOverwrite Priorities?", pe.CausingProcess.Task.Name, MessageBoxButton.YesNo);
                if (res == MessageBoxResult.Yes)
                {
                    Scheduler = rta;
                    Scheduler.Processes.ToList().ForEach(p => p.Reset());
                }
                //MessageBox.Show(pe.ToString(), pe.CausingProcess.Task.Name);
            }
        }

        private void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
