import os
import sys
import subprocess
import shlex


blocks = [x*4 for x in range(5,25)]
sizes = [2**x for x in range(8,13)]
log = ""
for size in sizes:
	for b_size in blocks:
		commandline_args = shlex.split("bin/matrixmult_cp 2 # -b %s -n %s"%(b_size,size))
		print commandline_args
		proc = subprocess.Popen(commandline_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		out, err = proc.communicate()
		print out
		log += out


with open("result_blocksizesSSE.txt","w") as f:
	f.write(log)