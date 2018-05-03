using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RTSScheduler.Scheduler
{
    public class RateMonotonicScheduler : MonotonicScheduler
    {
        public RateMonotonicScheduler(ObservableCollection<Task> tasks): base()
        {
            int count = tasks.Count;
            var tmp = tasks.OrderByDescending(t => t.PeriodTime).ToList();
            tmp.ForEach(t => t.Priority = (tmp.IndexOf(t) + 1));

            Processes = new ObservableCollection<Process>(
                tmp.OrderByDescending(t => t.Priority)
                .Select<Task, Process>(t => new Process(t))
            );
        }
    }
}
