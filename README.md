<h1>Sampling based performance monitoring through hardware PMU in linux kernel</h1>

Performance monitoring counters can be used to analyze and measure performance of a program in a system. Most modern processors contain such counters. These counters are nothing but hardware registers which measure programmable events in the processor. The aim of this project is to use these hardware counter registers to obtain various statistics about a program such as - relative distribution of number of branch misses or L1 cache misses or CPU cycles in every function. The approach used to gather statistics is sampling based, in the sense that we periodically take samples of the state of the program to generate the statistics. The tool can also be used to adopt a counting based approach in gathering statistics.

Read report.pdf for more information.

