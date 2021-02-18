
import json

# Return type of testfunc
class testclass:
	def __init__(self, name, vec):
		# string
		self.name = name
		# vector<double>
		self.vec = vec
		# bool
		self.retval = True
	
	# Doubles the values of self.vec
	def doublevec(self):
		self.vec = [x*2 for x in self.vec]

# Returns back a new testclass object. Just for testing purposes
# Args: string, vector<double>
def testfunc(name, vec):
	print('Now inside python script function "testfunc"')
	print("This is the input string: " + name)
	print("This is the input vector: ")
	print(vec)
	print("")

	# return a new testclass object
	t = testclass(name, vec)
	t.doublevec()
	return t

# For testing vector<vector<double>>
def testfunc2(vec_of_vec):
	print('Now inside python script function "testfunc2"')
	print("This is the input vector<vector<double>>: ")
	print(vec_of_vec)

# Called from the agents
# Accepts as argument a json string of vector<sim_state>, which the python json module can automatically parse as a list of dicts
# Adds 1 to the xyz position and velocity to prove that we can:
#	1) Read in the state vectors over from cosmosstruc
#	2) Parse the data structure and modify the values in the python script
#	3) Return back the modified values to the agent
def modify_sim_states(sim_states):
	j = json.loads(sim_states)
	for state in j["sim_states"]:
		state["x_pos"] += 1
		state["y_pos"] += 1
		state["z_pos"] += 1
		state["x_vel"] += 1
		state["y_vel"] += 1
		state["z_vel"] += 1
		state["x_way"] += 1
		state["y_way"] += 1
		state["z_way"] += 1
	#print(j)
	return json.dumps(j)