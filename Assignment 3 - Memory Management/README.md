### **Learning Objectives**
- **Demand Paging:** Understand and implement demand paging for memory management, including handling page faults and page replacement policies.  
- **Memory Management Concepts:** Explore the use of backing stores, partitioning memory into frame and variable stores, and using page tables to map virtual to physical memory.  
- **Process Sharing:** Implement mechanisms for memory sharing between processes, optimizing memory utilization for programs with identical scripts.  

---

### **Operating Systems Concepts Covered**
- **Paging Infrastructure:** Design a paging system to dynamically load pages into memory as needed, supporting execution of programs larger than the available shell memory.  
- **Page Replacement Policies:** Implement and manage the Least Recently Used (LRU) page replacement policy for efficient memory usage.  
- **Process Management:** Extend process execution to support multiple identical scripts using shared memory and demand paging.  
- **Backstore Simulation:** Use a directory as a simulated backing store to manage pages not currently loaded into memory.  
- **Scheduling Integration:** Combine demand paging with Round Robin (RR) scheduling to ensure seamless process execution.  

---

### **Summary of the Problem**
The task is to enhance a simulated operating system to support demand paging, allowing execution of programs larger than the shell memory size. Programs will be split into pages, dynamically loaded into memory when needed, and managed using page tables. The system will also implement the LRU page replacement policy to handle page faults efficiently.

---

### **Deliverables**
#### **Functional Requirements**
1. **Paging Infrastructure:**  
   - Set up a backing store directory to simulate a hard disk for managing pages not currently in memory.  
   - Split shell memory into frame and variable stores, with frames sized to hold 3 lines of code each.  

2. **Page Table Management:**  
   - Add page tables to the process control block (PCB) to map virtual pages to physical memory frames.  
   - Manage validity information to track whether a page is in memory or requires loading.

3. **Demand Paging Implementation:**  
   - Modify the `run` and `exec` commands to load only the required pages into memory dynamically.  
   - Handle page faults by loading the required page from the backing store and updating the page table.  
   - Implement a random page replacement policy initially and extend it to LRU in the final step.

4. **LRU Page Replacement Policy:**  
   - Track and evict the least recently used page from the frame store upon a page fault when memory is full.  
   - Ensure the victim page’s contents are printed before eviction.  

5. **Testing and Integration:**  
   - Run programs using the RR scheduling policy with a time slice of 2 instructions, ensuring seamless paging integration.  
   - Support execution of identical scripts in memory using shared frames.  

---

### **Expected Outputs**
- Programs must execute with dynamic memory loading, printing page fault messages and victim page contents as needed.  
- The system should integrate paging seamlessly with the RR scheduler and handle multiple instances of identical scripts.  
- Page replacement must be managed using a precise LRU policy to minimize performance overhead.  
