### **Learning Objectives**
- **Process Scheduling:** Understand and implement scheduling algorithms such as First-Come-First-Serve (FCFS), Shortest Job First (SJF), Round Robin (RR), and Aging to manage concurrent processes.  
- **Concurrency Control:** Learn to execute multiple processes simultaneously using a single-core simulation and later extend to a multi-threaded scheduler with worker threads.  
- **Shell and OS Extensions:** Expand the functionality of a basic shell to support process management, memory allocation, and scheduling policies.  

---

### **Operating Systems Concepts Covered**
- **Process Control Block (PCB):** Design data structures to represent and manage process information, including process IDs and program counters.  
- **Ready Queue and Scheduling:** Create and manage a ready queue to facilitate scheduling and switching of processes.  
- **Scheduling Policies:** Implement and compare various scheduling algorithms (e.g., FCFS, SJF, RR) and introduce job aging to prevent starvation.  
- **Concurrency in OS:** Transition from single-threaded to multi-threaded schedulers using the `pthreads` library, simulating a multi-core environment.  
- **Batch and Background Execution:** Support concurrent batch execution and background processes using scheduling policies.  

---

### **Summary of the Problem**
The task involves enhancing a simulated operating system shell to support multi-process execution. Starting with basic process scheduling using FCFS, the assignment progresses to implementing additional scheduling policies (SJF, RR, and Aging) and concludes with the creation of a multi-threaded scheduler. Processes are represented by scripts, and the shell must handle concurrent execution, resource allocation, and process management efficiently.

---

### **Deliverables**
#### **Functional Requirements**
1. **Basic Scheduling Infrastructure:**  
   - Modify the shell to load script code into memory, represent processes using PCBs, and execute processes via a ready queue.  
   - Implement FCFS scheduling for single-process execution.  

2. **Multi-Process Execution (`exec` Command):**  
   - Extend the shell with an `exec` command to support up to three concurrent processes.  
   - Implement scheduling policies (SJF, RR) for managing multiple processes.  

3. **Advanced Scheduling Policies:**  
   - Add job aging to SJF to address starvation.  
   - Implement an extended Round Robin policy (RR30) with a larger time slice.  

4. **Multi-Threaded Scheduler:**  
   - Use `pthreads` to create a pool of worker threads for concurrent process execution.  
   - Ensure proper handling of output synchronization using mutexes or locks.

5. **Batch Mode Enhancements:**  
   - Support background execution with the `#` option, allowing the shell to execute scripts while continuing batch processing.

---

### **Expected Outputs**
- Processes must execute according to the specified scheduling policies, with the correct order and time slicing.  
- Background processes should execute independently, with output synchronization.  
- Multi-threaded execution should simulate a two-core system, with deterministic outputs where applicable.  
