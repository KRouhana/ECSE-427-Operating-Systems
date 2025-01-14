### **Learning Objectives**
- **Shell Programming:** Learn the principles of shell design and implementation, including user interaction and command processing.  
- **Operating System Fundamentals:** Understand key operating system concepts such as command execution, file management, and process control.  
- **C Programming in OS Context:** Gain proficiency in implementing and debugging OS-like features using the C programming language.  

---

### **Operating Systems Concepts Covered**
- **Command Interpretation and Execution:** Implement basic and extended shell commands (e.g., `set`, `echo`, `ls`, `mkdir`, `touch`, `cd`).  
- **Process Management:** Introduce concepts such as command chaining and process creation with the `fork-exec` pattern.  
- **File and Directory Management:** Manage files and directories using commands like `ls`, `mkdir`, and `touch`.  
- **Batch and Interactive Modes:** Support multiple modes of operation, including batch file execution and interactive user input.  
- **Error Handling:** Implement robust error detection and handling mechanisms for invalid commands and batch processing issues.  

---

### **Summary of the Problem**
The task is to enhance a basic shell program to support additional functionality, improving its usability and robustness. The assignment involves adding features like multi-token value assignment, command chaining, directory and file management, and custom command handling. 

---

### **Deliverables**
#### **Functional Requirements**
1. **New `set` Command:**  
   - Support multi-token values (up to 5 tokens) while handling token limits with appropriate error messages.

2. **New `echo` Command:**  
   - Display alphanumeric strings or variable values stored in shell memory.

3. **Batch Mode Execution:**  
   - Resolve infinite loop issues when `quit` is not the last batch command.  
   - Suppress `$` prompts during batch execution.

4. **File and Directory Management Commands:**  
   - Implement `my_ls` for listing files and directories in alphabetical order.  
   - Add `my_mkdir` and `my_touch` for creating directories and files, respectively.  
   - Implement `my_cd` for navigating directories.

5. **Command Chaining:**  
   - Support multiple commands in a single line separated by semicolons.

6. **Execute**
	- Add an `exec` command using the `fork-exec` pattern to execute system commands with arguments.

---

### **Outputs**
- The shell must correctly execute and handle all specified commands in both batch and interactive modes.  
- Batch mode must transition seamlessly to interactive mode after executing all batch commands.  
- Commands like `my_ls`, `my_mkdir`, `my_touch`, and `my_cd` must perform file and directory operations with proper error handling.  
- Chained commands must execute sequentially, and appropriate errors must be displayed for invalid commands.  
