using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RTSScheduler
{
    public class Process: INotifyPropertyChanged
    {
        public enum Status
        {
            Waiting,
            Running,
            Preempted,
            Finished,
            Idle
        }

        private readonly Task task;

        private Status currentStatus;
        private int deadlineReachedIn;
        private int periodResetIn;
        private int cyclesNeeded;
        private double responseTime;


        private ObservableCollection<Status> statusHistogram;

        public string HistogramString
        {
            get { return getHistogramString(); }
        }
        public ObservableCollection<Status> StatusHistogram
        {
            get { return statusHistogram; }
        }
        public double ResponseTime
        {
            get { return responseTime; }
            set
            {
                if (value != responseTime)
                {
                    responseTime = value;
                    OnPropertyChanged("ResponseTime");
                }
            }
        }
        public int CyclesNeeded
        {
            get { return cyclesNeeded; }
            private set
            {
                if (value != cyclesNeeded)
                {
                    cyclesNeeded = value;
                    OnPropertyChanged("CyclesNeeded");
                }
            }
        }
        public int PeriodResetIn
        {
            get { return periodResetIn; }
            private set
            {
                if (value >= 0 && value != periodResetIn)
                {
                    periodResetIn = value;
                    OnPropertyChanged("PeriodResetIn");
                }
            }
        }
        public int DeadlineReachedIn
        {
            get { return deadlineReachedIn; }
            private set
            {
                if (value >= 0 && value != deadlineReachedIn)
                {
                    deadlineReachedIn = value;
                    OnPropertyChanged("DeadlineReachedIn");
                }
            }
        }
        public Status CurrentStatus
        {
            get { return currentStatus; }
            private set
            {
                if (value != currentStatus)
                {
                    currentStatus = value;
                    OnPropertyChanged("CurrentStatus");
                }
            }
        }
        public Task Task
        {
            get { return task; }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public Process(Task task)
        {
            this.task = task;
            this.statusHistogram = new ObservableCollection<Status>();
            this.statusHistogram.CollectionChanged += (s, e) => OnPropertyChanged("HistogramString");
            CurrentStatus = Status.Waiting;
            
            PeriodResetIn = Task.PeriodTime;
            DeadlineReachedIn = Task.Deadline;
            CyclesNeeded = Task.ExecutionTime;
        }

        public void Reset()
        {
            StatusHistogram.Clear();
            CurrentStatus = Status.Waiting;

            PeriodResetIn = Task.PeriodTime;
            DeadlineReachedIn = Task.Deadline;
            CyclesNeeded = Task.ExecutionTime;
        }

        public void doTick(bool start, bool preempt = false)
        {
            DeadlineReachedIn--;
            PeriodResetIn--;

            if (start)
            {
                if (CurrentStatus != Status.Idle && CurrentStatus != Status.Finished)
                {
                    CurrentStatus = Status.Running;
                } else
                {
                    //throw new ArgumentException("This Process has already Finished");
                }
            }
            if (preempt)
            {
                if (CurrentStatus == Status.Running)
                {
                    CurrentStatus = Status.Preempted;
                } else
                {
                    throw new ArgumentException("Cannot preemt a non-running Process");
                }
            }

            statusHistogram.Add(currentStatus);

            if (CurrentStatus == Status.Running)
            {
                CyclesNeeded--;
                if (CyclesNeeded == 0)
                {
                    CurrentStatus = Status.Finished;
                }
            } else if (CurrentStatus == Status.Finished)
            {
                CurrentStatus = Status.Idle;
            }

            if (DeadlineReachedIn == 0 && CurrentStatus != Status.Idle && CurrentStatus != Status.Finished)
            {
                throw new ProcessNotFinishedException(this);
            }

            if (PeriodResetIn == 0)
            {
                PeriodResetIn = Task.PeriodTime;
                DeadlineReachedIn = Task.Deadline;
                CyclesNeeded = Task.ExecutionTime;
                CurrentStatus = Status.Waiting;
            }
        }

        private string getHistogramString()
        {
            var tmp = StatusHistogram.Select<Status, char>((s) =>
            {
                switch (s)
                {
                    case Status.Waiting:
                        return '.';
                    case Status.Running:
                        return '#';
                    case Status.Preempted:
                        return '-';
                    case Status.Finished:
                    case Status.Idle:
                        return ' ';
                    default:
                        return '?';
                }
            }).ToArray();
            return new string(tmp);
        }
    
        private void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
