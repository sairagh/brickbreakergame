#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

/*#include <thread>
#include <ao/ao.h>
#include <mpg123.h>*/
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
typedef long long int ll;
typedef float fl;
struct VAO {
  GLuint VertexArrayID;
  GLuint VertexBuffer;
  GLuint ColorBuffer;

  GLenum PrimitiveMode;
  GLenum FillMode;
  int NumVertices;
};
typedef struct VAO VAO;

struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
} Matrices;

GLuint programID;


struct rect
{
  VAO *rec;
  fl x,y,l,b,x_speed,y_speed,angle,color;
  int flag;
  float trans;
  int flagc;
};
//float zoom=1.0,pan=0,pany=0,zoomin=0,zoomout=0,panleft=0,panright=0,panup=0,pandown=0,rightclick=0,leftclick=0;
int flagr=0,flagb=0;
int flagq=0;
int flagsp=0;
float leftclick=0,rightclick=0;
double intx,inty,finx,finy;
double intx1,inty1,finx1,finy1;
ll vis[1000];
float camera_rotation_angle = 90;
float rectangle_rotation = 0;
float triangle_rotation = 0;
float g=0,h=0;
int flag111=0,flag112=0;
//vector<struct rect> bricks;
struct rect mirrors[4];
struct rect recta[1000];
//for laser canon
struct rect laser[3];
struct rect cannon[100];
struct rect numbers[10];
float brickspeed=0.005;
int flagp=0;
double last_update_time1, current_time1;

//laser[0].angle=0;
//laser[2].angle=0;
/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}

void quit(GLFWwindow *window)
{
  glfwDestroyWindow(window);
  glfwTerminate();
//    exit(EXIT_SUCCESS);
}


/* Generate VAO, VBOs and return VAO handle */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
  struct VAO* vao = new struct VAO;
  vao->PrimitiveMode = primitive_mode;
  vao->NumVertices = numVertices;
  vao->FillMode = fill_mode;

    // Create Vertex Array Object
    // Should be done after CreateWindow and before any other GL calls
    glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
    glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
    glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors

    glBindVertexArray (vao->VertexArrayID); // Bind the VAO 
    glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices 
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
    glVertexAttribPointer(
                          0,                  // attribute 0. Vertices
                          3,                  // size (x,y,z)
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );

    glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors 
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
    glVertexAttribPointer(
                          1,                  // attribute 1. Color
                          3,                  // size (r,g,b)
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );

    return vao;
  }

/* Generate VAO, VBOs and return VAO handle - Common Color for all vertices */
  struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
  {
    GLfloat* color_buffer_data = new GLfloat [3*numVertices];
    for (int i=0; i<numVertices; i++) {
      color_buffer_data [3*i] = red;
      color_buffer_data [3*i + 1] = green;
      color_buffer_data [3*i + 2] = blue;
    }

    return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
  }

/* Render the VBOs handled by VAO */
  void draw3DObject (struct VAO* vao)
  {
    // Change the Fill Mode for this object
    glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

    // Bind the VAO to use
    glBindVertexArray (vao->VertexArrayID);

    // Enable Vertex Attribute 0 - 3d Vertices
    glEnableVertexAttribArray(0);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

    // Enable Vertex Attribute 1 - Color
    glEnableVertexAttribArray(1);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);

    // Draw the geometry !
    glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
  }

/**************************
 * Customizable functions *
 **************************/
  float xpos=0;
float ypos=0;
float zoom=1;
GLFWwindow* window;
int flag12=0;

void mousezoom(GLFWwindow* window, double xoffset, double yoffset)
{
  if (yoffset==-1) { 
    zoom /= 1.1; 
  }
  else if(yoffset==1){
    zoom *= 1.1; 
  }
  if (zoom<=1) {
    zoom = 1;
  }
  if (zoom>=2) {
    zoom=2;
  }


if(xpos-4.0f/zoom<-4)
    xpos=-4+4.0f/zoom;
  else if(xpos+4.0f/zoom>4)
    xpos=4-4.0f/zoom;
  if(ypos-4.0f/zoom<-4)
    ypos=-4+4.0f/zoom;
  else if(ypos+4.0f/zoom>4)
    ypos=4-4.0f/zoom;
Matrices.projection = glm::ortho((float)(-4.0f/zoom+xpos), (float)(4.0f/zoom+xpos), (float)(-4.0f/zoom+ypos), (float)(4.0f/zoom+ypos), 0.1f, 500.0f);
 // Matrices.projection = glm::ortho((float)(-4.0f/zoom+xpos), (float)(4.0f/zoom+xpos), (float)(-4.0f/zoom+ypos), (float)(4.0f/zoom+ypos), 0.1f, 500.0f);
}
//glfwSetScrollCallback(window, mousezoom);

void pan(){
  if(xpos-4.0f/zoom<-4)
    xpos=-4+4.0f/zoom;
  else if(xpos+4.0f/zoom>4)
    xpos=4-4.0f/zoom;
  if(ypos-4.0f/zoom<-4)
    ypos=-4+4.0f/zoom;
  else if(ypos+4.0f/zoom>4)
    ypos=4-4.0f/zoom;
  Matrices.projection = glm::ortho((float)(-4.0f/zoom+xpos), (float)(4.0f/zoom+xpos), (float)(-4.0f/zoom+ypos), (float)(4.0f/zoom+ypos), 0.1f, 500.0f);

}

void createcannon()
  {
   int index;
   for (int i = 0; i < 100; ++i)
   {
    if(cannon[i].flag==0)
    {
      index=i;
      break;
    }
  }
  static GLfloat vertex_buffer_data[9];
  vertex_buffer_data[0]=0;
  vertex_buffer_data[1]=0;
  vertex_buffer_data[2]=0;
  vertex_buffer_data[3]=0.3;
  vertex_buffer_data[4]=0;
  vertex_buffer_data[5]=0;
  vertex_buffer_data[6]=0.3;
  vertex_buffer_data[7]=0;
  vertex_buffer_data[8]=0;

  static const GLfloat color_buffer_data [] = {
    1,0,0, // color 1
    1,0,0, // color 2
    1,0,0 // color 1
  };
  float a=0.3*cos(laser[0].angle*M_PI/180.0);
  float b=0.3*sin(laser[0].angle*M_PI/180.0); 
  cannon[index].rec=create3DObject(GL_TRIANGLES, 3, vertex_buffer_data, color_buffer_data, GL_LINE);
  cannon[index].x=-3.75+a;
  cannon[index].y=b+laser[1].y_speed;
  cannon[index].l=0.3;
  cannon[index].b=0;
  cannon[index].x_speed=0;
  cannon[index].y_speed=0;
  cannon[index].angle=laser[0].angle;
  cannon[index].flag=1;
  cannon[index].trans=laser[1].y_speed;
  cannon[index].flagc=0;
}
float triangle_rot_dir = 1;
float rectangle_rot_dir = 1;
bool triangle_rot_status = true;
bool rectangle_rot_status = true;
float  rect1x=-2;
float  rect2x=2;
int a[9]={0};
void func()
{
  if(a[0]&&rect1x<3.50)
  {
    rect1x+=0.1; 
  }
  if(a[1]&&rect1x>-4)
  {
    rect1x-=0.1;
  }
  if(a[2]&&rect2x>-4)
  {
   rect2x-=0.1;
 }
 if(a[3]&&rect2x<3.5)
 {
  rect2x+=0.1;
}
if(a[4]&&laser[0].angle<=90)
{
  laser[0].angle+=1;
  laser[2].angle+=1;
}   
if(a[5]&&laser[0].angle>=-90)
{
  laser[0].angle-=1;
  laser[2].angle-=1;
} 

if(a[6]&&laser[0].y_speed<=3.4)
{
  laser[0].y_speed+=0.1;
  laser[1].y_speed+=0.1;
  laser[2].y_speed+=0.1;
}    
if(a[7]&&laser[0].y_speed>=-3.4)
{
  laser[0].y_speed-=0.1;
  laser[1].y_speed-=0.1;
  laser[2].y_speed-=0.1;
}
}
int createRect (GLfloat *vertex_buffer_data,float a,float b,float l,float h,float tilt,int cnt)
{
    a=a/4.0;
    b=b/4.0;
    l=l/4.0;
    h=h/4;

    vertex_buffer_data[cnt]=a;
    cnt++;
    vertex_buffer_data[cnt]=b;
    cnt++;
    vertex_buffer_data[cnt]=0;
    cnt++;
    vertex_buffer_data[cnt]=a+l*cos(tilt*(M_PI/180));
    cnt++;
    vertex_buffer_data[cnt]=b+l*sin(tilt*(M_PI/180));
    cnt++;
    vertex_buffer_data[cnt]=0;
    cnt++;
    vertex_buffer_data[cnt]=a+h*sin(tilt*(M_PI/180));
    cnt++;
    vertex_buffer_data[cnt]=b+h*cos(tilt*(M_PI/180));
    cnt++;
    vertex_buffer_data[cnt]=0;
    cnt++;
    vertex_buffer_data[cnt]=a+h*sin(tilt*(M_PI/180))+l*cos(tilt*(M_PI/180));
    cnt++;
    vertex_buffer_data[cnt]=b+h*cos(tilt*(M_PI/180))+l*sin(tilt*(M_PI/180));
    cnt++;
    vertex_buffer_data[cnt]=0;
    cnt++;
    vertex_buffer_data[cnt]=a+l*cos(tilt*(M_PI/180));
    cnt++;
    vertex_buffer_data[cnt]=b+l*sin(tilt*(M_PI/180));
    cnt++;
    vertex_buffer_data[cnt]=0;
    cnt++;
    vertex_buffer_data[cnt]=a+h*sin(tilt*(M_PI/180));
    cnt++;
    vertex_buffer_data[cnt]=b+h*cos(tilt*(M_PI/180));
    cnt++;
    vertex_buffer_data[cnt]=0;
    cnt++;
    return cnt;
  }
/* Executed when a regular key is pressed/released/held-down */
/* Prefered for Keyboard events */
void Number()
  {
      GLfloat vertex_buffer_data0[500],color_buffer_data0[500];
      GLfloat vertex_buffer_data1[500],color_buffer_data1[500];
      GLfloat vertex_buffer_data2[500],color_buffer_data2[500];
      GLfloat vertex_buffer_data3[500],color_buffer_data3[500];
      GLfloat vertex_buffer_data4[500],color_buffer_data4[500];
      GLfloat vertex_buffer_data5[500],color_buffer_data5[500];
      GLfloat vertex_buffer_data6[500],color_buffer_data6[500];
      GLfloat vertex_buffer_data7[500],color_buffer_data7[500];
      GLfloat vertex_buffer_data8[500],color_buffer_data8[500];
      GLfloat vertex_buffer_data9[500],color_buffer_data9[500];
      int cnt=0,i,m=1,l;
      cnt=0;
      for(i=0;i<=9;i++)
      {
        cnt=0;
        if(i==0) 
        {  
          cnt=createRect(vertex_buffer_data0,2,2.95,0.5,0.05,0,cnt);
          cnt=createRect(vertex_buffer_data0,2,2.5,0.05,0.5,0,cnt);
          cnt=createRect(vertex_buffer_data0,2,2.05,0.05,0.5,0,cnt);
          cnt=createRect(vertex_buffer_data0,2.45,2.5,0.05,0.5,0,cnt);
          cnt=createRect(vertex_buffer_data0,2.45,2.05,0.05,0.5,0,cnt);
          cnt=createRect(vertex_buffer_data0,2,2.05,0.5,0.05,0,cnt);
          for(l=0;l<cnt;l++)
          {
            color_buffer_data0[l]=0;
          }    
          numbers[i].rec=create3DObject(GL_TRIANGLES, cnt/3, vertex_buffer_data0, color_buffer_data0, GL_FILL);
        }
        if(i==1)
        {
          cnt=createRect(vertex_buffer_data1,2.45,2.5,0.05,0.5,0,cnt);
          cnt=createRect(vertex_buffer_data1,2.45,2.05,0.05,0.5,0,cnt);
          for(l=0;l<cnt;l++)
          {
            color_buffer_data1[l]=0;
          }    
          numbers[i].rec=create3DObject(GL_TRIANGLES, cnt/3, vertex_buffer_data1, color_buffer_data1, GL_FILL);

        }
        if(i==2)
        {
          cnt=createRect(vertex_buffer_data2,2,2.95,0.5,0.05,0,cnt);
          cnt=createRect(vertex_buffer_data2,2,2.05,0.05,0.5,0,cnt);
          cnt=createRect(vertex_buffer_data2,2,2.5,0.5,0.05,0,cnt);
          cnt=createRect(vertex_buffer_data2,2.45,2.5,0.05,0.5,0,cnt);
          cnt=createRect(vertex_buffer_data2,2,2.05,0.5,0.05,0,cnt);
          for(l=0;l<cnt;l++)
          {
            color_buffer_data2[l]=0;
          }    
          numbers[i].rec=create3DObject(GL_TRIANGLES, cnt/3, vertex_buffer_data2, color_buffer_data2, GL_FILL);

        }
        if(i==3)
        {
          cnt=createRect(vertex_buffer_data3,2,2.95,0.5,0.05,0,cnt);
          cnt=createRect(vertex_buffer_data3,2,2.5,0.5,0.05,0,cnt);
          cnt=createRect(vertex_buffer_data3,2.45,2.5,0.05,0.5,0,cnt);
          cnt=createRect(vertex_buffer_data3,2.45,2.05,0.05,0.5,0,cnt);
          cnt=createRect(vertex_buffer_data3,2,2.05,0.5,0.05,0,cnt);
          for(l=0;l<cnt;l++)
          {
            color_buffer_data3[l]=0;
          }    
          numbers[i].rec=create3DObject(GL_TRIANGLES, cnt/3, vertex_buffer_data3, color_buffer_data3, GL_FILL);
        }
        if(i==4)
        {
          cnt=createRect(vertex_buffer_data4,2,2.5,0.05,0.5,0,cnt);
          cnt=createRect(vertex_buffer_data4,2,2.5,0.5,0.05,0,cnt);
          cnt=createRect(vertex_buffer_data4,2.45,2.5,0.05,0.5,0,cnt);
          cnt=createRect(vertex_buffer_data4,2.45,2.05,0.05,0.5,0,cnt);
          for(l=0;l<cnt;l++)
          {
            color_buffer_data4[l]=0;
          }    
          numbers[i].rec=create3DObject(GL_TRIANGLES, cnt/3, vertex_buffer_data4, color_buffer_data4, GL_FILL);

        }
        if(i==5)
        {
          cnt=createRect(vertex_buffer_data5,2,2.95,0.5,0.05,0,cnt);
          cnt=createRect(vertex_buffer_data5,2,2.5,0.05,0.5,0,cnt);
          cnt=createRect(vertex_buffer_data5,2,2.5,0.5,0.05,0,cnt);
          cnt=createRect(vertex_buffer_data5,2.45,2.05,0.05,0.5,0,cnt);
          cnt=createRect(vertex_buffer_data5,2,2.05,0.5,0.05,0,cnt);
          for(l=0;l<cnt;l++)
          {
            color_buffer_data5[l]=0;
          }    
          numbers[i].rec=create3DObject(GL_TRIANGLES, cnt/3, vertex_buffer_data5, color_buffer_data5, GL_FILL);

        } 
        if(i==6)
        {
          cnt=createRect(vertex_buffer_data6,2,2.95,0.5,0.05,0,cnt);
          cnt=createRect(vertex_buffer_data6,2,2.5,0.05,0.5,0,cnt);
          cnt=createRect(vertex_buffer_data6,2,2.05,0.05,0.5,0,cnt);
          cnt=createRect(vertex_buffer_data6,2,2.5,0.5,0.05,0,cnt);
          cnt=createRect(vertex_buffer_data6,2.45,2.05,0.05,0.5,0,cnt);
          cnt=createRect(vertex_buffer_data6,2,2.05,0.5,0.05,0,cnt);

          for(l=0;l<cnt;l++)
          {
            color_buffer_data6[l]=0;
          }    
          numbers[i].rec=create3DObject(GL_TRIANGLES, cnt/3, vertex_buffer_data6, color_buffer_data6, GL_FILL);

        } 
        if(i==7)
        {
          cnt=createRect(vertex_buffer_data7,2,2.95,0.5,0.05,0,cnt);
          cnt=createRect(vertex_buffer_data7,2.45,2.5,0.05,0.5,0,cnt);
          cnt=createRect(vertex_buffer_data7,2.45,2.05,0.05,0.5,0,cnt);
          for(l=0;l<cnt;l++)
          {
            color_buffer_data7[l]=0;
          }    
          numbers[i].rec=create3DObject(GL_TRIANGLES, cnt/3, vertex_buffer_data7, color_buffer_data7, GL_FILL);

        }
        if(i==8)
        {
         cnt=createRect(vertex_buffer_data8,2,2.95,0.5,0.05,0,cnt);
         cnt=createRect(vertex_buffer_data8,2,2.5,0.05,0.5,0,cnt);
         cnt=createRect(vertex_buffer_data8,2,2.05,0.05,0.5,0,cnt);
         cnt=createRect(vertex_buffer_data8,2,2.5,0.5,0.05,0,cnt);
         cnt=createRect(vertex_buffer_data8,2.45,2.5,0.05,0.5,0,cnt);
         cnt=createRect(vertex_buffer_data8,2.45,2.05,0.05,0.5,0,cnt);
         cnt=createRect(vertex_buffer_data8,2,2.05,0.5,0.05,0,cnt);

         for(l=0;l<cnt;l++)
         {
          color_buffer_data8[l]=0;
        }    
        numbers[i].rec=create3DObject(GL_TRIANGLES, cnt/3, vertex_buffer_data8, color_buffer_data8, GL_FILL);

      } 
      if(i==9)
      {
        cnt=createRect(vertex_buffer_data9,2,2.95,0.5,0.05,0,cnt);
        cnt=createRect(vertex_buffer_data9,2,2.5,0.05,0.5,0,cnt);
        cnt=createRect(vertex_buffer_data9,2,2.5,0.5,0.05,0,cnt);
        cnt=createRect(vertex_buffer_data9,2.45,2.5,0.05,0.5,0,cnt);
        cnt=createRect(vertex_buffer_data9,2.45,2.05,0.05,0.5,0,cnt);
        cnt=createRect(vertex_buffer_data9,2,2.05,0.5,0.05,0,cnt);
        for(l=0;l<cnt;l++)
        {
          color_buffer_data9[l]=0;
        }    
        numbers[i].rec=create3DObject(GL_TRIANGLES, cnt/3, vertex_buffer_data9, color_buffer_data9, GL_FILL);

      }   
    }
}

void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
     // Function is called first on GLFW_PRESS.

  if (action == GLFW_PRESS) {
   switch (key) {
    case GLFW_KEY_UP:
      mousezoom(window,0,+1);
      break;
    case GLFW_KEY_DOWN:
      mousezoom(window,0,-1);
      break;
    case GLFW_KEY_RIGHT:
        if(GLFW_MOD_CONTROL==mods)
        {
        a[0]=1;
        }
        else if(GLFW_MOD_ALT==mods)
        {
        a[3]=1;
        }
        else{  
        xpos+=0.2;
        pan();
       }
       break;
    case GLFW_KEY_LEFT:
       if(GLFW_MOD_CONTROL==mods)
        {
        a[1]=1;
        }
       else if(GLFW_MOD_ALT==mods)
        {
        a[2]=1;
        }
        else
        {
      xpos-=0.2;
      pan();
        }
       break;
    case GLFW_KEY_Z:
      ypos+=0.2;
      pan();
      break;
    case GLFW_KEY_X:
      ypos-=0.2;
      pan();
      break;
    case GLFW_KEY_C:
        rectangle_rot_status = !rectangle_rot_status;
        break;
    case GLFW_KEY_P:
        triangle_rot_status = !triangle_rot_status;
        break;
    /*case GLFW_KEY_RIGHT :
     // int  sta = glutGetModifiers();
        if(GLFW_MOD_CONTROL==mods)
        {
        a[0]=1;
        }
        if(GLFW_MOD_ALT==mods)
        {
        a[3]=1;
        }
        // do something ..
        break;
*/
  /*  case GLFW_KEY_LEFT:
       if(GLFW_MOD_CONTROL==mods)
        {
        a[1]=1;
        }
        if(GLFW_MOD_ALT==mods)
        {
        a[2]=1;
        }
    */    // 
       // func();
      //  break;
   /* case GLFW_KEY_J:
        a[2]=1;
               // do something ..
        break;
    case GLFW_KEY_L:
        a[3]=1;
                // do something ..
        break;*/
    case GLFW_KEY_A:
        a[4]=1;
        break;
    case GLFW_KEY_D:
        a[5]=1;
        break;
    case GLFW_KEY_S:
        a[6]=1;
        break;
    case GLFW_KEY_F:
        a[7]=1;
        break;
    case GLFW_KEY_SPACE:
        if(flagsp==1)
        {
        createcannon();
        last_update_time1=current_time1;
        } 
        break;
    case GLFW_KEY_N:
        if(brickspeed<=0.02)
        {
          brickspeed+=0.005;
        }
        break;
    case GLFW_KEY_M:
        if(brickspeed>=0.010)
        {
          brickspeed-=0.005;
        }
        break;                       
    default:
        break;
  }
}
  else if (action == GLFW_RELEASE) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
      quit(window);
      break;
       case GLFW_KEY_RIGHT:
              if(GLFW_MOD_CONTROL==mods)
              {
                    a[0]=0;
              }
              if(GLFW_MOD_ALT==mods)
              {
              a[3]=0;
              }
                 break;

        case GLFW_KEY_LEFT:
                    if(GLFW_MOD_CONTROL==mods)
                  {
                    a[1]=0;
                  }
                    if(GLFW_MOD_ALT==mods)
                  {
                    a[2]=0;
                  }
                  //  a[1]=0;
                    break;
            /*case GLFW_KEY_J:
                    a[2]=0;
                    break;
            case GLFW_KEY_L:
                    a[3]=0;
                    break;*/
            case GLFW_KEY_A:
                    a[4]=0;
                    break;
            case GLFW_KEY_D:
                    a[5]=0;
                    break;
            case GLFW_KEY_S:
                    a[6]=0;
                    break;
            case GLFW_KEY_F:
                    a[7]=0;
                    break;
            case GLFW_KEY_SPACE:
                    a[8]=0;
                    break;


      default:
      break;
    }
  }
}

/* Executed for character input (like in text boxes) */
void keyboardChar (GLFWwindow* window, unsigned int key)
{
	switch (key) {
		case 'Q':
		case 'q':
    quit(window);
    break;
    default:
    break;
  }
}

/* Executed when a mouse button is pressed/released */
void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
  switch (button) {
      case GLFW_MOUSE_BUTTON_LEFT:
      if (action == GLFW_RELEASE) {
      leftclick=0;
      flag12=0;
      flag112=0;
      flag111=0;
      } 
      if(action==GLFW_PRESS)
      {
        leftclick=1;
        glfwGetCursorPos(window,&intx1,&inty1);
         intx1-=400;
        intx1/=800/8.0;
        inty1=300-inty1;
        inty1/=(600/8.0);
        flag12=1;
        flag111=0;
        flag112=0;
      }
      break;
    case GLFW_MOUSE_BUTTON_RIGHT:
      if (action == GLFW_RELEASE) {
      rightclick=0;
        }
    if(action==GLFW_PRESS)
    {
      glfwGetCursorPos(window,&intx,&inty);
      //flagr
      intx-=400;
      intx/=800/8.0;
      inty=300-inty;
      inty/=(600/8.0);
      rightclick=1;
    }
    break;
    default:
    break;
  }
}


/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */
void reshapeWindow (GLFWwindow* window, int width, int height)
{
  int fbwidth=width, fbheight=height;
    /* With Retina display on Mac OS X, GLFW's FramebufferSize
     is different from WindowSize */
  glfwGetFramebufferSize(window, &fbwidth, &fbheight);

  GLfloat fov = 90.0f;

	// sets the viewport of openGL renderer
  glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);

	// set the projection matrix as perspective
	/* glMatrixMode (GL_PROJECTION);
	   glLoadIdentity ();
	   gluPerspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1, 500.0); */
	// Store the projection matrix in a variable for future use
    // Perspective projection for 3D views
    // Matrices.projection = glm::perspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 500.0f);

    // Ortho projection for 2D views
  Matrices.projection = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 500.0f);
}
long long int score=0;
VAO *triangle, *rectangle;
VAO *rect1, *rect2;
VAO *line;

// Creates the triangle object used in this sample code
/*void dr(int u)
{
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glUseProgram (programID);
glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
  glm::vec3 target (0, 0, 0);
 glm::vec3 up (0, 1, 0);
 Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0));
glm::mat4 VP = Matrices.projection * Matrices.view;
glm::mat4 MVP;
   Matrices.model = glm::mat4(1.0f);
  glm::mat4 translateRectangle = glm::translate (glm::vec3(0, 0, 0));
   glm::mat4 rectTransform = translateRectangle ;
   Matrices.model *= rectTransform; 
   MVP = VP * Matrices.model;
   glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
//  draw3DObject(cir);
   draw3DObject(recta[u].rec);

}*/
int checkcollision(int ind)
{
  int fla=0;

  for (int i = 0; i <100; ++i)
  {
    if(cannon[i].flag==1)
    {
      float x1,y1;
      float th;
      th=cannon[i].angle;
      float a=cos(th*M_PI/180.0);
      float b=sin(th*M_PI/180.0);   
      float r=cannon[i].x_speed;
      x1=cannon[i].x+r*a+0.3*a;
      y1=cannon[i].y+r*b+b*0.3;
      float x12,y12;
      x12=cannon[i].x+r*a;
      y12=cannon[i].y+r*b;
      float x2,y2;
      x2=(x1+x12)/2;
      y2=(y1+y12)/2;
      float q=recta[ind].x;
      float qr=recta[ind].y;
      float x3=q-0.05;
      float x4=q+0.05;
      float y3=qr-0.15+recta[ind].y_speed;
      float y4=qr+0.15+recta[ind].y_speed;
         // printf("x1%f y1%f x3%f x4%f y3%f y4%f\n",x1,y1,x3,x4,y3,y4);
      if(x1<=x4&&x1>=x3&&y1<=y4&&y1>=y3)
      {
        cannon[i].flag=0;
        recta[ind].flag=0;
        fla=1;
        if(recta[ind].color==0||recta[ind].color==1)
        {
          score-=3;
         // printf("%lld\n",score);
          cannon[i].flagc=1;
        }
        else if(recta[ind].color==2)
        {
          score+=10;
          //printf("%lld\n",score);
          cannon[i].flagc=1;
        }
        break;
      }
      else if(x12<=x4&&x12>=x3&&y12<=y4&&y12>=y3)
      {
        cannon[i].flag=0;
        recta[ind].flag=0;
        fla=1;
        if(recta[ind].color==0||recta[ind].color==1)
        {
          score-=3;
            //        printf("%lld\n",score);

          cannon[i].flagc=1;
        }
        else if(recta[ind].color==2)
        {
          score+=10;
              //     printf("%lld\n",score);

          cannon[i].flagc=1;
        }
        break;
      }
      else if(x2<=x4&&x2>=x3&&y2<=y4&&y2>=y3)
      {
        cannon[i].flag=0;
        recta[ind].flag=0;
        fla=1;
        if(recta[ind].color==0||recta[ind].color==1)
        {
          score-=3;
                //   printf("%lld\n",score);

          cannon[i].flagc=1;
        }
        else if(recta[ind].color==2)
        {
          score+=10;
                  // printf("%lld\n",score);

          cannon[i].flagc=1;
        }
        break;
      }
    }
  }
  if(fla==0)
  {
    //printf("hello\n");
    float x11=0.25+rect1x; 
    float x12=0.25+rect2x;
    float y11=-3.65;
    float x22=recta[ind].x;
    float y22=recta[ind].y+recta[ind].y_speed;
    if((abs(x22-x11)<=0.25 &&abs(y22-y11)<=0.50 && recta[ind].color==0)||(recta[ind].color==1&&abs(x12-x22)<=0.25 &&abs(y22-y11)<=0.50))
    {
      score+=10;
      //printf("%lld\n",score);
      recta[ind].flag=0;
      fla=1;
    /* code */
    }
     else if((abs(x22-x11)<=0.25 &&abs(y22-y11)<=0.50 && recta[ind].color==2)||(recta[ind].color==2&&abs(x12-x22)<=0.25 &&abs(y22-y11)<=0.50))
    {
      //score-=3;
      //printf("%lld\n",score);
      recta[ind].flag=0;
      printf("\n\n");
      printf("Gameover\n");
      flagq=1;
      // quit(window);
     //glfwDestroyWindow(window);
     //glfwTerminate();
      
      fla=1;
    /* code */
    }
    else if((abs(x22-x11)<=0.25 &&abs(y22-y11)<=0.50 && recta[ind].color==1)||(recta[ind].color==0&&abs(x12-x22)<=0.25 &&abs(y22-y11)<=0.50))
    {
      recta[ind].flag=0;
      score-=3;
      fla=1;
    }
  }
  return fla;
}

void createmir(int i,float x,float y,float angle)
{

/*static const GLfloat vertex_buffer_data [] = {
    -0.05,0,0, // vertex 1
    0.05,0,0, // vertex 2
    0.05,0.7,0, // vertex 3

     0.05,0.7,0, // vertex 3
    -0.05,0.7,0, // vertex 4
    -0.05,0,0  // vertex 1
  };*/
  static GLfloat vertex_buffer_data[18];
  vertex_buffer_data[0]=-0.025;
  vertex_buffer_data[1]=0;
  vertex_buffer_data[2]=0;
  vertex_buffer_data[3]=0.025;
  vertex_buffer_data[4]=0;
  vertex_buffer_data[5]=0;
  vertex_buffer_data[6]=0.025;
  vertex_buffer_data[7]=0.7;
  vertex_buffer_data[8]=0;
  vertex_buffer_data[9]=0.025;
  vertex_buffer_data[10]=0.7;
  vertex_buffer_data[11]=0;
  vertex_buffer_data[12]=-0.025;
  vertex_buffer_data[13]=0.7;
  vertex_buffer_data[14]=0;
  vertex_buffer_data[15]=-0.025;
  vertex_buffer_data[16]=0;
  vertex_buffer_data[17]=0;

  static const GLfloat color_buffer_data1 [] = {
    0,1,0, // color 1
    0,1,0, // color 2
    0,1,0, // color 3

    0,1,0, // color 3
    0,1,0, // color 4
    0,1,0  // color 1
  };
  mirrors[i].angle=angle;
  mirrors[i].x_speed=x;
  mirrors[i].y_speed=y;
  mirrors[i].x=0;
  mirrors[i].y=0;

  mirrors[i].rec= create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data1, GL_FILL);
//mirrors[1].rec= create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data1, GL_FILL);
//mirrors[2].rec= create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data1, GL_FILL);
//mirrors[3].rec= create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data1, GL_FILL);
}

int createrec (float x,float y,float l ,float b,ll c)
{
  /* ONLY vertices between the bounds specified in glm::ortho will be visible on screen */

  /* Define vertex array as used in glBegin (GL_TRIANGLES) */
  float e=b/2.0;
  float f=l/2.0;
  /*static const GLfloat vertex_buffer_data [] = {
    x-e, y-f,0, // vertex 0
    x+e,y-f,0, // vertex 1
    x+e,y+f,0, // vertex 2

    x+e,y+f,0,
    x-e,y+f,0,
    x-e,y-f,0
  };*/
  static GLfloat vertex_buffer_data[18];
  vertex_buffer_data[0]=x-e;
  vertex_buffer_data[1]=y-f;
  vertex_buffer_data[2]=0;
  vertex_buffer_data[3]=x+e;
  vertex_buffer_data[4]=y-f;
  vertex_buffer_data[5]=0;
  vertex_buffer_data[6]=x+e;
  vertex_buffer_data[7]=y+f;
  vertex_buffer_data[8]=0;
  vertex_buffer_data[9]=x+e;
  vertex_buffer_data[10]=y+f;
  vertex_buffer_data[11]=0;
  vertex_buffer_data[12]=x-e;
  vertex_buffer_data[13]=y+f;
  vertex_buffer_data[14]=0;
  vertex_buffer_data[15]=x-e;
  vertex_buffer_data[16]=y-f;
  vertex_buffer_data[17]=0;

  float q,w;
  if(c==0)
  {
    q=1;
    w=0;
    e=0;
  }
  else if(c==1)
  {
    q=0;
    w=1;
    e=0;
  }
  else
  {
    q=0;
    e=0;
    w=0;
  }
  /*static const GLfloat color_buffer_data [] = {
    q,w,e, // color 0
    q,w,e, // color 1
    q,w,e,// color 2
    
    q,w,e,
    q,w,e,
    q,w,e
  };*/
  static GLfloat color_buffer_data[18];
  color_buffer_data[0]=q;
  color_buffer_data[1]=w;
  color_buffer_data[2]=e;
  color_buffer_data[3]=q;
  color_buffer_data[4]=w;
  color_buffer_data[5]=e;
  color_buffer_data[6]=q;
  color_buffer_data[7]=w;
  color_buffer_data[8]=e;
  color_buffer_data[9]=q;
  color_buffer_data[10]=w;
  color_buffer_data[11]=e;
  color_buffer_data[12]=q;
  color_buffer_data[13]=w;
  color_buffer_data[14]=e;
  color_buffer_data[15]=q;
  color_buffer_data[16]=w;
  color_buffer_data[17]=e;
  ll fl,i;
  for (i = 0; i < 100; ++i)
  {
    //if(//recta[i].flag==0)
    if(recta[i].flag==0)
    {
      fl=i;
      recta[i].flag=1;
      //vis[i]=1;
      break;
    }
    /* code */
  }
  recta[fl].x=x;
  recta[fl].y=y;
  recta[fl].l=l;
  recta[fl].b=b;
  recta[fl].x_speed=0;
  recta[fl].y_speed=0;
  recta[fl].color=c;
  recta[fl].angle=0;
  recta[fl].flag=1;

  // create3DObject creates and returns a handle to a VAO that can be used later
  recta[fl].rec = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);

  return fl;
}
VAO *line1;
void createline()
{
  static const GLfloat vertex_buffer_data [] = {
    -4,-3.27,0, // vertex 1
    4,-3.27,0, // vertex 2
    4,-3.27,0
  };
  static const GLfloat color_buffer_data [] = {
    1,0,0, // color 1
    1,0,0, // color 2
    1,0,0, // color 3
  };
  static const GLfloat vertex_buffer_data1 [] = {
    0,0,0, // vertex 1
    0.15,0,0, // vertex 2
    0.15,0,0
  };
  static const GLfloat color_buffer_data1 [] = {
    0,0,0, // color 1
    0,0,0, // color 2
    0,0,0, // color 3
  };
  line = create3DObject(GL_TRIANGLES, 3, vertex_buffer_data, color_buffer_data, GL_LINE);
  line1 = create3DObject(GL_TRIANGLES, 3, vertex_buffer_data1, color_buffer_data1, GL_LINE);

}
VAO *rect3,*rect4;
// Creates the rectangle object used in this sample code
void createRectangle ()
{
  // GL3 accepts only Triangles. Quads are not supported
  static const GLfloat vertex_buffer_data [] = {
    0,0,0, // vertex 1
    0.5,0,0, // vertex 2
    0.5,0.7,0, // vertex 3

     0.5,0.7,0, // vertex 3
    0,0.7,0, // vertex 4
    0,0,0  // vertex 1
  };
  static const GLfloat vertex_buffer_data1 [] = {
    -0.15,-0.15,0, // vertex 1
    0.15,-0.15,0, // vertex 2
    0.15,0.15,0, // vertex 3

     0.15,0.15,0, // vertex 3
    -0.15,0.15,0, // vertex 4
    -0.15,-0.15,0  // vertex 1
  };
  static const GLfloat vertex_buffer_data2 [] = {
    0,-0.07,0, // vertex 1
    0.3,-0.07,0, // vertex 2
    0.3,0.07,0, // vertex 3

     0.3,0.07,0, // vertex 3
    0,0.07,0, // vertex 4
    0,-0.07,0  // vertex 1
  };
  static const GLfloat color_buffer_data [] = {
    1,0,0, // color 1
    1,0,0, // color 2
    1,0,0, // color 3

    1,0,0, // color 3
    1,0,0, // color 4
    1,0,0  // color 1
  };

  static const GLfloat color_buffer_data1 [] = {
    0,1,0, // color 1
    0,1,0, // color 2
    0,1,0, // color 3

    0,1,0, // color 3
    0,1,0, // color 4
    0,1,0  // color 1
  };
  static const GLfloat color_buffer_data3 [] = {
    1,0.4,0.4, // color 1
    1,0.4,0.4, // color 1
    1,0.4,0.4, // color 1
    1,0.4,0.4, // color 1
    1,0.4,0.4, // color 1
    1,0.4,0.4 // color 1
 
  };
  static const GLfloat color_buffer_data2 [] = {
    0,0,1, // color 1
    0,0,1, // color 2
    0,0,1, // color 3

    0,0,1, // color 3
    0,0,1, // color 4
    0,0,1  // color 1
  };
  static const GLfloat color_buffer_data4 [] = {
    0.678,1,0.18, // color 1
    0.678,1,0.18, // color 1
    0.678,1,0.18, // color 1
    0.678,1,0.18, // color 1
    0.678,1,0.18, // color 1
    0.678,1,0.18 // color 1
    
    };
  // create3DObject creates and returns a handle to a VAO that can be used later
  rect1 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
  rect2 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data1, GL_FILL);
  rect3 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data3, GL_FILL);
  rect4 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data4, GL_FILL);
  laser[1].rec = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data1, color_buffer_data2, GL_FILL);
  laser[2].rec = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data2, color_buffer_data2, GL_FILL);

}
VAO *cir,*cir1,*cir2;
void crecir()
{
  static GLfloat vertex_buffer_data[9*360];
  static GLfloat vertex_buffer_data1[9*360];

  float th=0;
  for(int i=0;i<360;i++)
  {
    float a=0.07*cos(th*M_PI/180.0);
    float b=0.07*sin(th*M_PI/180.0);
    float c=0.07*cos((th+1)*M_PI/180.0);
    float d=0.07 *sin((th+1)*M_PI/180.0);
    vertex_buffer_data1[9*i+0]=a;
    vertex_buffer_data1[9*i+1]=b;
    vertex_buffer_data1[9*i+2]=0;
    vertex_buffer_data1[9*i+3]=c;
    vertex_buffer_data1[9*i+4]=d;
    vertex_buffer_data1[9*i+5]=0;
    vertex_buffer_data1[9*i+6]=0;
    vertex_buffer_data1[9*i+7]=0;
    vertex_buffer_data1[9*i+8]=0;
    th++;
  }
  th=0;
  for(int i=0;i<360;i++)
  {
    float a=0.25*cos(th*M_PI/180.0);
    float b=0.25*sin(th*M_PI/180.0);
    float c=0.25*cos((th+1)*M_PI/180.0);
    float d=0.25*sin((th+1)*M_PI/180.0);
    vertex_buffer_data[9*i+0]=a;
    vertex_buffer_data[9*i+1]=b;
    vertex_buffer_data[9*i+2]=0;
    vertex_buffer_data[9*i+3]=c;
    vertex_buffer_data[9*i+4]=d;
    vertex_buffer_data[9*i+5]=0;
    vertex_buffer_data[9*i+6]=0;
    vertex_buffer_data[9*i+7]=0;
    vertex_buffer_data[9*i+8]=0;
    th++;
  }
  static GLfloat color_buffer_data[9*360];
  for(int i=0;i<360;i++)
  {
    color_buffer_data[9*i+0]=0.949;
    color_buffer_data[9*i+1]=0.470;
    color_buffer_data[9*i+2]=0.470;
    color_buffer_data[9*i+3]=0.949;
    color_buffer_data[9*i+4]=0.470;
    color_buffer_data[9*i+5]=0.470;
    color_buffer_data[9*i+6]=0.949;
    color_buffer_data[9*i+7]=0.470;
    color_buffer_data[9*i+8]=0.470;
  }
  static GLfloat color_buffer_data1[9*360];
  for(int i=0;i<360;i++)
  {
    color_buffer_data1[9*i+0]=0.32;
    color_buffer_data1[9*i+1]=0.91;
    color_buffer_data1[9*i+2]=0.28;
    color_buffer_data1[9*i+3]=0.32;
    color_buffer_data1[9*i+4]=0.91;
    color_buffer_data1[9*i+5]=0.28;
    color_buffer_data1[9*i+6]=0.32;
    color_buffer_data1[9*i+7]=0.91;
    color_buffer_data1[9*i+8]=0.28;
  }
  static GLfloat color_buffer_data2[9*360];
  for(int i=0;i<360;i++)
  {
    color_buffer_data2[9*i+0]=0;
    color_buffer_data2[9*i+1]=0;
    color_buffer_data2[9*i+2]=1;
    color_buffer_data2[9*i+3]=0;
    color_buffer_data2[9*i+4]=0;
    color_buffer_data2[9*i+5]=1;
    color_buffer_data2[9*i+6]=0;
    color_buffer_data2[9*i+7]=0;
    color_buffer_data2[9*i+8]=1;
  }

  cir= create3DObject(GL_TRIANGLES, 360*3, vertex_buffer_data, color_buffer_data, GL_FILL);
  cir1= create3DObject(GL_TRIANGLES, 360*3, vertex_buffer_data, color_buffer_data, GL_FILL);
  cir2= create3DObject(GL_TRIANGLES, 360*3, vertex_buffer_data, color_buffer_data1, GL_FILL);
  laser[0].rec= create3DObject(GL_TRIANGLES, 360*3, vertex_buffer_data1, color_buffer_data2, GL_FILL);
//laser[1].rec= create3DObject(GL_TRIANGLES, 360*3, vertex_buffer_data1, color_buffer_data, GL_FILL);


} 
/*void createRectangle ()
{
  // GL3 accepts only Triangles. Quads are not supported
  static const GLfloat vertex_buffer_data [] = {
    0,0,0, // vertex 1
    0.6,0,0, // vertex 2
    0.6,0.5,0, // vertex 3

     0.6,0.5,0, // vertex 3
    0,0.5,0, // vertex 4
    0,0,0  // vertex 1
  };

  static const GLfloat color_buffer_data [] = {
    1,0,0, // color 1
    1,0,0, // color 2
    1,0,0, // color 3

    1,0,0, // color 3
    1,0,0, // color 4
    1,0,0  // color 1
  };

   static const GLfloat color_buffer_data1 [] = {
    0,1,0, // color 1
    0,1,0, // color 2
    0,1,0, // color 3

    0,1,0, // color 3
    0,1,0, // color 4
    0,1,0  // color 1
  };

  // create3DObject creates and returns a handle to a VAO that can be used later
  rect1 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
  rect2 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data1, GL_FILL);
}*/


/*float camera_rotation_angle = 90;
float rectangle_rotation = 0;
float triangle_rotation = 0;

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void checkmirror(int i,int j)
{
 float th;
 th=mirrors[j].angle +90;
 double x1=mirrors[j].x_speed;
 double y1=mirrors[j].y_speed;
 double a1=0.7*cos(th*M_PI/180.0);
 double b1=0.7*sin(th*M_PI/180.0);
 double x2=x1+a1;
 double y2=y1+b1;
 th=cannon[i].angle;
 double a12=cannon[i].x_speed*cos(th*M_PI/180.0);
 double b12=cannon[i].y_speed*sin(th*M_PI/180.0);
 double x3=cannon[i].x +a12;
 double y3=cannon[i].y+ b12;
 double a13=0.3*cos(th*M_PI/180.0);
 double b13=0.3*sin(th*M_PI/180.0);
 double x4=x3+a13;
 double y4=y3+b13;
 double d=((x1-x2)*(y3-y4))-(y1-y2)*(x3-x4);
 if(d==0)
 {
  return;
}
else
{
  double xa=((x1*y2-y1*x2)*(x3-x4))-((x1-x2)*(x3*y4-y3*x4));
  double ya=((x1*y2-y1*x2)*(y3-y4))-((y1-y2)*(x3*y4-y3*x4));
  double xa1=xa/d;
  double ya1=ya/d;
   // if(x3<=xa1&&xa1<=x4&&y3<=ya1&&ya1<=y4&&x1<=xa1&&xa1<=x2&&y1<=ya1&&ya1<=y2)
  if(xa1<=max(x1,x2)&&xa1>=min(x1,x2)&&ya1<=max(y1,y2)&&ya1>=min(y1,y2)&&xa1<=max(x3,x4)&&xa1>=min(x3,x4)&&ya1<=max(y3,y4)&&ya1>=min(y3,y4))
  {

    cannon[i].angle=2*(mirrors[j].angle+90)-cannon[i].angle;
    cannon[i].x=xa1;
    cannon[i].y=ya1;
    cannon[i].x_speed=0;
    cannon[i].y_speed=0;
  }
}  
}


void draw (ll un)
{
  // clear the color and depth in the frame buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // use the loaded shader program
  // Don't change unless you know what you are doing
  glUseProgram (programID);

  // Eye - Location of camera. Don't change unless you are sure!!
  glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
  // Target - Where is the camera looking at.  Don't change unless you are sure!!
  glm::vec3 target (0, 0, 0);
  // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
  glm::vec3 up (0, 1, 0);

  // Compute Camera matrix (view)
  // Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
  //  Don't change unless you are sure!!
  Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane

  // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
  //  Don't change unless you are sure!!
  glm::mat4 VP = Matrices.projection * Matrices.view;

  // Send our transformation to the currently bound shader, in the "MVP" uniform
  // For each model you render, since the MVP will be different (at least the M part)
  //  Don't change unless you are sure!!
  glm::mat4 MVP;	// MVP = Projection * View * Model
  glfwGetCursorPos(window,&finx1,&finy1);
  finx1-=400;
  finx1/=800/8.0;
  finy1=300-finy1;
  finy1/=(600/8.0);
  glm::mat4 temp=glm::mat4(1.0f);
  glm::mat4 temp1=glm::mat4(1.0f);

if(leftclick==1)
{  
 if(finx1>=rect1x && finx1<=rect1x+0.5 && finy1<=-3.3 && finy1>=-4)
 {    
   
   g+=finx1-intx1;
   if(rect1x+finx1-intx1>=-4&&rect1x+finx1-intx1<=3.5)
   {
   rect1x+=finx1-intx1;
   //total1maxX+=x_pos-start_x_pos;
   flag111=1;
   intx1=finx1;
 }
   //temp=glm::translate(glm::vec3(g,0,0));
 
}
else if(finx1>=rect2x && finx1<=rect2x+0.5 &&finy1<=-3.3 && finy1>=-4)
 {    
   h+=finx1-intx1;
    if(rect2x+finx1-intx1>=-4&&rect2x+finx1-intx1<=3.5)
   {
   rect2x+=finx1-intx1;
   //total1maxX+=x_pos-start_x_pos;
   intx1=finx1;
   flag112=1;
   //temp=glm::translate(glm::vec3(g,0,0));
 }
}
 //else if((x_pos>=-4 && x_pos<=-4+0.6 && y_pos>=0+RifleY && y_pos<=0.2+RifleY) || (rifle_x_pos>=-4+0 && rifle_x_pos<=-4+0.2 && rifle_y_pos>=-0.5+RifleY && rifle_y_pos<=0.5+RifleY) || (rifle_x_pos>=-4+0 && rifle_x_pos<=-4+0.6 && rifle_y_pos>=0+RifleY && rifle_y_pos<=0.2+RifleY))
else if((finx1>=-4 && finx1<=-3.75 && finy1>=laser[1].y_speed-0.075 && finy1<=0.075+laser[1].y_speed))
 {
  //printf("%lf\n",finy1-inty1);
  if(laser[0].y_speed+finy1-inty1>=-3.7&&laser[0].y_speed+finy1-inty1<=3.7)
  {
  laser[0].y_speed+=finy1-inty1;

  laser[1].y_speed+=finy1-inty1;
  laser[2].y_speed+=finy1-inty1;
  inty1=finy1;
 // printf("HARI  %d\n",flag);
}
}
else 
{
  float c=-3.75;
  float d=laser[0].y_speed;
  float b=(finx1-c)/(finy1-d);
  laser[0].angle=atan(1/b)*(180/M_PI);
 // printf("SAI %d\n",flag);
  laser[2].angle=atan(1/b)*(180/M_PI);

 //lag++;
  if(flag12==1)
  {
   // bullet();
    if(flagsp==1)
    {
    createcannon();
    flag12=0;
    last_update_time1=current_time1;
  }
  }

}
}
  glfwGetCursorPos(window,&finx,&finy);
  finx-=400;
  finx/=800/8.0;
  finy=300-finy;
  finy/=(600/8.0);  
  if(rightclick==1)
  {
    xpos-=finx-intx;
    intx=finx;
    ypos-=finy-inty;
    inty=finy;
    flagp=1;
    pan();
  }
  int fg=score;
  float tr=0;
  int flags=0;
  if(score<0)
  {
    fg=-1*score;
    flags=1;
  }
  if(fg==0)
  {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translatecirc = glm::translate (glm::vec3(tr+2.5, 3.1, 0));
    Matrices.model *= translatecirc; 
    MVP = VP * Matrices.model; // MVP = p * V * M
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(numbers[0].rec);
  }
  while(fg>0)
  {
    int cn=fg%10;
    fg=fg/10;
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translatecirc = glm::translate (glm::vec3(tr+2.5, 3.1, 0));
    Matrices.model *= translatecirc; 
    MVP = VP * Matrices.model; // MVP = p * V * M
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(numbers[cn].rec);
    tr=tr-0.15;
  }
  if(flags)
  {
     Matrices.model = glm::mat4(1.0f);
    glm::mat4 translatecirc = glm::translate (glm::vec3(tr+3, 3.7, 0));
    Matrices.model *= translatecirc; 
    MVP = VP * Matrices.model; // MVP = p * V * M
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(line1);
  }

  // Load identity to model matrix
  Matrices.model = glm::mat4(1.0f);

  /* Render your scene */

 /* glm::mat4 translateTriangle = glm::translate (glm::vec3(-2.0f, 0.0f, 0.0f)); // glTranslatef
  glm::mat4 rotateTriangle = glm::rotate((float)(triangle_rotation*M_PI/180.0f), glm::vec3(0,0,1));  // rotate about vector (1,0,0)
  glm::mat4 triangleTransform = translateTriangle * rotateTriangle;
  Matrices.model *= triangleTransform; 
  MVP = VP * Matrices.model; // MVP = p * V * M

  //  Don't change unless you are sure!!
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(triangle);

  // Pop matrix to undo transformations till last push matrix instead of recomputing model matrix
  // glPopMatrix ();
  Matrices.model = glm::mat4(1.0f);

  glm::mat4 translateRectangle = glm::translate (glm::vec3(2, 0, 0));        // glTranslatef
  glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
  Matrices.model *= (translateRectangle * rotateRectangle);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(rectangle);*/

  
  glm::mat4 translatecirc = glm::translate (glm::vec3(rect1x+0.25, -3.3, 0));
  glm::mat4 rotatecirc= glm::rotate((float)(70*M_PI/180.0f), glm::vec3(0.5,0,0)); 
  glm::mat4 circTransform =translatecirc*rotatecirc;
  Matrices.model *= circTransform; 
  MVP = VP * Matrices.model; // MVP = p * V * M
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
  draw3DObject(cir);
  
  Matrices.model = glm::mat4(1.0f);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
 //draw3DObject(mirrors[0].rec);
  draw3DObject(line);

  Matrices.model = glm::mat4(1.0f);
  glm::mat4 translatecirc1 = glm::translate (glm::vec3(rect1x+0.25, -4, 0));
  glm::mat4 rotatecirc1= glm::rotate((float)(70*M_PI/180.0f), glm::vec3(0.5,0,0)); 
  glm::mat4 circ1Transform =translatecirc1*rotatecirc1;
  Matrices.model *= circ1Transform; 
  MVP = VP * Matrices.model; // MVP = p * V * M
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
  draw3DObject(cir1);
  for(int i=0;i<=3;i++)
  {
    Matrices.model = glm::mat4(1.0f);
    translatecirc1 = glm::translate (glm::vec3(mirrors[i].x_speed,mirrors[i].y_speed,0));
    rotatecirc1= glm::rotate((float)(mirrors[i].angle*M_PI/180.0f), glm::vec3(0,0,1)); 
    circ1Transform =translatecirc1*rotatecirc1;
    Matrices.model *= circ1Transform; 
  MVP = VP * Matrices.model; // MVP = p * V * M
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
  draw3DObject(mirrors[i].rec);
}
Matrices.model = glm::mat4(1.0f);
  // Matrices.model = glm::mat4(1.0f);
glm::mat4 translateRectangle1 = glm::translate (glm::vec3(rect1x, -4, 0));
glm::mat4 rect1Transform = translateRectangle1 ;
Matrices.model *= rect1Transform; 
MVP = VP * Matrices.model;
glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
  // Increment angles
draw3DObject(rect1);
if(flag111==1)
{
draw3DObject(rect3);

}
Matrices.model = glm::mat4(1.0f);

glm::mat4 translateRectangle = glm::translate (glm::vec3(rect2x, -4, 0));
glm::mat4 rectTransform = translateRectangle ;
Matrices.model *= rectTransform; 
MVP = VP * Matrices.model;
glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
//  draw3DObject(cir);
draw3DObject(rect2);
if(flag112==1)
{
draw3DObject(rect4);
}
Matrices.model = glm::mat4(1.0f);

glm::mat4 translatecirc2= glm::translate (glm::vec3(rect2x+0.25, -3.3, 0));
glm::mat4 rotatecirc2= glm::rotate((float)(70*M_PI/180.0f), glm::vec3(0.5,0,0)); 
glm::mat4 circ2Transform =translatecirc2*rotatecirc2;
Matrices.model *= circ2Transform; 
  MVP = VP * Matrices.model; // MVP = p * V * M
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
  draw3DObject(cir2);
// Matrices.model = glm::mat4(1.0f);
 //current_time = glfwGetTime();
/*if(recta[0].flag==1)*/
/*for (int i = 0; i < 100; ++i)
{
if(recta[i].flag==1)
{
glm::mat4 translateRectangle3 = glm::translate (glm::vec3(0, 0, 0));
   glm::mat4 rect3Transform = translateRectangle3 ;
   Matrices.model *= rect3Transform; 
   MVP = VP * Matrices.model;
   glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
//  draw3DObject(cir);
   draw3DObject(recta[un].rec);
}
}*/
  Matrices.model = glm::mat4(1.0f);
  float a=0.3*cos(laser[0].angle*M_PI/180.0);
  float b=0.3*sin(laser[0].angle*M_PI/180.0);
  translatecirc2= glm::translate (glm::vec3(a-3.75, b+laser[0].y_speed, 0));
  rotatecirc2= glm::rotate((float)(70*M_PI/180.0f), glm::vec3(0,1,0));
  glm::mat4 rotatecirc3= glm::rotate((float)(laser[0].angle*M_PI/180.0f), glm::vec3(0,0,1)); 
  Matrices.model *= translatecirc2*rotatecirc3*rotatecirc2; 
  MVP= VP * Matrices.model;
 //float increments = 1;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
  draw3DObject(laser[0].rec);
  Matrices.model = glm::mat4(1.0f);
  translatecirc2= glm::translate (glm::vec3(-3.85, laser[1].y_speed, 0));
//rotatecirc2= glm::rotate((float)(70*M_PI/180.0f), glm::vec3(0,1,0));
  Matrices.model *= translatecirc2; 
  MVP= VP * Matrices.model;
 //float increments = 1;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
  draw3DObject(laser[1].rec);
  Matrices.model = glm::mat4(1.0f);
  translatecirc2= glm::translate (glm::vec3(-3.75, 0, 0));
  rotatecirc2= glm::rotate((float)(laser[2].angle*M_PI/180.0f), glm::vec3(0,0,1));
  glm::mat4 translatecirc3= glm::translate (glm::vec3(0, laser[2].y_speed, 0));

  Matrices.model *= translatecirc3*translatecirc2*rotatecirc2; 
  MVP= VP * Matrices.model;
 //float increments = 1;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
  draw3DObject(laser[2].rec);

//draw3DObject(laser[1].rec);
//draw3DObject(laser[2].rec);
  ll i=0;

//double last_update_time = glfwGetTime(), current_time;
//if ((current_time - last_update_time) >= 0.5)
//{ 
  for(i=0;i<100;i++)
  { 
    int flag1=0;
  //if(recta[i].flag==1)
    if(recta[i].flag==1)
    {
     Matrices.model = glm::mat4(1.0f);
     flag1=checkcollision(i);
     if(flag1==0)
     {
      if(recta[i].y+recta[i].y_speed>=-3.5)
      {
        glm::mat4 translateRectangle3=glm::translate(glm::vec3(0,recta[i].y_speed, 0));
        recta[i].y_speed=recta[i].y_speed-brickspeed;
        glm::mat4 rect3Transform = translateRectangle3 ;
        Matrices.model *= rect3Transform; 
        MVP = VP * Matrices.model;
        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
        draw3DObject(recta[i].rec);
      }
      else
      {

        recta[i].flag=0;
      }
    }
  }
//}
}
for(i=0;i<100;i++)
{ 
  //if(recta[i].flag==1)
  if(cannon[i].flag==1)
  {
    checkmirror(i,0);
    checkmirror(i,1);
    checkmirror(i,2);
    checkmirror(i,3);
    Matrices.model = glm::mat4(1.0f);
    float a1=0.3*cos(cannon[i].angle*M_PI/180.0);
    float b1=0.3*sin(cannon[i].angle*M_PI/180.0);
    float a=cannon[i].x_speed*cos(cannon[i].angle*M_PI/180.0);
    float b=cannon[i].y_speed*sin(cannon[i].angle*M_PI/180.0);  
    rotatecirc2= glm::rotate((float)(cannon[i].angle*M_PI/180.0f), glm::vec3(0,0,1)); 
    glm::mat4 translateRectangle3=glm::translate(glm::vec3(cannon[i].x+a,b+cannon[i].y, 0));
    if(cannon[i].x+a>4||cannon[i].x+a<-4||cannon[i].y+b>4||cannon[i].y+b<-4)
    {
      cannon[i].flag=0;
      if(cannon[i].flagc==0)
      {
                score-=3;  
                  //("%lld\n",score);

      }
    }
    cannon[i].y_speed=cannon[i].y_speed+0.100;
    cannon[i].x_speed=cannon[i].x_speed+0.100;
    glm::mat4 rect3Transform = translateRectangle3*rotatecirc2 ;
    Matrices.model *= rect3Transform; 
    MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(cannon[i].rec);
  }
//}
}
float increments = 1;
  //camera_rotation_angle++; // Simulating camera rotation
triangle_rotation = triangle_rotation + increments*triangle_rot_dir*triangle_rot_status;
rectangle_rotation = rectangle_rotation + increments*rectangle_rot_dir*rectangle_rot_status;
}
GLFWwindow* initGLFW (int width, int height)
{
    GLFWwindow* window; // window desciptor/handle

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
//        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);

    if (!window) {
      glfwTerminate();
//        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval( 1 );

    /* --- register callbacks with GLFW --- */

    /* Register function to handle window resizes */
    /* With Retina display on Mac OS X GLFW's FramebufferSize
     is different from WindowSize */
    glfwSetFramebufferSizeCallback(window, reshapeWindow);
    glfwSetWindowSizeCallback(window, reshapeWindow);

    /* Register function to handle window close */
    glfwSetWindowCloseCallback(window, quit);

    /* Register function to handle keyboard input */
    glfwSetKeyCallback(window, keyboard);      // general keyboard input
    glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling

    /* Register function to handle mouse click */
    glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks
   glfwSetScrollCallback(window, mousezoom);

    return window;
 }  
/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
  void initGL (GLFWwindow* window, int width, int height)
  {
    /* Objects should be created before any other gl function and shaders */
	// Create the models
	//createTriangle (); // Generate the VAO, VBOs, vertices data & copy into the array buffer
   createRectangle ();
   crecir();
   createmir(0,0,3.3,60);
   createmir(1,-0.2,-0.2,-20);
   createmir(2,3.5,2.9,30);
   createmir(3,3.3,-2.3,-30);
  //crete
   createline();
   Number(); 
	// Create and compile our GLSL program from the shaders
   programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
	// Get a handle for our "MVP" uniform
   Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


   reshapeWindow (window, width, height);

    // Background color of the scene
	glClearColor (1.0f, 1.0f, 1.0f, 0.0f); // R, G, B, A
	glClearDepth (1.0f);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

  cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
  cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
  cout << "VERSION: " << glGetString(GL_VERSION) << endl;
  cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

int main (int argc, char** argv)
{
	int width = 800;
	int height = 600;
  
//   thread(play_audio,"1.mp3").detach();
  window = initGLFW(width, height);

  initGL (window, width, height);

  double last_update_time = glfwGetTime(), current_time;
  last_update_time1=glfwGetTime();
  for (int i = 0; i < 100; ++i)
  {
    recta[i].flag=0;
      //vis[i]=0;
      /* code */
  }
  //  ll r=recta[0].flag;
    //cout<<r;
  laser[0].angle=0;
  laser[0].y_speed=0;
  laser[1].y_speed=0;
  laser[2].y_speed=0;
  laser[2].angle=0;
  int ind=-1;
//ind=createrec(3.3,3.9,0.1,0.05,1);
    /* Draw in loop */
  while (!glfwWindowShouldClose(window)) {

          //int ind=-1;
         //ind=createrec(3.3,3.9,0.1,0.05,1);
          // OpenGL Draw commands

    func();
    draw(ind);

        // Swap Frame Buffer in double buffering
    glfwSwapBuffers(window);

        // Poll for Keyboard and mouse events
    glfwPollEvents();

        // Control based on time (Time based transformation like 5 degrees rotation every 0.5s)
        current_time = glfwGetTime(); // Time in seconds
        current_time1=glfwGetTime();
      //  int ind;
        //ind=createrec(3.3,3.9,0.1,0.05,1);
        srand(time(NULL)); 
        if ((current_time - last_update_time) >= 2.5) 
        { // atleast 0.5s elapsed since last frame
            // do something every 0.5 seconds ..
          float LO=-3.5;
          float HI=3.5;
          float r3 = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
            //ll cl= min1 + static_cast <ll> (rand()) /( static_cast <ll> (RAND_MAX/(max-min1)));
          ll cl=rand()%3;
           // cout<<cl<<r3;
          ind=createrec(r3,3.9,0.3,0.1,cl);

 //           dr(ind);
 //           upd(ind);
          last_update_time = current_time;
        }
        if((current_time1-last_update_time1)>=1)
        {
          flagsp=1;
          //last_update_time1=current_time1;
        }
        else if ((current_time1-last_update_time1)<1)
        {
          flagsp=0;
        }
        if(flagq==1)
      {
        exit(0);
        quit(window);
      }
      }


      glfwTerminate();
//    exit(EXIT_SUCCESS);
    }
