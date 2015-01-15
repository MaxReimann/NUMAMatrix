import os
import sys
import subprocess
import shlex

steps = [2,4,6,8,10]
steps += [x for x in range(20,100,20)]
steps += [x for x in range(100,500,50)]
print steps
log = ""
for b_size in range(1000, 5000, 1000):
	for step in steps:
		if step!=2:
			continue
		commandline_args = shlex.split("bin/matrixmult 1 -b %s -n %s"%(step,b_size))
		print commandline_args
		proc = subprocess.Popen(commandline_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		out, err = proc.communicate()
		print out
		log += out


with open("result_blocksizes.txt","w") as f:
	f.write(log)