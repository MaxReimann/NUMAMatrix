import ast



p_func_names = []
c_func_names = []
output  = ""

def parse(inp):
	global output
	output = ""
	expressions = inp.split("\n")
	for expr in expressions:
		if not expr.startswith("P") and not expr.startswith("c"):
			continue
		
		stmts = [x.strip() for x in expr.split("=")]
		tree = ast.parse(stmts[1], "", "eval")
		if expr.startswith("P"):
			parse_P(tree, stmts)
		else:
			parse_C(tree, stmts)


	return output

def parse_P(tree, statements):
	global output
	output += function_template(statements[0])
	p_func_names.append(statements[0])
	left_op = "p11"
	right_op = "p12"
	left = tree.body.left
	right = tree.body.right
	if left.__class__ == ast.Name:
		left_op = "_" + left.id
		traverse(right,  False)
	elif right.__class__ == ast.Name:
		right_op = "_" + right.id
		traverse(left, True)
	else :
		traverse(left, True)
		traverse(right,  False)
	output += "strassen_multiply(n, %s, %s, p_res, p_temp);\n"%(left_op,right_op)
	output += "}\n\n\n"

def parse_C(tree, statements):
	global output
	output += gen_function_parametersC(statements[0])
	c_func_names.append(statements[0])
	startIndex = len(output)

	# subdivide in toplevel expressions
	statement = statements[1]
	parts = statement.split("(")[1:] #first el is always empty
	parts = [part[:part.find(")")] for part in parts]

	for i, p in enumerate(parts):
		statement = statement.replace("(%s)"%p, str(i))


	x = { 'has_temp': False, "is_first":True }

	def conditional_traverse(lastOp):
		global output
		first,x["is_first"] = x["is_first"], False
		if lastOp == "+" or lastOp==None :
			traverse(tree, True, "_" + statements[0], first=first)
		elif lastOp == "-":
			traverse(tree, True, "temp", first=first)
			output += "sub(n, _%s, temp, _%s);\n"%(statements[0],statements[0])
			if not x["has_temp"]:
				w = "matrix temp = strassen_newmatrix(n);\n"
				output = output[:startIndex] + w + output[startIndex:]
				x["has_temp"] = False

		
	lastOp = None
	for char in statement:
		if char in ["0","1","2","3"]:
			tree = ast.parse(parts[int(char)], "", "eval").body
			conditional_traverse(lastOp)
		elif char in ["+","-"]:
			lastOp = char
	
	output += "}\n\n\n"

def gen_function_parametersC(name):
	out = "void _%s(int n, matrix P[], matrix _%s)\n{\n"%(name,name)
	return out



def traverse(tree, is_left, tmp_mat = "p11",first = False): 
	global output
	if not is_left:
		tmp_mat = "p12"

	if tree.op.__class__ == ast.Add :
		op = "add"
	else :
		op = "sub"

	left_class = tree.left.__class__
	right_class = tree.right.__class__


	if left_class == ast.Name and right_class == ast.Name :
		if first:
			output += "%s(n, _%s, _%s, %s);\n"%(op,tree.left.id, tree.right.id,tmp_mat)
		else:
			output += "%s(n, _%s, %s, %s);\n"%(op,tree.left.id, tmp_mat,tmp_mat)
			output += "%s(n, %s, _%s, %s);\n"%(op,tmp_mat, tree.right.id,tmp_mat)
	elif left_class != ast.Name and right_class == ast.Name :
		traverse(tree.left, is_left, tmp_mat)
		output += "%s(n, %s, _%s, %s);\n"%(op,tmp_mat,tree.right.id,tmp_mat)
	elif left_class == ast.Name and right_class != ast.Name :
		traverse(tree.right, is_left, tmp_mat)
		output += "%s(n, _%s, %s, %s);\n"%(op,tree.left.id,tmp_mat,tmp_mat)
	else:
		traverse(tree.left, is_left, tmp_mat)
		traverse(tree.right, is_left, tmp_mat)


def function_template(name):
	return """void %s(int n, matrix a, matrix b, matrix p)\n{\n"""%(name)

def add_function_pointers():
	f_pointers = ""
	for i, name in enumerate(p_func_names):
			f_pointers+= "p_fPtr[%s] = %s;\n"%(i, name)
	for i, name in enumerate(c_func_names):
			f_pointers+= "c_fPtr[%s] = _%s;\n"%(i, name)
	return f_pointers

if __name__ == "__main__":

	with open("strassenParallelTemplate.template") as _file:
		inp = _file.read()
		start = inp.find("#start_definition")
		end = inp.find("#end_definition")
		out = parse(inp[start:end])

		start = inp.find("#start_result_definition")
		end = inp.find("#end_result_definition")
		out += parse(inp[start:end])


	with open("strassenParallelGenerated.c","w") as _file:
		out = inp.replace("#insert_functions", out)
		out = out.replace("#insert_funtion_pointers", add_function_pointers())
		_file.write(out)





