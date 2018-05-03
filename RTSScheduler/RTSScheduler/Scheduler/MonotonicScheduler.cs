using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RTSScheduler.Scheduler
{
    public abstract class MonotonicScheduler : Scheduler
    {
        public override void doTick()
        {
            if (CurrentCycle >= MajorCycle)
            {
                return;
            }

            CurrentCycle++;

            Process[] tmp = Processes.Where(p => p.CurrentStatus == Process.Status.Running).ToArray();
            bool hasRunning = tmp.Length > 0;
            Process highestPriorityProcess = Processes
                .Where(p => p.CurrentStatus != Process.Status.Finished && p.CurrentStatus != Process.Status.Idle)
                .OrderByDescending(p => p.Task.Priority)
                .FirstOrDefault();

            if (hasRunning)
            {
                Process runningProcess = tmp[0];
                if (highestPriorityProcess != null && highestPriorityProcess != runningProcess)
                {
                    runningProcess.doTick(false, true);
                    highestPriorityProcess.doTick(true, false);
                    Processes.Where(p => p != highestPriorityProcess && p != runningProcess)
                        .ToList()
                        .ForEach(p => p.doTick(false, false));
                }
                else
                {
                    Processes.ToList().ForEach(p => p.doTick(false, false));
                }
            } else
            {
                highestPriorityProcess?.doTick(true, false);
                Processes.Where(p => p != highestPriorityProcess)
                    .ToList()
                    .ForEach(p => p.doTick(false, false));
            }
        }
    }
}
