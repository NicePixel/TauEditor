#!/usr/bin/python
import sys, math

all_vertices = []
all_lines    = []
all_tris     = []
all_entities = []

all_vertexnormals = []
all_linenormals = []
all_uvs = []

# Flipped X and Y coord?
""" Order is important!

typedef struct _VERTEX
{
	int id;
	int y;
	int x;
} VERTEX;

typedef struct _LINE
{
	int v_id0;
	int v_id1;
	int id;
	int selected; // THIS IS NOT WRITTEN IN THE EXPORTED FILE
} LINE;

typedef struct _TRI
{
	int v_id0;
	int v_id1;
	int v_id2;
	int id;
} TRI;

typedef struct _ENTITY
{
	int x, y;
	int id;
	int eid;
	int flags;
	int angle;
} ENTITY;
"""	

def findvertex(id):
	for i in range(0, len(all_vertices)):
		if all_vertices[i][0] == id:
			return all_vertices[i]
	print("No vertex of ID=" + str(id) + " has been found. How did this happen?")

def calc_normal_lines():
	global all_linenormals
	for i in range(0, len(all_lines)):
		l = all_lines[i]
		v0 = findvertex(l[0])
		v1 = findvertex(l[1])
		dx = v0[1] - v1[1]
		dy = v0[2] - v1[2]
		all_linenormals.append([-dy, dx])
		
def findconnectedlines_normalweight(vert_id):
	normal_weight = [0.0, 0.0]
	counter = 0
	for i in range(0, len(all_lines)):
		l = all_lines[i]
		if l[0] == vert_id or l[1] == vert_id:
			normal_weight[0] = normal_weight[0] + all_linenormals[i][0]
			normal_weight[1] = normal_weight[1] + all_linenormals[i][1]
			counter = counter + 1
	
	if counter != 0:
		normal_weight[0] = normal_weight[0] / counter
		normal_weight[1] = normal_weight[1] / counter
	
	return normal_weight

def calc_normal_vertices():
	global all_vertexnormals
	for i in range(0, len(all_vertices)):
		v     = all_vertices[i]
		wnavg = findconnectedlines_normalweight(v[0])
		all_vertexnormals.append(wnavg)
		
def getvertexindexinlist(v_id):
	for i in range(0, len(all_vertices)):
		if all_vertices[i][0] == v_id:
			return i
	print("No vertex of ID=" + str(v_id) + " has been found. How did this happen?")
	
def calc_texture_uv():
	global all_uvs
	scale = 8.0
	for i in range(0, len(all_lines)):
		l  = all_lines[i];
		v0 = findvertex(l[0])
		v1 = findvertex(l[1])
		
		dx = v0[2] - v1[2]
		dy = v0[1] - v1[1]
		d  = math.sqrt(dx*dx + dy*dy)
		
		uv = []
		uv.append([d/16, 1.0])
		uv.append([0.0, 0.0])
		uv.append([d/16, 0.0])
		uv.append([0.0, 1.0])
		print(">>>>>>>>>>>>>>>> UV d=" + str(d) + " " + str(uv))
		
		all_uvs.append(uv)

def generateWavefrontobj(outputfilepath):
	# Faces [v0, v1, v2, v0_id, v1_id, v2_id, line_index
	faces           = []
	verticesWritten = 0
	content = "# Tau Software.\n"
	content = content + "o TODO_worldname\n"
	
	calc_normal_lines()
	calc_normal_vertices()
	calc_texture_uv()
	
	# Every line is defined with two vertices
	# Every face needs one square (two triangles)
	# The face is created with four vertices, where two are identical copies of
	# the previous two, except they are moved upward.
	for i in range(0, len(all_lines)):
		l  = all_lines[i];
		v0 = findvertex(l[0])
		v1 = findvertex(l[1])
		content = content + "v " + str(v0[1]) + " 0 " + str(v0[2]) + "\n"  # +1
		content = content + "v " + str(v0[1]) + " 16 " + str(v0[2]) + "\n" # +2
		content = content + "v " + str(v1[1]) + " 0 " + str(v1[2]) + "\n"  # +3
		content = content + "v " + str(v1[1]) + " 16 " + str(v1[2]) + "\n" # +4
		faces.append([verticesWritten + 4, verticesWritten + 1, verticesWritten + 3, v1[0], v0[0], v1[0], i])
		faces.append([verticesWritten + 4, verticesWritten + 2, verticesWritten + 1, v1[0], v0[0], v0[0], i])
		verticesWritten = verticesWritten + 4
	
	# Texture coordinates
	for i in range(0, len(all_uvs)):
		content = content + "vt " + str(all_uvs[i][0][0]) + " " + str(all_uvs[i][0][1]) + "\n"
		content = content + "vt " + str(all_uvs[i][1][0]) + " " + str(all_uvs[i][1][1])  + "\n"
		content = content + "vt " + str(all_uvs[i][2][0]) + " " + str(all_uvs[i][2][1]) + "\n"
		content = content + "vt " + str(all_uvs[i][3][0]) + " " + str(all_uvs[i][3][1]) + "\n"
	#content = content + "vt 1.0 1.0\n"
	#content = content + "vt 0.0 0.0\n"
	#content = content + "vt 1.0 0.0\n"
	#content = content + "vt 0.0 1.0\n"
	
	# Normals
	for i in range(0, len(all_vertexnormals)):
		vn = all_vertexnormals[i]
		content = content + "vn " + str(vn[0]) + " 0.0 " + str(vn[1]) + "\n"
	
	# Faces
	# Two triangles per quad.
	for i in range(0, len(faces), 2):
		# f v/vt/vn v/vt/vn v/vt/vn
		vn_0 = getvertexindexinlist(faces[i][3]) + 1
		vn_1 = getvertexindexinlist(faces[i][4]) + 1
		vn_2 = getvertexindexinlist(faces[i][5]) + 1
		vn1_0 = getvertexindexinlist(faces[i+1][3]) + 1
		vn1_1 = getvertexindexinlist(faces[i+1][4]) + 1
		vn1_2 = getvertexindexinlist(faces[i+1][5]) + 1
		
		# Everyt line has its own UV calculated. Line builds a wall of 4 vertices.
		uv0_offset = faces[i][6]*4     # the faces[i+1][6] and faces[i][6]
		uv1_offset = faces[i+1][6]*4   # should be the same!
		
		content = content + "f " + str(faces[i][0])     + "/" + str(uv0_offset+1) + "/" + str(vn_0)  + " " + str(faces[i][1])     + "/" + str(uv0_offset+2) +"/" + str(vn_1)  + " " + str(faces[i][2])     + "/" + str(uv0_offset+3) +"/" + str(vn_2)  + "\n"
		content = content + "f " + str(faces[i+1][0])   + "/" + str(uv1_offset+1) + "/" + str(vn1_0) + " " + str(faces[i+1][1])   + "/" + str(uv1_offset+4) + "/" + str(vn1_1) + " " + str(faces[i+1][2])   + "/"+ str(uv1_offset+2) + "/" + str(vn1_2) + "\n"
		#content = content + "f " + str(faces[i][0])     + "/1/" + str(vn_0)  + " " + str(faces[i][1])     + "/2/" + str(vn_1)  + " " + str(faces[i][2])     + "/3/" + str(vn_2)  + "\n"
		#content = content + "f " + str(faces[i+1][0])   + "/1/" + str(vn1_0) + " " + str(faces[i+1][1])   + "/4/" + str(vn1_1) + " " + str(faces[i+1][2])   + "/2/" + str(vn1_2) + "\n"
	
	print("Content:")
	print(content)
	file = open(outputfilepath, "w")
	file.write(content)
	file.close()
	

def readfile(fil):
	global all_vertices
	global all_lines
	global all_tris
	global all_entities
	allcontent = None
	
	lenvertices = 0
	lenlines = 0
	lentris = 0
	lensectors = 0
	
	with open(fil, "rb") as f:
		allcontent = f.readlines()
	
	allcontent[0] = str(allcontent[0], "utf-8")
	allcontent[1] = str(allcontent[1], "utf-8")
	lengths       = allcontent[0].split(';')
	lenvertices   = int(lengths[0])
	lenlines      = int(lengths[1])
	lentris       = int(lengths[2])
	lenentities   = int(lengths[3])
	lentotal      = lenvertices + lenlines + lentris + lenentities;
	
	print("Lengths: ")
	print("\tVertices: " + str(lenvertices))
	print("\tLines:    " + str(lenlines))
	print("\tTris:     " + str(lentris))
	print("\tEntities: " + str(lenentities))
	print("\tTOTAL:    " + str(lentotal))
	
	data = allcontent[1].split(';')
	del data[-1] # Always an empty element because of the previous split
	
	print("Reading data...")
	for i in range(0, lenvertices):
		v = data[i]
		v = v.split(",")
		v[0] = int(v[0])
		v[1] = int(v[1])
		v[2] = int(v[2])
		all_vertices.append(v)
		
	for i in range(0, lenlines):
		l = data[i + lenvertices]
		l = l.split(",")
		l[0] = int(l[0])
		l[1] = int(l[1])
		l[2] = int(l[2])
		all_lines.append(l)
	
	for i in range(0, lentris):
		t = data[i + lenvertices + lenlines]
		t = t.split(",")
		t[0] = int(t[0])
		t[1] = int(t[1])
		t[2] = int(t[2])
		t[3] = int(t[3])
		all_tris.append(t)
		
	for i in range(0, lenentities):
		e = data[i + lenvertices + lenlines + lentris]
		e = e.split(",")
		e[0] = int(e[0])
		e[1] = int(e[1])
		e[2] = int(e[2])
		e[3] = int(e[3])
		e[4] = int(e[4])
		e[5] = int(e[5])
		all_entities.append(e)
	
	print("Final data:")
	print("Vertices: " + str(all_vertices))
	print("Lines:    " + str(all_lines))
	print("Tris:     " + str(all_tris))
	print("Entities: " + str(all_entities))
	print("=========")
	

if __name__ == "__main__":
	if len(sys.argv) < 3:
		print ("Expected map file. Usage is: \"" + sys.argv[0] + " inputfile outputfile\"")
		sys.exit(1)
		
	readfile(sys.argv[1])
	generateWavefrontobj(sys.argv[2])