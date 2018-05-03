using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RTSScheduler
{
    public class Task: INotifyPropertyChanged
    {
        private int priority;
        private int executionTime;
        private int periodTime;
        private int deadline;
        private string name;

        public string Name
        {
            get { return name; }
            set {
                if (value != name)
                {
                    name = value;
                    OnPropertyChanged("Name");
                }
            }
        }
        public int Deadline
        {
            get { return deadline; }
            set
            {
                if (value == 0)
                {
                    value = PeriodTime;
                }
                if (value != deadline)
                {
                    deadline = value;
                    OnPropertyChanged("Deadline");
                }
            }
        }
        public int PeriodTime
        {
            get { return periodTime; }
            set
            {
                if (value != periodTime)
                {
                    periodTime = value;
                    OnPropertyChanged("PeriodTime");
                }
            }
        }
        public int ExecutionTime
        {
            get { return executionTime; }
            set
            {
                if (value != executionTime)
                {
                    executionTime = value;
                    OnPropertyChanged("ExecutionTime");
                }
            }
        }
        public int Priority
        {
            get { return priority; }
            set
            {
                if (value != priority)
                {
                    priority = value;
                    OnPropertyChanged("Priority");
                }
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public Task(string Name, int PeriodTime, int Deadline, int ExecutionTime)
        {
            this.Name = Name;
            this.PeriodTime = PeriodTime;
            this.Deadline = Deadline;
            this.ExecutionTime = ExecutionTime;
        }

        private void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
