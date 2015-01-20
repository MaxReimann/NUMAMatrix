import os
import sys
import subprocess
import shlex
import string



def change_node_define(num):
	contents = ""
	with open("globals.h","r") as f:
		contents = f.read()
		start = string.find(contents,"NUMA_NODES")
		end = string.find(contents,"\n",start)
		for i in range(start, end):
			if contents[i] in ["1","2","4","6"]:
				contents = contents[:i] + str(num) +contents[i+1:]
				break

	with  open("globals.h","w") as f:
		f.write(contents)

	subprocess.call("make")


ndims = [2**x for x in range(6,14)]
#numa_nodes = [1,2,4,6]
log = ""
for ndim in ndims:
	log += "ndim: "+ str(ndim) + "\n"
	# log += "0: "

	for i in [0,1,3,4]:
		commandline_args = shlex.split("bin/matrixmult %s # -n %s"%(i,ndim))
		proc = subprocess.Popen(commandline_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		out, err = proc.communicate()
		print out
		log += out

with open("result_fastest.txt","w") as f:
	f.write(log)
