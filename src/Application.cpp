#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct ShaderProgramSource
{
    std::string VertexSource; 
    std::string FragmentSource;
};
// Using the struct shaderprogramsource to return vertex and fragment shaders
// Defining a function parseshader to get filepaths in string format
static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType //enumeration to define shader types
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];// 2 stringstreams for Vertex Shader and Fragment shader
    ShaderType type = ShaderType::NONE;//default shader type

    while (getline(stream, line))//gets filepath from ifstream and stores it in line string variable
    {
    	//this logic checks for the shader and defines its type
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n'; //adds the vertex/fragment shader type code to the relative stringstream
        }
    }
    // returns the two shaders in the stringstream as strings
    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type); //creates an empty shader object and assigns a reference value(here referenced as id)
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);//sets the source code in id to source code in src/source
    glCompileShader(id);//compiles the shader referenced to by id

	// this logic checks if the shader compiled successfully
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to Compile"<< (type == GL_VERTEX_SHADER ? "vertex" : "fragment") <<" shader" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id; //returns the shader object reference
}


static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return(program);
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Physics Engine", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[] = {
        0.0f,  0.0f,//0
        1.0f,  0.0f,//1
        0.8f,  0.8f,//2
        0.0f,  1.0f,//3
       -0.8f,  0.8f,//4
       -1.0f,  0.0f,//5
       -0.8f, -0.8f,//6
    	0.0f, -1.0f,//7
    	0.8f, -0.8f //8
    };

    unsigned int indices[]
    {
        0, 1, 2,
		2, 3, 0,
    	3, 4, 0,
    	4, 5, 0,
    	5, 6, 0,
        6, 7, 0,
    	7, 8, 0,
    	8, 1, 0
    };
	
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    unsigned int ibo;
    glGenBuffers(1, &ibo); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
  
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
      
        glDrawElements(GL_TRIANGLE_FAN, 24, GL_UNSIGNED_INT, nullptr);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}

