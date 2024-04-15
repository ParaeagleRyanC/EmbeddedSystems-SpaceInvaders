# Space Invaders

This is a large-scale C/C++ program that focuses on embedded systems. A key feature of this program is the Programmable Interval Timer driver that allows the program to modify its game run speed on the fly.
 
 * **device_tree:** This folder contains the Linux Device Tree, which enumerates all hardware available to the Linux system.
 * **hw:** This folder contains the hardware design programmed on the FPGA. 
 * **kernel:** This folder will contain kernel software, ie. kernel drivers.
 * **userspace:** This folder contains all software, drivers and applications, that will run in userspace.

This program demonstrates the knowledge and the ability to
* Implement multiple user space drivers, including drivers for the Xilinx GPIO and interrupt controller IP cores.
* Create a user-space application that makes use of the GPIO and interrupt controller drivers.
* Design and build a large-scale C/C++ program, with functionalitlies that follows the classic Space Invaders game.
* Create loadable kernel modules, which allows one to write code that runs in kernel space. (Audio Driver and PIT Driver)
* Showcase embedded systems using PYNQ board.
