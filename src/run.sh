echo "[run ibm01]"
../bin/hw5 ../testcase/ibm01.modified.txt ../output/ibm01.result
echo "[finished ibm01]"



tar -zcvf CS6135_HW5_109065527.tar.gz ./HW5
mv ./CS6135_HW5_109065527.tar.gz ./HW5_grading/student/109065527/CS6135_HW5_109065527.tar.gz
cd HW5_grading
bash HW5_grading.sh