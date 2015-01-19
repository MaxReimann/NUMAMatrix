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


ndims = [2**x for x in range(8,16)]
#numa_nodes = [1,2,4,6]
log = ""
for ndim in ndims:
	log += "ndim: "+ str(ndim)
	# log += "0: "
	# commandline_args = shlex.split("bin/matrixmult 0 -n %s"%ndim)
	# proc = subprocess.Popen(commandline_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	# out, err = proc.communicate()
	# print out
	# log += out

	commandline_args = shlex.split("bin/matrixmult 2 -n %s"%ndim)
	proc = subprocess.Popen(commandline_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	out, err = proc.communicate()
	print out
	log += out


	# for nodes in numa_nodes :
	# 	log += str(nodes)+": " 
	# 	change_node_define(nodes)

	# 	commandline_args = shlex.split("bin/matrixmult 3 -n %s"%ndim)
	# 	proc = subprocess.Popen(commandline_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	# 	out, err = proc.communicate()
	# 	print out
	# 	log += out

	# commandline_args = shlex.split("bin/matrixmult 4 -n %s"%ndim)
	# proc = subprocess.Popen(commandline_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	# out, err = proc.communicate()
	# print out
	# log += out

with open("result_parallel_strassen.txt","w") as f:
	f.write(log)
