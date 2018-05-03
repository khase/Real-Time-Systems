using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RTSScheduler
{
    class ProcessNotFinishedException: Exception
    {
        private Process causingProcess;

        public Process CausingProcess
        {
            get { return causingProcess; }
            set { causingProcess = value; }
        }

        public ProcessNotFinishedException(Process causingProcess)
        {
            this.CausingProcess = causingProcess;
        }

        public override string ToString()
        {
            return $"Task {CausingProcess.Task.Name} was not finished in time";
        }
    }
}
