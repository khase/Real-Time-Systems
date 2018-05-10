using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Linq.Expressions;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace RTSScheduler.Scheduler
{
    class OPA : MonotonicScheduler
    {
        private bool solutionFound = false;

        public OPA(ObservableCollection<Process> processes) : base()
        {
            Processes = new ObservableCollection<Process>(processes.OrderBy(p => p.Task.Priority));
            testPriorities();
        }

        //TODO Umbennen und was gescheites draus machen
        public bool testPriorities()
        {
            List<Process> processList = new List<Process>();

            for (var i = 0; i < Processes.Count; i++)
                for (var j = i; j < Processes.Count; j++)
                {
                    if(processList.Contains(Processes[j]))
                        continue;
                    swap(i,j);
                    bool result = rta(base.Processes, i);
                    if (result)
                    {
                        processList.Add(Processes[j]);
                        continue;
                    }
                }

            if (processList.Count == Processes.Count)
            {
                solutionFound = true;
                return true;
            }
            else
                return false;
        }

        private void swap(int i, int j)
        {
            var tmp = Processes[i].Task.Priority;
            Processes[i].Task.Priority = Processes[j].Task.Priority;
            Processes[j].Task.Priority = tmp;
        }

        public override String ToString()
        {
            if (solutionFound)
            {
                String s = "Possible solution might be:\n";
                s += "Task \tPriority\n";
                foreach (var p in Processes)
                {
                    s += p.Task.Name + "\t" + p.Task.Priority + "\n";
                }

                return s;
            }
            else
                return "";
        }
    }
}
