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

            for (var i = 1; i <= Processes.Count; i++) { 
                for (var j = i; j <= Processes.Count; j++)
                {
                    /*if(processList.Contains(Processes[j]))
                        continue;*/
                    swap(i,j);
                    bool result = Rta(Processes, i);
                    if (result)
                    {
                        solutionFound = true;
                        //processList.Add(Processes[j]);
                        break;
                    }
                    else
                    {
                        swap(i, j);
                    }
                }

                if (solutionFound == false)
                {
                    return false;
                }
            }

            return true;

        }

        private void swap(int i, int j)
        {
            int gotIAt = 0;
            int gotJAt = 0;
            int n = 0;
            foreach (var p in Processes)
            {
                if (p.Task.Priority == i)
                    gotIAt = n;
                if (p.Task.Priority == j)
                    gotJAt = n;
                n++;
            }

            Processes[gotIAt].Task.Priority = j;
            Processes[gotJAt].Task.Priority = i;

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
