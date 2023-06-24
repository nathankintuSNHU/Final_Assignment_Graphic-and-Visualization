#include <iostream>         
#include <vector>
#include <cstdlib>          
#include <GL/glew.h>        
#include <GLFW/glfw3.h>  

//GLM Math
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h> // Image loading Utility functions

using namespace std;

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

namespace {
	const char* const WINDOW_TITLE = "Nathan Kintu - Final Project";

	//initial window width and height
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;
	const float PI = 3.14159265359;

	//GLMesh struct with vertex array, vertext buffer array, and vertex values
	struct GLMesh {
		GLuint vArray;
		GLuint vBufferArray[2];
		GLuint vColorArray;
		//GLuint indNum;
		GLuint vertNum;
	};


	GLFWwindow* GLwindow = nullptr;

	GLMesh planeMesh;
	GLMesh planeMesh2;
	GLMesh boxMesh;
	GLMesh boxMesh2;
	GLMesh boxMesh3;
	GLMesh boxMesh4;
	GLMesh pyramidMesh;
	GLMesh cylinderMesh;
	GLMesh cylinderMesh2;
	GLMesh cylinderMesh3;
	GLMesh ballMesh;
	GLMesh ballMesh2;

	GLuint progId;
	//Camera
	glm::vec3 camPos = glm::vec3(1.5f, 3.0f, 4.0f);
	glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -2.0f);
	glm::vec3 camUp = glm::vec3(0.0f, 3.0f, 1.0f);

	float lX = WINDOW_WIDTH / 2.0f;
	float lY = WINDOW_HEIGHT / 2.0f;
	bool fMouse = true;
	float camSpeed = 5.5f;
	float sMult = 0.4;
	float mSensitivity = 0.1f;
	float cYaw = -90.0f;
	float cPitch = 0.0f;

	const char* pyramidText = "../../resources/textures/texture_3.jpg";
	const char* boxText = "../../resources/textures/scene_texture1.jpg";//
	const char* boxText2 = "../../resources/textures/remote_texture.jpg";//
	const char* boxText3 = "../../resources/textures/leaves.jpg";//
	const char* boxText4 = "../../resources/textures/remote_texture.jpg";//
	const char* tableText = "../../resources/textures/wood_top.jpg";//
	const char* bottleBodyText = "../../resources/textures/tile_texture.jpg";
	const char* bottleNeckText = "../../resources/textures/tile_texture.jpg";
	const char* ballText = "../../resources/textures/orange_top.jpg";
	const char* tapeRollText = "../../resources/textures/texture_3.jpg";
	const char* tapeText = "../../resources/textures/texture_3.jpg";


	GLuint pyramidTextId;
	GLuint boxTextId;
	GLuint boxText2Id;
	GLuint boxText3Id;
	GLuint boxText4Id;
	GLuint tableTextId;
	GLuint bottleBodyTextId;
	GLuint bottleNeckTextId;
	GLuint ballTextId;
	GLuint tapeRollTextId;
	GLuint tapeTextId;

	bool orthBool = false;

	float dTime = 0.0f; // time between current frame and last frame
	float lFrame = 0.0f;

	static glm::vec2 UVScale(1.0f, 1.0f);

	glm::vec3 diffLightColor(1.0f, 0.95f, 0.95f);
	glm::vec3 ambiLightColor(1.0f, 1.0f, 1.0f);
	glm::vec3 specLightColor(0.2f, 0.1f, 0.1f);

	glm::vec3 lightPos(0.0f, 2.0f, 1.0f);
	glm::vec3 lightScale(0.8f);
	glm::vec3 gObjectColor(1.0f, 1.0f, 1.0f);


	glm::vec3 diffLightColor2(0.6f, 0.6f, 0.7f);
	glm::vec3 ambiLightColor2(0.0f, 0.0f, 0.0f);
	glm::vec3 specLightColor2(0.4f, 0.4f, 0.5f);

	glm::vec3 lightPos2(-6.0f, 2.0f, 1.0f);

}



//foreward declarations
bool initUI(int, char* [], GLFWwindow** window);
void resizeWindow(GLFWwindow* window, int width, int height);
void destroyMesh(GLMesh& mesh);
void createShaderProg(const char* vertShad, const char* fragShad, GLuint& Id);
void destroyProgram(GLuint Id);
void procInput(GLFWwindow* window);
void mousePosCall(GLFWwindow* window, double xpos, double ypos);
void mouseScrollCall(GLFWwindow* window, double xoffset, double yoffset);
bool createTexture(const char* filename, GLuint& textureId);


const GLchar* VERT_SHAD_SRC = GLSL(440,
	layout(location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
layout(location = 1) in vec4 color;  // Color data from Vertex Attrib Pointer 1
layout(location = 2) in vec2 textureCoordinate;  // Color data from Vertex Attrib Pointer 1
layout(location = 3) in vec3 normals; //normals from vertex pointer 3


out vec4 vertexColor; // variable to transfer color data to the fragment shader
out vec2 newTextureCoords;
out vec3 vertNormals;
out vec3 vertexFragmentPos;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertex data using matrix

	vertexFragmentPos = vec3(model * vec4(position, 1.0f));

	vertexColor = color; // references incoming color data

	newTextureCoords = textureCoordinate;

	vertNormals = mat3(transpose(inverse(model))) * normals;
}
);

// Fragment Shader
const GLchar* FRAG_SHAD_SRC = GLSL(440,

	in vec2 newTextureCoords;
in vec4 vertexColor;
in vec3 vertexFragmentPos;
in vec3 vertNormals;

out vec4 FragColor;

uniform vec2 uvScale;
uniform sampler2D newTexture;

uniform vec3 objectColor;
uniform vec3 diffLightColor;
uniform vec3 ambiLightColor;
uniform vec3 specLightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;


uniform vec3 diffLightColor2;
uniform vec3 ambiLightColor2;
uniform vec3 specLightColor2;
uniform vec3 lightPos2;

void main() {

	/*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

	/*Calculate Ambient lighting*/
	float ambientStrength = 0.2f; // Set ambient or global lighting strength.
	vec3 ambient = ambientStrength * ambiLightColor; // Generate ambient light color.

	/*Calculate Diffuse lighting*/
	vec3 norm = normalize(vertNormals); // Normalize vectors to 1 unit.
	vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube.
	float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light.
	vec3 diffuse = impact * diffLightColor; // Generate diffuse light color.

	/*Calculate Specular lighting*/
	float specularIntensity = 0.2f; // Set specular light strength.
	float highlightSize = 3.0f; // Set specular highlight size.
	vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction.
	vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector.

	/*Calculate specular component.*/
	float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
	vec3 specular = specularIntensity * specularComponent * specLightColor;

	// Texture holds the color to be used for all three components
	vec4 textureColor = texture(newTexture, newTextureCoords * uvScale);

	/*Calculate Phong result.*/
	vec3 phong = (ambient + diffuse + specular) * textureColor.xyz * objectColor * vec3(vertexColor);



	/*Calculate Ambient lighting*/
	float ambientStrength2 = 0.0f; // Set ambient or global lighting strength.
	vec3 ambient2 = ambientStrength2 * ambiLightColor2; // Generate ambient light color.

	/*Calculate Diffuse lighting*/
	vec3 lightDirection2 = normalize(lightPos2 - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube.
	float impact2 = max(dot(norm, lightDirection2), 0.0);// Calculate diffuse impact by generating dot product of normal and light.
	vec3 diffuse2 = impact2 * diffLightColor2; // Generate diffuse light color.

	/*Calculate Specular lighting*/
	float specularIntensity2 = 0.2f; // Set specular light strength.
	float highlightSize2 = 3.0f; // Set specular highlight size.
	vec3 reflectDir2 = reflect(-lightDirection2, norm);// Calculate reflection vector.

	/*Calculate specular component.*/
	float specularComponent2 = pow(max(dot(viewDir, reflectDir2), 0.0), highlightSize2);
	vec3 specular2 = specularIntensity2 * specularComponent2 * specLightColor2;


	/*Calculate Phong result.*/
	vec3 phong2 = (ambient2 + diffuse2 + specular2) * textureColor.xyz * objectColor * vec3(vertexColor);





	FragColor = vec4((phong + phong2), 1.0f);//vertexColor + texture(newTexture, newTextureCoords) + vertexColor;
}
);




//initialize window
bool initUI(int argc, char* argv[], GLFWwindow** window) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	//Creates a new window

	* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
	if (*window == NULL) {
		std::cout << "Window Creation Failed" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, resizeWindow);
	glfwSetCursorPosCallback(*window, mousePosCall);
	glfwSetScrollCallback(*window, mouseScrollCall);


	glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	GLenum GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult)
	{
		std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
		return false;
	}



	return true;
}

//resize viewport given new width and height
void resizeWindow(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);

	return;
}


std::vector<GLfloat> genCircle(int sectNum, glm::vec3 center, float radius, glm::vec3 centMod) {
	std::vector<GLfloat> result;

	glm::vec3 xyz(0.0f, 0.0f, 0.0f);
	xyz = xyz;
	glm::vec3 Cxyz;
	Cxyz = xyz;

	std::vector<float> unitCircleVertices;
	centMod = center + centMod;

	for (int i = 0; i <= sectNum; i++) {
		float theta = 2.0f * PI * float(i) / float(sectNum);
		xyz.x = radius * cosf(theta);//calculate the x component 
		xyz.y = radius * sinf(theta);//calculate the y component 

		for (int j = 0; j < 3; j++) {
			//Step One
			//Gets the coords for the previous point on the circle
			if (j % 3 == 0) {
				result.push_back(Cxyz.x + center.x);
				result.push_back(Cxyz.y + center.y);
				result.push_back(Cxyz.z + center.z);
			}
			//Step 2
			//Get and set point the next point on the circle
			if (j % 3 == 1) {
				result.push_back(xyz.x + center.x);
				result.push_back(xyz.y + center.y);
				result.push_back(xyz.z + center.z);
			};
			//step3
			//Sets the center point
			if (j % 3 == 2) {
				result.push_back(centMod.x);
				result.push_back(centMod.y);
				result.push_back(centMod.z);
			};
		}
		Cxyz = xyz;

	}

	return result;
}

std::vector<GLfloat> genNormals(vector<GLfloat> verts, GLuint floatsPerVertex, GLuint floatsPerColor, GLuint floatsPerUV, GLuint floatsPerNormal) {

	glm::vec3 x0;
	glm::vec3 x1;
	glm::vec3 x2;

	glm::vec3 V1;
	glm::vec3 V2;
	//Resulting cross product var
	glm::vec3 crossProd;

	//Placeholder Variables
	int x1Val = 0;
	int x2Val = 0;

	int i = 0;
	int j = 0;

	while ((i + 1) <= verts.size()) {

		if (j == 0) {
			//saves curent array index
			x1Val = i;

			//Collect first vertex positions
			x0[0] = verts[i];
			x0[1] = verts[i + 1];
			x0[2] = verts[i + 2];

		}
		else if (j == 1) {
			//saves curent array index
			x2Val = i;

			//Collect second vertex positions
			x1[0] = verts[i];
			x1[1] = verts[i + 1];
			x1[2] = verts[i + 2];

		}
		else {
			//Collect third vertex positions
			x2[0] = verts[i];
			x2[1] = verts[i + 1];
			x2[2] = verts[i + 2];

			//Gets vectors V1 and V2
			V1 = x1 - x0;
			V2 = x2 - x0;

			//takes the cross product of vectors V1 and V2
			crossProd = glm::cross(V2, V1);

			//replaces temp normals with calculated normals
			verts[floatsPerVertex + floatsPerUV + floatsPerColor + x1Val] = crossProd[0];
			verts[floatsPerVertex + floatsPerUV + floatsPerColor + x1Val + 1] = crossProd[1];
			verts[floatsPerVertex + floatsPerUV + floatsPerColor + x1Val + 2] = crossProd[2];

			verts[floatsPerVertex + floatsPerUV + floatsPerColor + x2Val] = crossProd[0];
			verts[floatsPerVertex + floatsPerUV + floatsPerColor + x2Val + 1] = crossProd[1];
			verts[floatsPerVertex + floatsPerUV + floatsPerColor + x2Val + 2] = crossProd[2];

			verts[floatsPerVertex + floatsPerUV + floatsPerColor + i] = crossProd[0];
			verts[floatsPerVertex + floatsPerUV + floatsPerColor + i + 1] = crossProd[1];
			verts[floatsPerVertex + floatsPerUV + floatsPerColor + i + 2] = crossProd[2];

		}

		i = i + floatsPerVertex + floatsPerUV + floatsPerColor + floatsPerNormal;
		if (j != 2) {
			j++;
		}
		else {
			j = 0;
		}
	}


	return verts;
}


//creates cylinder flower vase
void createCylinderMesh(GLMesh& mesh) {//, int text) {
	std::vector<GLfloat> verts;

	std::vector<GLfloat> newVerts;

	std::vector<GLfloat> textVerts({
			0.0f, 2.0f,
			2.0f, 0.0f,
			2.0f, 0.5f, 

		});

	//values for a vertex location
	const GLuint floatsPerVertex = 3;

	//RGB values (R, G, B, Opacity)
	const GLuint floatsPerColor = 4;//

	//Texture Coordinate Values
	const GLuint floatsPerUV = 2;

	//Normal values
	const GLuint floatsPerNormal = 3;

	std::vector<float> unitCircleVerticesTop;
	glm::vec3 centerTop(1.0f, 0.5f, -0.3f);

	std::vector<float> unitCircleVerticesBottom;
	glm::vec3 centerBottom(1.2f, 1.2f, -0.001f);


	std::vector<float> cylinderWalls;


	int sectorNum = 50;
	float radius = 0.5f;
	glm::vec3 centMod1(0.0f, 0.0f, 0.2f);
	glm::vec3 centMod2(0.0f, 0.0f, 0.0f);
	glm::vec3 a;
	glm::vec3 b;
	glm::vec3 c;
	glm::vec3 d;
	unitCircleVerticesTop = genCircle(sectorNum, centerTop, radius, centMod1);
	unitCircleVerticesBottom = genCircle(sectorNum, centerBottom, radius, centMod2);

	int count = 0;

	for (int i = 0; i < unitCircleVerticesTop.size(); i = i + (floatsPerVertex)) {
		if (count % 3 == 0) {
			a.x = unitCircleVerticesTop[i];
			a.y = unitCircleVerticesTop[i + 1];
			a.z = unitCircleVerticesTop[i + 2];

			c.x = unitCircleVerticesBottom[i];
			c.y = unitCircleVerticesBottom[i + 1];
			c.z = unitCircleVerticesBottom[i + 2];
		}
		if (count % 3 == 1) {
			b.x = unitCircleVerticesTop[i];
			b.y = unitCircleVerticesTop[i + 1];
			b.z = unitCircleVerticesTop[i + 2];

			d.x = unitCircleVerticesBottom[i];
			d.y = unitCircleVerticesBottom[i + 1];
			d.z = unitCircleVerticesBottom[i + 2];


			cylinderWalls.push_back(b.x);
			cylinderWalls.push_back(b.y);
			cylinderWalls.push_back(b.z);

			cylinderWalls.push_back(a.x);
			cylinderWalls.push_back(a.y);
			cylinderWalls.push_back(a.z);

			cylinderWalls.push_back(c.x);
			cylinderWalls.push_back(c.y);
			cylinderWalls.push_back(c.z);


			cylinderWalls.push_back(b.x);
			cylinderWalls.push_back(b.y);
			cylinderWalls.push_back(b.z);

			cylinderWalls.push_back(c.x);
			cylinderWalls.push_back(c.y);
			cylinderWalls.push_back(c.z);

			cylinderWalls.push_back(d.x);
			cylinderWalls.push_back(d.y);
			cylinderWalls.push_back(d.z);

		}
		count++;
	}



	newVerts.insert(newVerts.end(), unitCircleVerticesTop.begin(), unitCircleVerticesTop.end());
	newVerts.insert(newVerts.end(), unitCircleVerticesBottom.begin(), unitCircleVerticesBottom.end());
	newVerts.insert(newVerts.end(), cylinderWalls.begin(), cylinderWalls.end());


	int counter = 0;
	for (int i = 0; i < newVerts.size(); i = i + (floatsPerVertex)) {
		for (int j = 0; j < floatsPerVertex; j++) {
			verts.push_back(newVerts[i + j]);
		};

		for (int k = 0; k < floatsPerColor; k++) {
			if (k % 4 == 0) { verts.push_back(1.0f); };
			if (k % 4 == 1) { verts.push_back(1.0f); };
			if (k % 4 == 2) { verts.push_back(1.0f); };
			if (k % 4 == 3) { verts.push_back(1.0f); };
		};

		for (int m = 0; m < floatsPerUV; m++) {
			verts.push_back(textVerts[(counter % 3) + m]);
		};

		for (int n = 0; n < floatsPerNormal; n++) {
			verts.push_back(0.0f);
		};
		counter++;
	};

	verts = genNormals(verts, floatsPerVertex, floatsPerColor, floatsPerUV, floatsPerNormal);

	mesh.vertNum = (verts.size() * sizeof(float)) / (sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV + floatsPerNormal));

	//generates new array and array buffer
	glGenVertexArrays(1, &mesh.vArray);
	glGenBuffers(2, mesh.vBufferArray);

	glBindVertexArray(mesh.vArray);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vBufferArray[0]);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vBufferArray[1]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_STATIC_DRAW);

	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV + floatsPerNormal);

	GLfloat vertexArray = stride;

	//vertex location values
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	//vertex color values
	glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	//Texture values
	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV)));
	glEnableVertexAttribArray(3);
}

//Creates table
void createPlaneMesh(GLMesh& mesh) {//, int text) {

	std::vector<GLfloat> verts(
		{

			// Vertex Positions    // Colors (r,g,b,a) //Texture coordinates //Temp Normals

			//Plane
			-3.0f, -3.0f, 0.0f,   1.0f, 1.0f, 1.0f, 0.0f,   1.0f, 1.0f,   1.0f, 1.0f, 0.0f,
			 2.0f, -3.0f, 0.0f,   1.0f, 1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   1.0f, 1.0f, 0.0f,
			-3.0f,  3.0f, 0.0f,   1.0f, 1.0f, 1.0f, 0.0f,   1.0f, 0.0f,   1.0f, 1.0f, 0.0f,

			-3.0f,  3.0f, 0.0f,   1.0f, 1.0f, 1.0f, 0.0f,   1.0f, 0.0f,   0.0f, 0.0f, 0.0f,
			 2.0f, -3.0f, 0.0f,   1.0f, 1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 0.0f,
			 2.0f,  3.0f, 0.0f,   1.0f, 1.0f, 1.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,

		});
	//0.0f, 0.0f, 0.0f,

	//values for a vertex location
	const GLuint floatsPerVertex = 3;

	//RGB values (R, G, B, Opacity)
	const GLuint floatsPerColor = 4;//

	//Texture Coordinate Values
	const GLuint floatsPerUV = 2;

	//Normal values
	const GLuint floatsPerNormal = 3;


	verts = genNormals(verts, floatsPerVertex, floatsPerColor, floatsPerUV, floatsPerNormal);

	mesh.vertNum = (verts.size() * sizeof(float)) / (sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV + floatsPerNormal));

	//generates new array and array buffer
	glGenVertexArrays(1, &mesh.vArray);
	glGenBuffers(2, mesh.vBufferArray);

	glBindVertexArray(mesh.vArray);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vBufferArray[0]);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vBufferArray[1]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_STATIC_DRAW);

	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV + floatsPerNormal);

	GLfloat vertexArray = stride;

	//vertex location values
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	//vertex color values
	glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	//Texture values
	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV)));
	glEnableVertexAttribArray(3);
}


//Creates box mesh 
void createBoxMesh(GLMesh& mesh) {//, int text) {
	std::vector<GLfloat> verts;

	std::vector<GLfloat> newVerts(
		{

			// Vertex Positions    // Colors (r,g,b,a) //Texture coordinates //Temp Normals
		//Top
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.0f,
		 0.5f, -0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,
		-0.5f,  0.5f,  0.0f,
		-0.5f, -0.5f,  0.0f,

		-0.5f,  0.5f,  0.0f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.0f,
		-0.5f,  0.5f,  0.0f,

		 0.5f,  0.5f,  0.0f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.0f,
		 0.5f, -0.5f,  0.0f,
		-0.5f, -0.5f,  0.0f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,
		-0.5f,  0.5f,  0.0f,
		-0.5f,  0.5f, -0.5f,

		});

	std::vector<GLfloat> textVerts({
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,

		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,

		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,

		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,

		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		});


	//values for a vertex location
	const GLuint floatsPerVertex = 3;

	//RGB values (R, G, B, Opacity)
	const GLuint floatsPerColor = 4;//

	//Texture Coordinate Values
	const GLuint floatsPerUV = 2;

	//Normal values
	const GLuint floatsPerNormal = 3;

	glm::vec4 temp;

	glm::mat4 vRot = glm::rotate(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));


	for (int i = 0; i < newVerts.size(); i = i + (floatsPerVertex)) {
		temp[0] = newVerts[i];
		temp[1] = newVerts[i + 1];
		temp[2] = newVerts[i + 2];

		temp = temp * vRot;

		newVerts[i] = temp[0];
		newVerts[i + 1] = temp[1];
		newVerts[i + 2] = temp[2];

	}



	int counter = 0;
	for (int i = 0; i < newVerts.size(); i = i + (floatsPerVertex)) {
		for (int j = 0; j < floatsPerVertex; j++) {
			verts.push_back(newVerts[i + j]);
		};

		for (int k = 0; k < floatsPerColor; k++) {
			if (k % 4 == 0) { verts.push_back(1.0f); };
			if (k % 4 == 1) { verts.push_back(1.0f); };
			if (k % 4 == 2) { verts.push_back(1.0f); };
			if (k % 4 == 3) { verts.push_back(0.0f); };
		};

		for (int m = 0; m < floatsPerUV; m++) {
			verts.push_back(textVerts[(counter * 2) + m]);
		};

		for (int n = 0; n < floatsPerNormal; n++) {
			verts.push_back(0.0f);
		};
		counter++;
	};

	verts = genNormals(verts, floatsPerVertex, floatsPerColor, floatsPerUV, floatsPerNormal);

	mesh.vertNum = (verts.size() * sizeof(float)) / (sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV + floatsPerNormal));

	//generates new array and array buffer
	glGenVertexArrays(1, &mesh.vArray);
	glGenBuffers(2, mesh.vBufferArray);

	glBindVertexArray(mesh.vArray);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vBufferArray[0]);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vBufferArray[1]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_STATIC_DRAW);

	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV + floatsPerNormal);

	GLfloat vertexArray = stride;

	//vertex location values
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	//vertex color values
	glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	//Texture values
	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV)));
	glEnableVertexAttribArray(3);
}

//Creates box mesh (with no top or bottom(remote)
void createBoxMesh2(GLMesh& mesh) {//, int text) {
	std::vector<GLfloat> verts;

	std::vector<GLfloat> newVerts(
		{

		-1.5f, -1.5f, -0.2f,
		 0.5f, -1.5f, -0.2f,
		 0.5f, -1.0f, -0.2f,

		 0.5f, -1.0f, -0.2f,
		-1.5f, -1.0f, -0.2f,
		-1.5f, -1.5f, -0.2f,

		-1.5f, -1.5f, -0.0f,
		 0.5f, -1.5f, -0.0f,
		 0.5f, -1.0f, -0.0f,

		 0.5f, -1.0f,  0.0f,
		-1.5f, -1.0f,  0.0f,
		-1.5f, -1.5f,  0.0f,

		-1.5f, -1.0f,  0.0f,
		-1.5f, -1.0f, -0.2f,
		-1.5f, -1.5f,  0.0f,

		-1.5f, -1.5f, -0.2f,
		-1.5f, -1.5f,  0.0f,
		-1.5f, -1.0f, -0.2f,

		 0.5f, -1.0f,  0.0f,
		 0.5f, -1.0f, -0.2f,
		 0.5f, -1.5f, -0.2f,

		 0.5f, -1.5f, -0.2f,
		 0.5f, -1.5f,  0.0f,
		 0.5f, -1.0f,  0.0f,

		-1.5f, -1.5f, -0.2f,
		 0.5f, -1.5f, -0.2f,
		 0.5f, -1.5f,  0.0f,

		 0.5f, -1.5f,  0.0f,
		-1.5f, -1.5f,  0.0f,
		-1.5f, -1.5f, -0.2f,

		-1.5f, -1.0f, -0.2f,
		 0.5f, -1.0f, -0.2f,
		 0.5f, -1.0f,  0.0f,

		 0.5f, -1.0f,  0.0f,
		-1.5f, -1.0f,  0.0f,
		-1.5f, -1.0f, -0.2f,

		});

	std::vector<GLfloat> textVerts({
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,

		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,

		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,

		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,

		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		});


	//values for a vertex location
	const GLuint floatsPerVertex = 3;

	//RGB values (R, G, B, Opacity)
	const GLuint floatsPerColor = 4;//

	//Texture Coordinate Values
	const GLuint floatsPerUV = 2;

	//Normal values
	const GLuint floatsPerNormal = 3;

	glm::vec4 temp;

	glm::mat4 vRot = glm::rotate(glm::radians(150.0f), glm::vec3(0.0f, 0.0f, 1.0f));


	for (int i = 0; i < newVerts.size(); i = i + (floatsPerVertex)) {
		temp[0] = newVerts[i];
		temp[1] = newVerts[i + 1];
		temp[2] = newVerts[i + 2];

		temp = temp * vRot;

		newVerts[i] = temp[0];
		newVerts[i + 1] = temp[1];
		newVerts[i + 2] = temp[2];

	}



	int counter = 0;
	for (int i = 0; i < newVerts.size(); i = i + (floatsPerVertex)) {
		for (int j = 0; j < floatsPerVertex; j++) {
			verts.push_back(newVerts[i + j]);
		};

		for (int k = 0; k < floatsPerColor; k++) {
			if (k % 4 == 0) { verts.push_back(1.0f); };
			if (k % 4 == 1) { verts.push_back(1.0f); };
			if (k % 4 == 2) { verts.push_back(1.0f); };
			if (k % 4 == 3) { verts.push_back(0.0f); };
		};

		for (int m = 0; m < floatsPerUV; m++) {
			verts.push_back(textVerts[(counter * 2) + m]);
		};

		for (int n = 0; n < floatsPerNormal; n++) {
			verts.push_back(0.0f);
		};
		counter++;
	};

	verts = genNormals(verts, floatsPerVertex, floatsPerColor, floatsPerUV, floatsPerNormal);

	mesh.vertNum = (verts.size() * sizeof(float)) / (sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV + floatsPerNormal));

	//generates new array and array buffer
	glGenVertexArrays(1, &mesh.vArray);
	glGenBuffers(2, mesh.vBufferArray);

	glBindVertexArray(mesh.vArray);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vBufferArray[0]);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vBufferArray[1]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_STATIC_DRAW);

	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV + floatsPerNormal);

	GLfloat vertexArray = stride;

	//vertex location values
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	//vertex color values
	glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	//Texture values
	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV)));
	glEnableVertexAttribArray(3);
}


//Creates box mesh (with no top or bottom(remote2)
void createBoxMesh4(GLMesh& mesh) {//, int text) {
	std::vector<GLfloat> verts;

	std::vector<GLfloat> newVerts(
		{

		-1.5f, -2.5f, -0.1f,
		 0.5f, -2.5f, -0.1f,
		 0.5f, -2.0f, -0.1f,

		 0.5f, -2.0f, -0.1f,
		-1.5f, -2.0f, -0.1f,
		-1.5f, -2.5f, -0.1f,

		-1.5f, -2.5f, -0.0f,
		 0.5f, -2.5f, -0.0f,
		 0.5f, -2.0f, -0.0f,

		 0.5f, -2.0f,  0.0f,
		-1.5f, -2.0f,  0.0f,
		-1.5f, -2.5f,  0.0f,

		-1.5f, -2.0f,  0.0f,
		-1.5f, -2.0f, -0.1f,
		-1.5f, -2.5f,  0.0f,

		-1.5f, -2.5f, -0.1f,
		-1.5f, -2.5f,  0.0f,
		-1.5f, -2.0f, -0.1f,

		 0.5f, -2.0f,  0.0f,
		 0.5f, -2.0f, -0.1f,
		 0.5f, -2.5f, -0.1f,

		 0.5f, -2.5f, -0.1f,
		 0.5f, -2.5f,  0.0f,
		 0.5f, -2.0f,  0.0f,

		-1.5f, -2.5f, -0.1f,
		 0.5f, -2.5f, -0.1f,
		 0.5f, -2.5f,  0.0f,

		 0.5f, -2.5f,  0.0f,
		-1.5f, -2.5f,  0.0f,
		-1.5f, -2.5f, -0.1f,

		-1.5f, -2.0f, -0.1f,
		 0.5f, -2.0f, -0.1f,
		 0.5f, -2.0f,  0.0f,

		 0.5f, -2.0f,  0.0f,
		-1.5f, -2.0f,  0.0f,
		-1.5f, -2.0f, -0.1f,

		});

	std::vector<GLfloat> textVerts({
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,

		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,

		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,

		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,

		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		});


	//values for a vertex location
	const GLuint floatsPerVertex = 3;

	//RGB values (R, G, B, Opacity)
	const GLuint floatsPerColor = 4;//

	//Texture Coordinate Values
	const GLuint floatsPerUV = 2;

	//Normal values
	const GLuint floatsPerNormal = 3;

	glm::vec4 temp;

	glm::mat4 vRot = glm::rotate(glm::radians(170.0f), glm::vec3(0.0f, 0.0f, 1.0f));


	for (int i = 0; i < newVerts.size(); i = i + (floatsPerVertex)) {
		temp[0] = newVerts[i];
		temp[1] = newVerts[i + 1];
		temp[2] = newVerts[i + 2];

		temp = temp * vRot;

		newVerts[i] = temp[0];
		newVerts[i + 1] = temp[1];
		newVerts[i + 2] = temp[2];

	}



	int counter = 0;
	for (int i = 0; i < newVerts.size(); i = i + (floatsPerVertex)) {
		for (int j = 0; j < floatsPerVertex; j++) {
			verts.push_back(newVerts[i + j]);
		};

		for (int k = 0; k < floatsPerColor; k++) {
			if (k % 4 == 0) { verts.push_back(1.0f); };
			if (k % 4 == 1) { verts.push_back(1.0f); };
			if (k % 4 == 2) { verts.push_back(1.0f); };
			if (k % 4 == 3) { verts.push_back(0.0f); };
		};

		for (int m = 0; m < floatsPerUV; m++) {
			verts.push_back(textVerts[(counter * 2) + m]);
		};

		for (int n = 0; n < floatsPerNormal; n++) {
			verts.push_back(0.0f);
		};
		counter++;
	};

	verts = genNormals(verts, floatsPerVertex, floatsPerColor, floatsPerUV, floatsPerNormal);

	mesh.vertNum = (verts.size() * sizeof(float)) / (sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV + floatsPerNormal));

	//generates new array and array buffer
	glGenVertexArrays(1, &mesh.vArray);
	glGenBuffers(2, mesh.vBufferArray);

	glBindVertexArray(mesh.vArray);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vBufferArray[0]);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vBufferArray[1]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_STATIC_DRAW);

	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV + floatsPerNormal);

	GLfloat vertexArray = stride;

	//vertex location values
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	//vertex color values
	glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	//Texture values
	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV)));
	glEnableVertexAttribArray(3);
}

//Creates leaves
void createBoxMesh3(GLMesh& mesh) {//, int text) {
	std::vector<GLfloat> verts;

	std::vector<GLfloat> newVerts(
		{

		0.4f, 1.5f, -0.3f,
		0.6f, 0.8f, -0.2f,
		0.5f, 1.1f, -0.4f,

		0.2f, 1.6f, -0.3f,
		0.3f, 0.7f, -0.2f,
		0.5f, 1.2f, -0.4f,

		0.5f, 1.7f, -0.4f,
		0.3f, 1.4f, -0.3f,
		0.2f, 1.3f, -0.3f,

		0.5f, 1.4f, -0.3f,
		0.4f, 1.0f, -0.3f,
		0.3f, 1.5f, -0.2f,

		0.5f, 1.1f, -0.3f,
		0.4f, 1.2f, -0.7f,
		0.6f, 1.4f, -0.2f,

		0.3f, 1.6f, -0.2f,
		0.5f, 0.3f, -0.5f,
		0.2f, 1.2f, -0.3f,

		0.4f, 1.4f, -0.3f,
		0.6f, 0.5f, -0.4f,
		0.1f, 1.2f, -0.2f,

		0.5f, 0.8f, -0.4f,
		0.6f, 1.5f, -0.3f,
		0.3f, 0.3f, -0.1f,

		0.2f, 1.4f, -0.5f,
		0.2f, 0.7f, -0.4f,
		0.2f, 1.4f, -0.2f,

		0.2f, 1.0f, -0.2f,
		0.2f, 0.8f, -0.6f,
		0.2f, 1.2f, -0.1f,

		0.2f, 1.2f, -0.5f,
		0.2f, 0.7f, -0.3f,
		0.2f, 1.2f, -0.0f,

		0.2f, 1.2f, -0.3f,
		0.2f, 0.2f, -0.4f,
		0.2f, 1.2f, -0.3f,

		});

	std::vector<GLfloat> textVerts({
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,

		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,

		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,

		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,

		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		});


	//values for a vertex location
	const GLuint floatsPerVertex = 3;

	//RGB values (R, G, B, Opacity)
	const GLuint floatsPerColor = 4;//

	//Texture Coordinate Values
	const GLuint floatsPerUV = 2;

	//Normal values
	const GLuint floatsPerNormal = 3;

	glm::vec4 temp;

	glm::mat4 vRot = glm::rotate(glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));


	for (int i = 0; i < newVerts.size(); i = i + (floatsPerVertex)) {
		temp[0] = newVerts[i];
		temp[1] = newVerts[i + 1];
		temp[2] = newVerts[i + 2];

		temp = temp * vRot;

		newVerts[i] = temp[0];
		newVerts[i + 1] = temp[1];
		newVerts[i + 2] = temp[2];

	}



	int counter = 0;
	for (int i = 0; i < newVerts.size(); i = i + (floatsPerVertex)) {
		for (int j = 0; j < floatsPerVertex; j++) {
			verts.push_back(newVerts[i + j]);
		};

		for (int k = 0; k < floatsPerColor; k++) {
			if (k % 4 == 0) { verts.push_back(1.0f); };
			if (k % 4 == 1) { verts.push_back(1.0f); };
			if (k % 4 == 2) { verts.push_back(1.0f); };
			if (k % 4 == 3) { verts.push_back(0.0f); };
		};

		for (int m = 0; m < floatsPerUV; m++) {
			verts.push_back(textVerts[(counter * 2) + m]);
		};

		for (int n = 0; n < floatsPerNormal; n++) {
			verts.push_back(0.0f);
		};
		counter++;
	};

	verts = genNormals(verts, floatsPerVertex, floatsPerColor, floatsPerUV, floatsPerNormal);

	mesh.vertNum = (verts.size() * sizeof(float)) / (sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV + floatsPerNormal));

	//generates new array and array buffer
	glGenVertexArrays(1, &mesh.vArray);
	glGenBuffers(2, mesh.vBufferArray);

	glBindVertexArray(mesh.vArray);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vBufferArray[0]);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vBufferArray[1]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_STATIC_DRAW);

	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV + floatsPerNormal);

	GLfloat vertexArray = stride;

	//vertex location values
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	//vertex color values
	glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	//Texture values
	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV)));
	glEnableVertexAttribArray(3);
}


//destroy's a given mesh
void destroyMesh(GLMesh& mesh) {
	glDeleteVertexArrays(1, &mesh.vArray);
	glDeleteBuffers(1, mesh.vBufferArray);
	return;
}

//creates a new shader program
void createShaderProg(const char* vertShadSRC, const char* fragShadSRC, GLuint& Id) {

	//Create shader object
	Id = glCreateProgram();

	//create vertex shader
	GLuint vertShadId = glCreateShader(GL_VERTEX_SHADER);
	//create fragment shader
	GLuint fragShadId = glCreateShader(GL_FRAGMENT_SHADER);

	//get vertex shader
	glShaderSource(vertShadId, 1, &vertShadSRC, NULL);
	//get fragment shader
	glShaderSource(fragShadId, 1, &fragShadSRC, NULL);

	//compile vertex shader
	glCompileShader(vertShadId);
	//compile fragment shader
	glCompileShader(fragShadId);

	//atttach vertex shader
	glAttachShader(Id, vertShadId);
	//attach fragment shader
	glAttachShader(Id, fragShadId);

	glLinkProgram(Id);
}

//destroy's shader program
void destroyProgram(GLuint Id) {

	glDeleteProgram(Id);
	return;
}


void render() {
	// Enable Z-depth.
	glEnable(GL_DEPTH_TEST);

	// Clear the frame and Z buffers.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(progId);


	//Model translate, scale, and rotation

	// Scale model
	glm::mat4 mScale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
	// Rotate model
	glm::mat4 mRot = glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	// Translate model
	glm::mat4 mTran = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));

	//Apply model transform
	glm::mat4 model = mTran * mRot * mScale;


	//View translate and rotate

	glm::mat4 view = glm::lookAt(camPos, camPos + camFront, camUp);



	// Transformations are applied right-to-left order
	glm::mat4 projection;

	if (orthBool == true) {
		projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
	}
	else {
		projection = glm::perspective(45.0f, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
	};


	//Send transform to vertex shader
	GLint modelLoc = glGetUniformLocation(progId, "model");
	GLint viewLoc = glGetUniformLocation(progId, "view");
	GLint projLoc = glGetUniformLocation(progId, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//Positions
	GLint lightPositionLoc = glGetUniformLocation(progId, "lightPos");
	GLint viewPositionLoc = glGetUniformLocation(progId, "viewPosition");

	//Colors
	GLint diffLightColorLoc = glGetUniformLocation(progId, "diffLightColor");
	GLint ambiLightColorLoc = glGetUniformLocation(progId, "ambiLightColor");
	GLint specLightColorLoc = glGetUniformLocation(progId, "specLightColor");
	GLint objectColorLoc = glGetUniformLocation(progId, "objectColor");

	//sends color to GPU
	glUniform3f(diffLightColorLoc, diffLightColor.r, diffLightColor.g, diffLightColor.b);
	glUniform3f(ambiLightColorLoc, ambiLightColor.r, ambiLightColor.g, ambiLightColor.b);
	glUniform3f(specLightColorLoc, specLightColor.r, specLightColor.g, specLightColor.b);

	glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);


	//Positions
	glUniform3f(lightPositionLoc, lightPos.x, lightPos.y, lightPos.z);
	const glm::vec3 cameraPosition = camPos;
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);


	//Positions
	GLint lightPosition2Loc = glGetUniformLocation(progId, "lightPos2");

	//Colors
	GLint diffLightColor2Loc = glGetUniformLocation(progId, "diffLightColor2");
	GLint ambiLightColor2Loc = glGetUniformLocation(progId, "ambiLightColor2");
	GLint specLightColor2Loc = glGetUniformLocation(progId, "specLightColor2");
	GLint objectColor2Loc = glGetUniformLocation(progId, "objectColor2");

	//sends color to GPU
	glUniform3f(lightPosition2Loc, lightPos2.x, lightPos2.y, lightPos2.z);
	glUniform3f(diffLightColor2Loc, diffLightColor2.r, diffLightColor2.g, diffLightColor2.b);
	glUniform3f(ambiLightColor2Loc, ambiLightColor2.r, ambiLightColor2.g, ambiLightColor2.b);
	glUniform3f(specLightColor2Loc, specLightColor2.r, specLightColor2.g, specLightColor2.b);


	glm::vec2 uvMult(1.0f, 1.0f);

	GLint UVScaleLoc = glGetUniformLocation(progId, "uvScale");
	glUniform2fv(UVScaleLoc, 1, glm::value_ptr(uvMult));

	// Bind Table texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tableTextId);

	// Activates the buffer in the vertex array
	glBindVertexArray(planeMesh.vArray);
	glDrawArrays(GL_TRIANGLES, 0, planeMesh.vertNum);

	uvMult = { 1.0f, 1.0f };

	glUniform2fv(UVScaleLoc, 1, glm::value_ptr(uvMult));

	//Bind box texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, boxTextId);
	
	// Activates the buffer in the vertex array
	glBindVertexArray(boxMesh.vArray);
	glDrawArrays(GL_TRIANGLES, 0, boxMesh.vertNum);

	//Bind box texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, boxText2Id);

	// Activates the buffer in the vertex array
	glBindVertexArray(boxMesh2.vArray);
	glDrawArrays(GL_TRIANGLES, 0, boxMesh2.vertNum);


	//Bind box texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, boxText3Id);

	// Activates the buffer in the vertex array
	glBindVertexArray(boxMesh3.vArray);
	glDrawArrays(GL_TRIANGLES, 0, boxMesh3.vertNum);


	//Bind box texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, boxText4Id);

	// Activates the buffer in the vertex array
	glBindVertexArray(boxMesh4.vArray);
	glDrawArrays(GL_TRIANGLES, 0, boxMesh4.vertNum);


	//Bind pyramid texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pyramidTextId);


	// Activates the buffer in the vertex array
	glBindVertexArray(pyramidMesh.vArray);
	glDrawArrays(GL_TRIANGLES, 0, pyramidMesh.vertNum);

	//Bind Cylinder Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bottleBodyTextId);

	// Activates the buffer in the vertex array
	glBindVertexArray(cylinderMesh.vArray);
	glDrawArrays(GL_TRIANGLES, 0, cylinderMesh.vertNum);

	//Bind Cylinder Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bottleNeckTextId);

	// Activates the buffer in the vertex array
	glBindVertexArray(cylinderMesh2.vArray);
	glDrawArrays(GL_TRIANGLES, 0, cylinderMesh2.vertNum);

	//Bind Sphere Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ballTextId);

	// Activates the buffer in the vertex array
	glBindVertexArray(ballMesh.vArray);
	glDrawArrays(GL_TRIANGLES, 0, ballMesh.vertNum);

	// Activates the buffer in the vertex array
	glBindVertexArray(ballMesh2.vArray);
	glDrawArrays(GL_TRIANGLES, 0, ballMesh2.vertNum);

	//Bind Sphere Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tapeRollTextId);

	// Activates the buffer in the vertex array
	glBindVertexArray(cylinderMesh3.vArray);
	glDrawArrays(GL_TRIANGLES, 0, cylinderMesh3.vertNum);

	//Bind Sphere Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tapeTextId);

	// Activates the buffer in the vertex array
	glBindVertexArray(planeMesh2.vArray);
	glDrawArrays(GL_TRIANGLES, 0, planeMesh2.vertNum);



	//glDrawElements(GL_TRIANGLES, newMesh.indNum, GL_UNSIGNED_SHORT, NULL);
	//Stop vArray 
	glBindVertexArray(0);

	glfwSwapBuffers(GLwindow);

	return;

}


void procInput(GLFWwindow* window)
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);


	float camOff = camSpeed * dTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camPos += camOff * camFront;

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camPos -= camOff * camFront;

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camPos -= glm::normalize(glm::cross(camFront, camUp)) * camOff;

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camPos += glm::normalize(glm::cross(camFront, camUp)) * camOff;

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camPos -= camOff * camUp;

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camPos += camOff * camUp;

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		if (orthBool == false) {
			orthBool = true;
		}
		else {
			orthBool = false;
		}
}

void mousePosCall(GLFWwindow* window, double X, double Y)
{
	if (fMouse)
	{
		lX = X;
		lY = Y;
		fMouse = false;
	}

	float xOff = X - lX;
	float yOff = lY - Y;

	lX = X;
	lY = Y;

	cYaw += xOff * mSensitivity;
	cPitch += yOff * mSensitivity;

	if (cPitch > 89.0f)
		cPitch = 89.0f;
	if (cPitch < -89.0f)
		cPitch = -89.0f;

	camFront.x = cos(glm::radians(cYaw)) * cos(glm::radians(cPitch));
	camFront.y = sin(glm::radians(cPitch));
	camFront.z = sin(glm::radians(cYaw)) * cos(glm::radians(cPitch));
	camFront = glm::normalize(camFront);
}

void mouseScrollCall(GLFWwindow* window, double X, double Y)
{
	camSpeed += Y * sMult;
	if (camSpeed < 0) {
		camSpeed = 0;
	}
}


bool createTexture(const char* filename, GLuint& textureId) {
	int height;
	int width;
	int channels;

	unsigned char* newImg = stbi_load(filename, &width, &height, &channels, 0);

	std::cout << "Loading image" << endl;

	if (newImg != nullptr) {

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		// Set the texture wrapping parameters.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


		// Set texture filtering parameters.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, newImg);
		else if (channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, newImg);
		else
		{
			std::cout << "Not implemented to handle image with " << channels << " channels" << endl;
			return false;
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		std::cout << "Image Loaded" << endl;

		stbi_image_free(newImg);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture.

		return true;
	}

	std::cout << "Image failed to load" << endl;
	return false;
}


//Main function
int main(int argc, char* argv[]) {
	GLFWwindow* window = nullptr;

	//if not initiated, program has failed to load
	if (!initUI(argc, argv, &GLwindow))
		return EXIT_FAILURE;


	if (!createTexture(pyramidText, pyramidTextId))
	{
		std::cout << "Failed to load texture " << pyramidText << endl;
		return EXIT_FAILURE;
	}

	if (!createTexture(boxText, boxTextId))
	{
		std::cout << "Failed to load texture " << boxText << endl;
		return EXIT_FAILURE;
	}

	if (!createTexture(boxText2, boxText2Id))
	{
		std::cout << "Failed to load texture " << boxText2 << endl;
		return EXIT_FAILURE;
	}

	if (!createTexture(boxText3, boxText3Id))
	{
		std::cout << "Failed to load texture " << boxText3 << endl;
		return EXIT_FAILURE;
	}

	if (!createTexture(boxText4, boxText4Id))
	{
		std::cout << "Failed to load texture " << boxText4 << endl;
		return EXIT_FAILURE;
	}

	if (!createTexture(tableText, tableTextId))
	{
		std::cout << "Failed to load texture " << tableText << endl;
		return EXIT_FAILURE;
	}

	if (!createTexture(bottleBodyText, bottleBodyTextId))
	{
		std::cout << "Failed to load texture " << bottleBodyText << endl;
		return EXIT_FAILURE;
	}

	if (!createTexture(bottleNeckText, bottleNeckTextId))
	{
		std::cout << "Failed to load texture " << bottleNeckText << endl;
		return EXIT_FAILURE;
	}

	if (!createTexture(ballText, ballTextId))
	{
		std::cout << "Failed to load texture " << ballText << endl;
		return EXIT_FAILURE;
	}

	if (!createTexture(tapeRollText, tapeRollTextId))
	{
		std::cout << "Failed to load texture " << tapeRollText << endl;
		return EXIT_FAILURE;
	}

	if (!createTexture(tapeText, tapeTextId))
	{
		std::cout << "Failed to load texture " << tapeText << endl;
		return EXIT_FAILURE;
	}

	// Tell OpenGL for each sampler which texture unit it belongs to (only has to be done once).
	glUseProgram(progId);

	//creates new mesh
	createPlaneMesh(planeMesh);//, planeMeshVerts);
	createBoxMesh(boxMesh);
	createBoxMesh2(boxMesh2);
	createBoxMesh3(boxMesh3);
	createBoxMesh4(boxMesh4);
	createCylinderMesh(cylinderMesh);


	//creates new shader program
	createShaderProg(VERT_SHAD_SRC, FRAG_SHAD_SRC, progId);


	//set background color to black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


	//render loop
	while (!glfwWindowShouldClose(GLwindow)) {

		procInput(GLwindow);

		render();

		glfwPollEvents();

		float currFrame = glfwGetTime();

		dTime = currFrame - lFrame;
		lFrame = currFrame;

	}

	destroyMesh(planeMesh);
	destroyMesh(boxMesh);
	destroyMesh(boxMesh2);
	destroyMesh(boxMesh3);
	destroyMesh(boxMesh4);
	destroyMesh(cylinderMesh);
	destroyProgram(progId);


	exit(EXIT_SUCCESS);


}