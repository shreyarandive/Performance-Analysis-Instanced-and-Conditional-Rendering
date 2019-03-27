#include <windows.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <random>
#include <iostream>
#include "InitShader.h"
#include "LoadMesh.h"
#include "LoadTexture.h"
#include "Cube.h"
#include "imgui_impl_glut.h"
#include "VideoMux.h"


//imgui
float angle = 0.0f;
float up_angle = 0.0f;
float posx = 2.5f;
float posy = 1.0f;
float posz = 10.0f;

//SHADER 1
static const std::string vertex_shader("Shaders/occlusion_vs.glsl");
static const std::string fragment_shader("Shaders/occlusion_fs.glsl");

GLuint shader_program = -1;
GLuint texture_id = -1;
GLuint texture_id_2 = -1;
GLuint uniform_buffer = -1;
int occlusion_ = 1;
bool occlusion_flag = true;

struct PVM_light_uniforms
{
	glm::mat4 M;	
	glm::mat4 PV;	

	glm::vec4 La;	
	glm::vec4 Ld;	
	glm::vec4 Ls;	
	glm::vec4 ka;	
	glm::vec4 kd;	
	glm::vec4 ks;	
	glm::vec4 pos;
	glm::vec4 light_pos; 

	glm::vec4 color_occ;
	glm::vec4 color_mesh;

	float shininess;
	int pass;

} PVM_light_data;

static const std::string mesh_name = "3Dmodels/deer.obj";
static const std::string texture_name = "3Dmodels/deer.jpg";

static const std::string occluder_name = "3Dmodels/Tree_OBJ_1.obj";
static const std::string o_tex_name = "3Dmodels/leaf_Dif.jpg";

MeshData mesh_data;
MeshData occluder_data;
MeshData cube_data;

float time_sec;
const int number_of_meshes = 100;
float meshes_number = 100.0f;
bool recording = false;

GLuint occlusion_query[number_of_meshes];
GLuint occlusion_timer;

glm::mat4 bounding_box;
glm::mat4 mesh_scale;

std::vector<glm::vec3> occluders_pos(meshes_number);
std::vector<glm::vec4> occluder_color(meshes_number);

std::vector<glm::vec3> meshes_pos(meshes_number);
std::vector<glm::vec4> meshes_color(meshes_number);

std::vector<glm::mat4> mesh_rotation(meshes_number);

std::default_random_engine generator;
std::uniform_real_distribution<float> occluders(-15, 15);
std::uniform_real_distribution<float> meshes(-15, 15);
std::uniform_real_distribution<float> mesh_rot(-10, 10);

void draw_meshes();
void draw_occluders();
void draw_bounding_box();
void draw_conditional_mesh();

//SHADER 2
static const std::string vertex_shader_2("Shaders/instanced_vs.glsl");
static const std::string fragment_shader_2("Shaders/instanced_fs.glsl");

const int number_of_instances = 100000;

int instanced_bool_loc = 3;
int without_instanced_loc = 4;
int instanced_loc = 5;
float range_of_instances = 700.0f;
bool instanced_rendering;

GLuint shader_program_2 = -1;
GLuint position_buffer_2 = -1;
GLuint normal_buffer_2 = -1;
GLuint matrixBuffer = -1;
GLuint grass_texture_id = -1;

GLuint timer_query;
GLuint nanoseconds;

MeshData grass_data;

static const std::string grass_mesh = "3Dmodels/grass.obj";
static const std::string grass_texture = "3Dmodels/petal.jpg";

std::uniform_real_distribution<float> range_for_with_instance(-range_of_instances, range_of_instances);
std::uniform_real_distribution<float> range_for_without_instance(-range_of_instances, range_of_instances);
std::uniform_real_distribution<float> height(-5.0f, 5.0f);

std::vector<glm::mat4> grass_pos_i(number_of_instances);
std::vector<glm::mat4> grass_pos(number_of_instances);

//SHADER 3
static const std::string vertex_shader_3("Shaders/cube_map_vs.glsl");
static const std::string fragment_shader_3("Shaders/cube_map_fs.glsl");

static const std::string cubemap_name = "cubemap";
GLuint cubemap_texure_id = -1;
GLuint shader_program_3 = -1;
GLuint cube_vao = -1;

//SHADER 4
static const std::string vertex_shader_4("Shaders/particle_vs.glsl");
static const std::string fragment_shader_4("Shaders/particle_fs.glsl");

GLuint shader_program_4 = -1;

GLuint vao[2] = { -1, -1 };
GLuint vbo[2] = { -1, -1 };
const int num_particles = 1000;
GLuint tfo[2] = { -1, -1 };

int Read_Index = 0;
int Write_Index = 1;


void reload_shader()
{
	GLuint new_shader_1 = InitShader(vertex_shader.c_str(), fragment_shader.c_str());
	GLuint new_shader_2 = InitShader(vertex_shader_2.c_str(), fragment_shader_2.c_str());
	GLuint new_shader_3 = InitShader(vertex_shader_3.c_str(), fragment_shader_3.c_str());
	GLuint new_shader_4 = InitShader(vertex_shader_4.c_str(), fragment_shader_4.c_str());

	if (new_shader_1 == -1) // loading failed
	{
		glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
	}
	else
	{
		glClearColor(0.35f, 0.35f, 0.35f, 0.0f);

		if (shader_program != -1)
		{
			glDeleteProgram(shader_program);
		}
		shader_program = new_shader_1;
	}

	if (new_shader_2 == -1) // loading failed
	{
		glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
	}
	else
	{
		glClearColor(0.35f, 0.35f, 0.35f, 0.0f);

		if (shader_program_2 != -1)
		{
			glDeleteProgram(shader_program_2);
		}
		shader_program_2 = new_shader_2;
	}

	if (new_shader_3 == -1) // loading failed
	{
		glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
	}
	else
	{
		glClearColor(0.35f, 0.35f, 0.35f, 0.0f);

		if (shader_program_3 != -1)
		{
			glDeleteProgram(shader_program_3);
		}
		shader_program_3 = new_shader_3;
	}

	if (new_shader_4 == -1) // loading failed
	{
		glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
	}
	else
	{
		glClearColor(0.35f, 0.35f, 0.35f, 0.0f);

		if (shader_program_4 != -1)
		{
			glDeleteProgram(shader_program_4);
		}
		shader_program_4 = new_shader_4;

		const char *vars[] = { "pos_out", "vel_out", "age_out" };
		glTransformFeedbackVaryings(shader_program_4, 3, vars, GL_SEPARATE_ATTRIBS);

		glLinkProgram(shader_program_4);
		int status;
		glGetProgramiv(shader_program_4, GL_LINK_STATUS, &status);
		if (status == GL_FALSE)
		{
			printProgramLinkError(shader_program_4);
		}
	}
}

void draw_gui()
{
	ImGui_ImplGlut_NewFrame();

	const int filename_len = 256;
	static char video_filename[filename_len] = "capture.mp4";

	ImGui::InputText("Video filename", video_filename, filename_len);
	ImGui::SameLine();
	if (recording == false)
	{
		if (ImGui::Button("Start Recording"))
		{
			const int w = glutGet(GLUT_WINDOW_WIDTH);
			const int h = glutGet(GLUT_WINDOW_HEIGHT);
			recording = true;
			start_encoding(video_filename, w, h); //Uses ffmpeg
		}
	}
	else
	{
		if (ImGui::Button("Stop Recording"))
		{
			recording = false;
			finish_encoding(); //Uses ffmpeg
		}
	}

	if (ImGui::Button("Reload"))
	{
		reload_shader();
	}
	if (ImGui::Checkbox("Instanced Rendering", &instanced_rendering))
	{

	}

	if (ImGui::Checkbox("Occlusion", &occlusion_flag))
	{
		occlusion_ = 2;
	}
	else
		occlusion_ = 1;

	

	//create a slider to change the angle variables
	ImGui::SliderFloat("Left and Right", &posx, -10.0f, +10.0f);
	ImGui::SliderFloat("Up and Down", &posy, 0.0f, +10.0f);
	ImGui::SliderFloat("Z axis", &posz, -20.0f, +20.0f);

	static float slider = 0.5f;
	//if (ImGui::SliderFloat("Slider", &slider, 0.0f, 1.0f))
	{
		glUniform1f(2, slider);
	}

	ImGui::Image((void*)grass_texture_id, ImVec2(128, 128));
	ImGui::Image((void*)texture_id_2, ImVec2(128, 128));
	ImGui::Image((void*)texture_id, ImVec2(128, 128));

	static bool show = false;
	ImGui::ShowTestWindow();
	ImGui::Render();
}

// glut display callback function.
// This function gets called every time the scene gets redisplayed 
void display()
{
	glBeginQuery(GL_TIME_ELAPSED, timer_query);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//shader 1
	glUseProgram(shader_program);

	const int w = glutGet(GLUT_WINDOW_WIDTH);
	const int h = glutGet(GLUT_WINDOW_HEIGHT);
	const float aspect_ratio = float(w) / float(h);

	PVM_light_data.pos = glm::vec4(-posx, posy, posz, 1.0f);
	glm::mat4 M = glm::scale(glm::vec3(grass_data.mScaleFactor))*glm::scale(glm::vec3(0.4f));
	glm::mat4 R = glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 R1 = glm::rotate(up_angle, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 V = glm::lookAt(glm::vec3(PVM_light_data.pos * R * R1), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 P = glm::perspective(3.141592f / 4.0f, aspect_ratio, 0.1f, 100.0f);
	PVM_light_data.PV = P*V;

	glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id_2);
	draw_occluders();

	if (occlusion_ == 2) //conditional rendering
	{
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);

		draw_bounding_box();
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
		draw_conditional_mesh();
	}
	else if (occlusion_ == 3) //debug mode
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		draw_bounding_box();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		draw_meshes();
	}
	else //without conditional rendering
	{
		draw_meshes();
	}

	//SHADER 2
	glUseProgram(shader_program_2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grass_texture_id);
	int tex_loc_2 = glGetUniformLocation(shader_program_2, "diffuse_color");
	if (tex_loc_2 != -1)
	{
		glUniform1i(tex_loc_2, 0);
	}

	int PVM_loc_2 = glGetUniformLocation(shader_program_2, "PVM");
	if (PVM_loc_2 != -1)
	{
		glm::mat4 PVM_2 = P*V*M;
		glUniformMatrix4fv(PVM_loc_2, 1, false, glm::value_ptr(PVM_2));
	}

	glBindVertexArray(grass_data.mVao);

	glUniform1i(instanced_bool_loc, instanced_rendering);

	if (instanced_rendering)
	{
		glDrawElementsInstanced(GL_TRIANGLES, grass_data.mNumIndices, GL_UNSIGNED_INT, 0, number_of_instances);
	}

	else
	{
		for (int i = 0; i < number_of_instances; i++)
		{
			glUniformMatrix4fv(without_instanced_loc, 1, false, glm::value_ptr(grass_pos[i]));

			glDrawElements(GL_TRIANGLES, grass_data.mNumIndices, GL_UNSIGNED_INT, 0);
		}
	}

	//SHADER 3
	glUseProgram(shader_program_3);

	int PVM_loc_3 = glGetUniformLocation(shader_program_3, "PVM_cube");
	if (PVM_loc_3 != -1)
	{
		glm::mat4 M_3 = glm::scale(glm::vec3(50.0f));
		glm::mat4 PVM_3 = P*V*M_3;
		glUniformMatrix4fv(PVM_loc_3, 1, false, glm::value_ptr(PVM_3));
	}

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texure_id);
	int cube_loc = glGetUniformLocation(shader_program_3, "cubemap");
	if (cube_loc != -1)
	{
		glUniform1i(cube_loc, 1);
	}
	draw_cube_vao(cube_vao);

	//SHADER 4
	glUseProgram(shader_program_4);

	const int w4 = glutGet(GLUT_WINDOW_WIDTH);
	const int h4 = glutGet(GLUT_WINDOW_HEIGHT);
	const float aspect_ratio4 = float(w4) / float(h4);

	glm::mat4 M4 = glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 V4 = glm::lookAt(glm::vec3(0.0f, 1.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 P4 = glm::perspective(3.141592f / 4.0f, aspect_ratio4, 0.1f, 100.0f);

	int PVM_loc_4 = glGetUniformLocation(shader_program_4, "PVM");
	if (PVM_loc_4 != -1)
	{
		glm::mat4 PVM_4 = P4*V4*M4;
		glUniformMatrix4fv(PVM_loc_4, 1, false, glm::value_ptr(PVM_4));
	}

	const GLint position_location = 0;
	const GLint velocity_location = 1;
	const GLint age_location = 2;

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfo[Write_Index]);

	glDepthMask(GL_FALSE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE); //additive alpha blending

	glBindVertexArray(vao[Read_Index]);
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, num_particles);
	glEndTransformFeedback();

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);

	if (recording == true)
	{
		glFinish();

		glReadBuffer(GL_BACK);
		read_frame_to_encode(&rgb, &pixels, w, h);
		encode_frame(rgb);
	}

	draw_gui();
	glutSwapBuffers();

	//Ping-pong the buffers.
	Read_Index = 1 - Read_Index;
	Write_Index = 1 - Write_Index;
	
	glEndQuery(GL_TIME_ELAPSED);
	glGetQueryObjectuiv(timer_query, GL_QUERY_RESULT, &nanoseconds);
	
	if(instanced_rendering)
		if(occlusion_flag)
			std::cout << "INSTANCED and OCCLUSION Enabled-->> " << 1e-9*nanoseconds << std::endl;
		else
			std::cout << "Only INSTANCED Rendering Enabled->> " << 1e-9*nanoseconds << std::endl;
	else
		if(occlusion_flag)
			std::cout << "Only OCCLUSION rendering Enabled->> " << 1e-9*nanoseconds << std::endl;
		else
			std::cout << "Instanced & Occlusion DISABLED--->> " << 1e-9*nanoseconds << std::endl;
}


void draw_occluders()
{
	PVM_light_data.pass = 0;
	glBindVertexArray(occluder_data.mVao);
	glm::mat4 scale_occluder = glm::scale(glm::vec3(0.03f))*glm::scale(glm::vec3(3.0f));

	for (int i = 0; i < number_of_meshes; i++)
	{
		PVM_light_data.color_occ = occluder_color[i];
		PVM_light_data.M = glm::translate(occluders_pos[i]) * scale_occluder;
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PVM_light_data), &PVM_light_data); 
		glDrawElements(GL_TRIANGLES, occluder_data.mNumIndices, GL_UNSIGNED_INT, 0);
	}
}

void draw_bounding_box()
{
	PVM_light_data.pass = 1;
	glBindVertexArray(cube_data.mVao);
	glBindTexture(GL_TEXTURE_2D, 0);

	for (int i = 0; i < number_of_meshes; i++)
	{
		PVM_light_data.color_mesh = meshes_color[i];
		PVM_light_data.M =	glm::translate(meshes_pos[i]) * 
							mesh_rotation[i] *
							glm::scale(glm::vec3(cube_data.mScaleFactor)) * 
							bounding_box * 
							mesh_scale;

		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PVM_light_data), &PVM_light_data); 
		glBeginQuery(GL_ANY_SAMPLES_PASSED, occlusion_query[i]);
		glDrawElements(GL_TRIANGLES, cube_data.mNumIndices, GL_UNSIGNED_SHORT, 0);
		glEndQuery(GL_ANY_SAMPLES_PASSED);
	}
}

void draw_conditional_mesh()
{
	PVM_light_data.pass = 2;
	glBindVertexArray(mesh_data.mVao);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	for (int i = 0; i < number_of_meshes; i++)
	{
		PVM_light_data.color_mesh = meshes_color[i];
		PVM_light_data.M =	glm::translate(meshes_pos[i]) * 
							mesh_rotation[i] *
							mesh_scale;

		glBeginConditionalRender(occlusion_query[i], GL_QUERY_WAIT);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PVM_light_data), &PVM_light_data);
		glDrawElements(GL_TRIANGLES, mesh_data.mNumIndices, GL_UNSIGNED_INT, 0);
		glEndConditionalRender();
	}
}

void draw_meshes()
{
	PVM_light_data.pass = 2;
	glBindVertexArray(mesh_data.mVao);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	for (int i = 0; i < number_of_meshes; i++)
	{
		PVM_light_data.color_mesh = meshes_color[i];
		PVM_light_data.M =	glm::translate(meshes_pos[i]) * 
							mesh_rotation[i] *
							mesh_scale;

		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PVM_light_data), &PVM_light_data);
		glDrawElements(GL_TRIANGLES, mesh_data.mNumIndices, GL_UNSIGNED_INT, 0);
	}
}


void idle()
{
	//glEndQuery(GL_TIME_ELAPSED);
	glutPostRedisplay();

	const int time_ms = glutGet(GLUT_ELAPSED_TIME);
	time_sec = 0.001f*time_ms;

	//GLint available = 0;
	//while (!available)
	{
		//glGetQueryObjectiv(TimerQuery_1, GL_QUERY_RESULT_AVAILABLE, &available);
	}

	//GLuint64 elapsed;
	//glGetQueryObjectui64v(TimerQuery_1, GL_QUERY_RESULT, &elapsed);
	//float delta_seconds = 1e-9*elapsed;
	//glBeginQuery(GL_TIME_ELAPSED_EXT, TimerQuery_1);

	//printf("delta_seconds_occlusion = %f", delta_seconds);
}

// glut keyboard callback function.
// This function gets called when an ASCII key is pressed
void keyboard(unsigned char key, int x, int y)
{
	std::cout << "key : " << key << ", x: " << x << ", y: " << y << std::endl;

	switch (key)
	{
	case 'r':
	case 'R':
		reload_shader();
		break;

	case '1':
		occlusion_ = 1;
		occlusion_flag = false;
		glutDisplayFunc(display);
		break;

	case '2':
		occlusion_ = 2;
		occlusion_flag = true;
		glutDisplayFunc(display);
		break;

	case '3':
		occlusion_ = 3;
		occlusion_flag = true;
		glutDisplayFunc(display);
		break;
	}
}

void printGlInfo()
{
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

void initOpenGl()
{
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POINT_SPRITE);
	reload_shader();

	glGenQueries(1, &timer_query);

	//SHADER 1
	glUseProgram(shader_program);

	glGenBuffers(1, &uniform_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PVM_light_uniforms), NULL, GL_STREAM_DRAW); 
	const int ubBindingPoint = 5;

	glBindBufferBase(GL_UNIFORM_BUFFER, ubBindingPoint, uniform_buffer); 
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//occluder and meshes position
	for (int i = 0; i < number_of_meshes; i++)
	{
		float x = occluders(generator);
		float z = occluders(generator);

		float x_m = meshes(generator);
		float z_m = meshes(generator);

		occluders_pos[i] = (glm::vec3(x, -0.5f, z));
		meshes_pos[i] = (glm::vec3(x_m, -0.f, z_m));
		mesh_rotation[i] = glm::rotate(mesh_rot(generator), glm::vec3(0.0, 1.0, 0.0));

		if (z < -5.0f)
			occluder_color[i] = glm::vec4(0.3, 0.3, 0.3, 1.0);

		else if (z >= -5.0f && z < 5.0f)
			occluder_color[i] = glm::vec4(0.4, 0.4, 0.4, 1.0);

		else if (z >= 5.0f)//near
			occluder_color[i] = glm::vec4(0.5, 0.5, 0.5, 1.0);

		if (z_m < -5.0f)
			meshes_color[i] = glm::vec4(0.3, 0.3, 0.3, 1.0);

		else if (z_m >= -5.0f && z_m < 5.0f)
			meshes_color[i] = glm::vec4(0.4, 0.4, 0.4, 1.0);

		else if (z_m >= 5.0f)//near
			meshes_color[i] = glm::vec4(0.5, 0.5, 0.5, 1.0);
	}

	PVM_light_data.pass = 0;
	
	PVM_light_data.La = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	PVM_light_data.Ld = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	PVM_light_data.Ls = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	PVM_light_data.ka = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
	PVM_light_data.kd = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
	PVM_light_data.ks = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	PVM_light_data.light_pos = glm::vec4(-10.0f, 10.0f, 10.0f, 1.0f);
	PVM_light_data.shininess = 8.0f;

	mesh_data = LoadMesh(mesh_name);
	occluder_data = LoadMesh(occluder_name);
	cube_data = CreateCube();

	aiVector3D box_width = mesh_data.mBbMax - mesh_data.mBbMin;
	aiVector3D box_center = 0.5f*(mesh_data.mBbMax + mesh_data.mBbMin);
	bounding_box = glm::translate(glm::vec3(box_center.x, box_center.y, box_center.z))*glm::scale(glm::vec3(box_width.x, box_width.y, box_width.z));
	mesh_scale = glm::scale(glm::vec3(mesh_data.mScaleFactor * 0.85f));

	//texture 1 mesh
	texture_id = LoadTexture(texture_name.c_str());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	int tex_loc = glGetUniformLocation(shader_program, "texColor");
	if (tex_loc != -1)
	{
		glUniform1i(tex_loc, 0);
	}

	//texture 2 tree
	texture_id_2 = LoadTexture(o_tex_name.c_str());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	int tex_loc_2 = glGetUniformLocation(shader_program, "texColor_2");
	if (tex_loc_2 != -1)
	{
		glUniform1i(tex_loc_2, 0); 
	}

	glGenQueries(1, &occlusion_timer);
	glBeginQuery(GL_TIME_ELAPSED, occlusion_timer);
	glGenQueries(number_of_meshes, occlusion_query);


	//SHADER 2
	glUseProgram(shader_program_2);

	grass_data = LoadMesh(grass_mesh);
	grass_texture_id = LoadTexture(grass_texture.c_str());

	glBindVertexArray(grass_data.mVao);

	//without instanced rendering
	for (int i = 0; i < number_of_instances; i++)
	{
		grass_pos[i] = glm::translate(glm::vec3(range_for_without_instance(generator), -5.0f, range_for_without_instance(generator)));
	}

	//with instanced rendering
	for (int i = 0; i < number_of_instances; i++)
	{
		grass_pos_i[i] = glm::translate(glm::vec3(range_for_with_instance(generator), -5.0f, range_for_with_instance(generator)));
	}

	int matrixLocation = glGetAttribLocation(shader_program_2, "pos_instanced");

	glGenBuffers(1, &matrixBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, matrixBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * grass_pos_i.size(), &grass_pos_i[0], GL_STATIC_DRAW);
	for (int i = 0; i < 4; i++)
	{
		glVertexAttribPointer(matrixLocation + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(sizeof(glm::vec4) * i));
		glEnableVertexAttribArray(matrixLocation + i);
		glVertexAttribDivisor(matrixLocation + i, 1);
	}


	//SHADER 3
	glUseProgram(shader_program_3);

	cubemap_texure_id = LoadCube(cubemap_name);

	shader_program_3 = InitShader(vertex_shader_3.c_str(), fragment_shader_3.c_str());
	cube_vao = create_cube_vao();


	//SHADER 4
	glUseProgram(shader_program_4);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glPointSize(8);

	reload_shader();
	glGenTransformFeedbacks(2, tfo);

	const GLint position_location = 0;
	const GLint velocity_location = 1;
	const GLint age_location = 2;

	float zeros[8 * num_particles] = { 0.0f };
	glGenVertexArrays(2, vao);
	glGenBuffers(2, vbo);

	//init write buffers
	glBindVertexArray(vao[Write_Index]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[Write_Index]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(zeros), zeros, GL_DYNAMIC_COPY);

	glEnableVertexAttribArray(position_location); 
	glVertexAttribPointer(position_location, 3, GL_FLOAT, false, 0, BUFFER_OFFSET(0)); 

	glEnableVertexAttribArray(velocity_location); 
	glVertexAttribPointer(velocity_location, 3, GL_FLOAT, false, 0, BUFFER_OFFSET(num_particles * sizeof(glm::vec3)));

	glEnableVertexAttribArray(age_location); 
	glVertexAttribPointer(age_location, 1, GL_FLOAT, false, 0, BUFFER_OFFSET(2 * num_particles * sizeof(glm::vec3)));

																											
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfo[Write_Index]);
	glBindBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, position_location, vbo[Write_Index], 0, num_particles * 3 * sizeof(float));
	glBindBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, velocity_location, vbo[Write_Index], num_particles * 3 * sizeof(float), num_particles * 3 * sizeof(float));
	glBindBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, age_location, vbo[Write_Index], num_particles * 6 * sizeof(float), num_particles * 1 * sizeof(float));
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

	//init read buffers
	glBindVertexArray(vao[Read_Index]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[Read_Index]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(zeros), zeros, GL_DYNAMIC_COPY);
	
	glEnableVertexAttribArray(position_location);
	glVertexAttribPointer(position_location, 3, GL_FLOAT, false, 0, BUFFER_OFFSET(0)); 
	
	glEnableVertexAttribArray(velocity_location);
	glVertexAttribPointer(velocity_location, 3, GL_FLOAT, false, 0, BUFFER_OFFSET(num_particles * sizeof(glm::vec3)));
	
	glEnableVertexAttribArray(age_location);
	glVertexAttribPointer(age_location, 1, GL_FLOAT, false, 0, BUFFER_OFFSET(2 * num_particles * sizeof(glm::vec3)));
																												 
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfo[Read_Index]);
	glBindBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, position_location, vbo[Read_Index], 0, num_particles * 3 * sizeof(float));
	glBindBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, velocity_location, vbo[Read_Index], num_particles * 3 * sizeof(float), num_particles * 3 * sizeof(float));
	glBindBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, age_location, vbo[Read_Index], num_particles * 6 * sizeof(float), num_particles * 1 * sizeof(float));
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

	glBindVertexArray(0); 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisable(GL_BLEND);
}

void keyboard_up(unsigned char key, int x, int y)
{
	ImGui_ImplGlut_KeyUpCallback(key);
}

void special_up(int key, int x, int y)
{
	ImGui_ImplGlut_SpecialUpCallback(key);
}

void passive(int x, int y)
{
	ImGui_ImplGlut_PassiveMouseMotionCallback(x, y);
}

void special(int key, int x, int y)
{
	ImGui_ImplGlut_SpecialCallback(key);
}

void motion(int x, int y)
{
	ImGui_ImplGlut_MouseMotionCallback(x, y);
}

void mouse(int button, int state, int x, int y)
{
	ImGui_ImplGlut_MouseButtonCallback(button, state);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(5, 5);
	glutInitWindowSize(1280, 720);
	int win = glutCreateWindow("CGT 521 - Final Project - Performance Analysis - Shreya Randive");

	printGlInfo();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutKeyboardUpFunc(keyboard_up);
	glutSpecialUpFunc(special_up);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(motion);
	glutIdleFunc(idle);

	initOpenGl();
	ImGui_ImplGlut_Init();
						  
	glutMainLoop();
	glutDestroyWindow(win);
	return 0;
}
