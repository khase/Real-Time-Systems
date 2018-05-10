using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RTSScheduler.Scheduler
{
    public abstract class Scheduler: INotifyPropertyChanged
    {
        private ObservableCollection<Process> processes;

        private int currentCycle;

        public int CurrentCycle
        {
            get { return currentCycle; }
            protected set
            {
                if (value != currentCycle)
                {
                    currentCycle = value;
                    OnPropertyChanged("CurrentCycle");
                }
            }
        }
        public ObservableCollection<Process> Processes
        {
            get { return processes; }
            protected set
            {
                if (value != processes)
                {
                    processes = value;
                    OnPropertyChanged("UtilizationFactor");
                    OnPropertyChanged("LiuTest");
                    processes.CollectionChanged += (s, e) =>
                    {
                        OnPropertyChanged("UtilizationFactor");
                        OnPropertyChanged("LiuTest");
                    };
                }
            }
        }
        public double UtilizationFactor
        {
            get
            {
                return processes.Select<Process, double>(p => (double)p.Task.ExecutionTime / (double)p.Task.PeriodTime).Sum();
            }
        }

        public bool LiuTest
        {
            get
            {
                return UtilizationFactor <= (processes.Count * (Math.Pow(2, 1.0/processes.Count) - 1));
            }
        }

        public long MajorCycle
        {
            get
            {
                return lcm(processes.Select<Process, int>(p => p.Task.PeriodTime).ToArray());
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public Scheduler()
        {
            this.Processes = new ObservableCollection<Process>();
        }

        public abstract void doTick();
        public void schedule()
        {
            do
            {
                doTick();
            } while (CurrentCycle < MajorCycle);
        }

        private long lcm(int[] numbers)
        {
            long lcm = 1;
            int divisor = 2;
            while (true)
            {
                int cnt = 0;
                bool divisible = false;
                for (int i = 0; i < numbers.Length; i++)
                {
                    /**
                     * lcm (n1,n2,... 0)=0.For negative number we convert into
                     * positive and calculate lcm.
                     */
                    if (numbers[i] == 0)
                    {
                        return 0;
                    }
                    else if (numbers[i] < 0)
                    {
                        numbers[i] = numbers[i] * (-1);
                    }
                    if (numbers[i] == 1)
                    {
                        cnt++;
                    }
                    /**
                     * divide numbers by devisor if complete division i.e. without
                     * remainder then replace number with quotient; used for find
                     * next factor
                     */
                    if (numbers[i] % divisor == 0)
                    {
                        divisible = true;
                        numbers[i] = numbers[i] / divisor;
                    }
                }
                /**
                 * If divisor able to completely divide any number from array
                 * multiply with lcm and store into lcm and continue to same divisor
                 * for next factor finding. else increment divisor
                 */
                if (divisible)
                {
                    lcm = lcm * divisor;
                }
                else
                {
                    divisor++;
                }
                /**
                 * Check if all numbers is 1 indicate we found all factors and
                 * terminate while loop.
                 */
                if (cnt == numbers.Length)
                {
                    return lcm;
                }
            }
        }

        protected virtual void analyseResponseTime()
        {
            foreach (Process p in processes)
            {
                double ResponseTimeOld = 0;
                do
                {
                    ResponseTimeOld = p.ResponseTime;
                    p.ResponseTime = ResponseNext(p.Task, ResponseTimeOld);
                } while (p.ResponseTime != ResponseTimeOld);
            }
        }
        
        protected virtual bool rta(ObservableCollection<Process> processes, int priority)
        {
            foreach (Process p in processes)
            {
                if (p.Task.Priority != priority) continue;
                double ResponseTimeOld = 0;
                do
                {
                    ResponseTimeOld = p.ResponseTime;
                    p.ResponseTime = ResponseNext(p.Task, ResponseTimeOld);
                } while (p.ResponseTime != ResponseTimeOld);

                if (p.Task.Deadline < ResponseTimeOld)
                    return false;
            }
            return true;
        }

        private double ResponseNext(Task task, double responseAct)
        {
            return task.ExecutionTime + 
                processes
                .Select(p => p.Task)
                .Where(t => t.Priority > task.Priority)
                .Select(t => Math.Ceiling(responseAct / t.PeriodTime) * t.ExecutionTime)
                .Sum();
        }

        private void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
