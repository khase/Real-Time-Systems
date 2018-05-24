using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RTSScheduler.Scheduler
{
    public class DeadlineMonotonicScheduler : MonotonicScheduler
    {
        public DeadlineMonotonicScheduler(ObservableCollection<Task> tasks): base()
        {
            int count = tasks.Count;
            var tmp = tasks.OrderByDescending(t => t.Deadline).ToList();
            tmp.Reverse();
            tmp.ForEach(t => t.Priority = (tmp.IndexOf(t) + 1));

            Processes = new ObservableCollection<Process>(
                tmp.OrderByDescending(t => t.Priority)
                .Select<Task, Process>(t => new Process(t))
            );

            analyseResponseTime();
        }

        protected override void analyseResponseTime()
        {
            foreach (Process proc in Processes)
            {
                proc.ResponseTime = proc.Task.ExecutionTime +
                    Processes
                    .Select(p => p.Task)
                    .Where(t => t.Priority > proc.Task.Priority)
                    .Select(t => Math.Ceiling((double)proc.Task.Deadline / t.PeriodTime) * t.ExecutionTime)
                    .Sum();
            }
        }
    }
}
