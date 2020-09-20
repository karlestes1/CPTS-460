# CPTS 460 - Washington State University

*****Disclaimer:***** These project were built as a requirement for the computer science 460 class at Washington State University under the instruction of K.C. Wang. All assembly code was provided by K.C. Wang and the projects were completed within the provided parameters for each assignment. Descriptions for each assignment can be found below.

*****This project is archived and is read-only*****
___

### Languages and Tools
<img align="left" height="32" width="32" src="https://raw.githubusercontent.com/github/explore/80688e429a7d4ef2fca1e82350fe8e3517d3494d/topics/c/c.png" />
<img align="left" height="32" width="32" src="https://raw.githubusercontent.com/github/explore/80688e429a7d4ef2fca1e82350fe8e3517d3494d/topics/linux/linux.png" />
<img align="left" height="32" width="32" src="https://raw.githubusercontent.com/github/explore/80688e429a7d4ef2fca1e82350fe8e3517d3494d/topics/visual-studio-code/visual-studio-code.png" />
<img align="left" height="32" width="32" src="https://cdn.jsdelivr.net/npm/simple-icons@v3/icons/qemu.svg" />
<img align="left" height="32" width="32" src="https://raw.githubusercontent.com/github/explore/80688e429a7d4ef2fca1e82350fe8e3517d3494d/topics/terminal/terminal.png" />
<img align="left" height="32" width="32" src="https://raw.githubusercontent.com/github/explore/80688e429a7d4ef2fca1e82350fe8e3517d3494d/topics/git/git.png" />
<img align="left" height="32" width="32" src="https://cdn.jsdelivr.net/npm/simple-icons@v3/icons/gitkraken.svg" />
<br>

### Textbooks
[<img alt="Embedded and Real-Time Operating Systems" align="left" height="64" width="52" src="https://eecs.wsu.edu/~cs460/cs560/embedOS.jpg" />][embeddedOSBook]
Embedded and Real-Time Operating Systems by K.C. Wang
<br />
<br />
<br />

[<img alt="Design and Implementation of the MTX Operating System" align="left" height="64" width="52" src="https://images-na.ssl-images-amazon.com/images/I/41yWJMfesOL._SX330_BO1,204,203,200_.jpg" />][MTXOSBook]
Design and Implementation of the MTX Operating System by K.C. Wang
<br />


<!--Definitions-->
[embeddedOSBook]: https://www.amazon.com/Embedded-Real-Time-Operating-Systems-K-C/dp/3319515160
[MTXOSBook]: https://www.amazon.com/Design-Implementation-MTX-Operating-System-ebook/dp/B010N45I86

<br />
<br />

___

### Lab 1
###### MTX Operating System Booter
**Lab 1.1:** Pre-lab work to learn how to generate a binary executable, dump it to a (virtual) disk, and boot up the disk using the QEMU emulator  
<br />

**Lab1.2:** Addition of C-code to get root inode, read in a data block of the root DIR, and print all the filenames contained within the root directory (/)  
<br />

**Lab1.3:** Modification of the code from Lab1.2 to get the INODE of /boot/mtx and to now boot up the MTX operating system from the mtximage virtual disk. The total boot program had to be less than 1KB when compiled to fit in the boot segment of the disk  
<br />

### Lab 2
###### UART Terminals
**Lab 2.0:** Initial assembly file downloads  

**Lab 2.1:** An exploration into UART terminals. Allows the lines to be entered from the UART terminal and appear on the connected terminal

**Lab 2.2:** Displays a .bmp file at 640x480 resolution in 4 different zoom levels while reading input from the UART terminal

### Lab 3
###### Binary Tree PROC Structure
**Lab3.1:** Implements parent-child process relationships as a binary tree and implements sleep(), wakeup(), and wait() functions in regards to processes


### Lab 4
##### The instructions for lab 4 could not be found. Should I find them, I shall update this section

### Lab 5
###### Forking and Execution of Processes
A modification of provided code to support a Umode image that is 4MB in size with the adition of vfork() and exec() functions to fork and execute processes

### Lab 6
##### The instructions for lab 4 could not be found. Should I find them, I shall update this section

### Midterm Project
**Part 1:** Focused on implemented process management with a queue  

**Part 2:** Initializaiton and management of processes through the use of timers  

**Part 3:** Answering a series of questions related to data_abort faults and address mapping from pTables  

### Final Project
The final project consisted of creating the init, login, and sh programs for the MTX operating system. It allows for logins across multiple terminals, can handlews a series of user commands, and it allows for chained commands within the shell which means redirected i/o streams.