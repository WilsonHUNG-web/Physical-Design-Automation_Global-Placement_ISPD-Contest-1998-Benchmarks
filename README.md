# Physical-Design-Automation_Global-Placement_ISPD-98-Benchmarks
How to compile and execute my program:
<br>$make</br>
$../bin/hw5 ../testcase/ibm01.modified.txt ../output/ibm01.result


The program starts with parsing the file. After parsing into NET, it will shuffle the NET vector and sort them by ascending HPWL. And it started the wave propagation. After the initial routing, it will check the number of overflow. If it is larger than 0, then we go back to shuffle stage and so on. If its overflow is 0 or time is larger than 500 or there’s no improvement after 10 loops, the program will whatever output the result and terminate itself.
