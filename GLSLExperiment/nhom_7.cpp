//Chương trình vẽ 1 hình lập phương đơn vị theo mô hình lập trình OpenGL hiện đại

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry( void );
void initGPUBuffers( void );
void shaderSetup( void );
void display( void );
void keyboard( unsigned char key, int x, int y );


typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints]; /*Danh sách các vector pháp tuyến ứng với mỗi đỉnh*/



point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;

// BEGIN: CÁC KHAI BÁO BAN ĐẦU (có thể thay đổi, khi thay đổi thì phải comment đã thay đổi những gì)
mat4 ctm;
mat4 instance;
mat4 model_view;
mat4 model_view_thung_hang;
//GLuint model_view_loc;

// khai bao cua xe
mat4 transformXe, luu;

mat4 xev;

// END: CÁC KHAI BÁO BAN ĐẦU


void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(2.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 1.2, 0.5, 1.0); // blue
	vertex_colors[5] = color4(0.3, 1.0, 0.7, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 1.0, 1.0, 1.0); // white
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}



void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry( void )
{
	initCube();
	makeColorCube();
}

void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);

}
/* Khởi tạo các tham số chiếu sáng - tô bóng*/
//chiếu sáng
point4 light_position(1.0, 0.0, 2.0, 0.0);//nguồn sáng
color4 light_ambient(0.2, 0.2, 0.2, 1.0);//ánh sáng môi trường
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);//ánh sáng khuếch tán
color4 light_specular(1.0, 1.0, 1.0, 1.0);//ánh sáng phản chiếu sáng chói

color4 material_ambient(1.0, 0.0, 1.0, 1.0);//chất liệu môi trường
color4 material_diffuse(1.0, 0.8, 0.0, 1.0);//vật liệu khếch tán
color4 material_specular(1.0, 0.8, 0.0, 1.0);//vật liệu phản chiếu sáng chói
float material_shininess = 100.0;//

//
color4 ambient_product = light_ambient * material_ambient;
color4 diffuse_product = light_diffuse * material_diffuse;
color4 specular_product = light_specular * material_specular;




GLuint model_loc, projection_loc, view_loc;
void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));


	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/
}







// BEGIN: VẼ BĂNG CHUYỀN
GLfloat theta2[] = { 0, 0, 0 };
GLfloat goc = 0;
//mat4 instance;
mat4 instance_bc;
mat4 model;
//GLuint model_loc;
GLfloat k1 = 1.0, k2 = 0.01f, k3 = 0.01f;
GLfloat k4 = 0.02, k5 = 0.5, k6 = 0.02;
void khung()
{
	// k1 là x
	// k2 là y
	// k3 là z
	instance = Translate(0.0f, 0.0f, 0.0f) * Scale(k1, k2, k3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


	instance = Translate(0.0f, 0.0f, 0.6f) * Scale(k1, k2, k3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);



	
	instance = Translate(0.5f, -0.25f, 0.0f) * Scale(k4, k5, k6);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance = Translate(0.25f, -0.25f, 0.0f) * Scale(k4, k5, k6);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance = Translate(0.0f, -0.25f, 0.0f) * Scale(k4, k5, k6);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance = Translate(-0.5f, -0.25f, 0.0f) * Scale(k4, k5, k6);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance = Translate(-0.25f, -0.25f, 0.0f) * Scale(k4, k5, k6);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0.5f, -0.25f, 0.6f) * Scale(k4, k5, k6);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance = Translate(0.25f, -0.25f, 0.6f) * Scale(k4, k5, k6);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance = Translate(0.0f, -0.25f, 0.6f) * Scale(k4, k5, k6);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance = Translate(-0.5f, -0.25f, 0.6f) * Scale(k4, k5, k6);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance = Translate(-0.25f, -0.25f, 0.6f) * Scale(k4, k5, k6);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}


	GLfloat q1 = 0.07f, q2 = 0.07f, q3 = 0.6f;
void quay()
{

	for (float i = 0; i <= 360; i += 5)
	{
		instance = Translate(-0.5f, 0.0f, 0.0f) * Scale(q1, q2, q3) * RotateZ(i) * RotateZ(goc);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}


	for (float i = 0; i <= 360; i += 5)
	{
		instance = Translate(-0.38f, 0.0f, 0.0f) * Scale(q1, q2, q3) * RotateZ(i) * RotateZ(goc);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}

	for (float i = 0; i <= 360; i += 5)
	{
		instance = Translate(-0.26f, 0.0f, 0.0f) * Scale(q1, q2, q3) * RotateZ(i) * RotateZ(goc);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}

	for (float i = 0; i <= 360; i += 5)
	{
		instance = Translate(-0.13f, 0.0f, 0.0f) * Scale(q1, q2, q3) * RotateZ(i) * RotateZ(goc);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}


	for (float i = 0; i <= 360; i += 5)
	{
		instance = Translate(-0.11f, 0.0f, 0.0f) * Scale(q1, q2, q3) * RotateZ(i) * RotateZ(goc);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}


	for (float i = 0; i <= 360; i += 5)
	{
		instance = Translate(0.01f, 0.0f, 0.0f) * Scale(q1, q2, q3) * RotateZ(i) * RotateZ(goc);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}


	for (float i = 0; i <= 360; i += 5)
	{
		instance = Translate(0.13f, 0.0f, 0.0f) * Scale(q1, q2, q3) * RotateZ(i) * RotateZ(goc);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}

	for (float i = 0; i <= 360; i += 5)
	{
		instance = Translate(0.26f, 0.0f, 0.0f) * Scale(q1, q2, q3) * RotateZ(i) * RotateZ(goc);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}

	for (float i = 0; i <= 360; i += 5)
	{
		instance = Translate(0.38f, 0.0f, 0.0f) * Scale(q1, q2, q3) * RotateZ(i) * RotateZ(goc);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}

	for (float i = 0; i <= 360; i += 5)
	{
		instance = Translate(0.5f, 0.0f, 0.0f) * Scale(q1, q2, q3) * RotateZ(i) * RotateZ(goc);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_bc * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}

}

void bangchuyen()
{
	material_diffuse = vec4(135.0 / 255, 245.0 / 255, 66.0 / 255, 1);   // mau vat (R, G, B, x) -> Blue
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance_bc = Translate(-1.5, 0, -1);
	khung();
	instance_bc *= Translate(0.0f, 0.0f, 0.31f);
	quay();


}



//END: VẼ BĂNG CHUYỀN





// BEGIN: VẼ CÁNH TAY ROBOT

//GLfloat
GLdouble BASE_HEIGHT = 0.4, BASE_WIDTH = 0.4, UPPER_ARM_HEIGHT = 0.7,
UPPER_ARM_WIDTH = 0.2, LOWER_ARM_HEIGHT = 0.6, LOWER_ARM_WIDTH = 0.1;
GLdouble MOC_HEIGHT = 0.4;
GLfloat theta[] = { 0, 0, 0 };

void base()
{
	for (float i = 0; i <= 360; i += 5)
	{
		instance = Translate(0.0, 0.5 * BASE_HEIGHT, 0.0) * RotateY(i) 
			* Scale(BASE_WIDTH, BASE_HEIGHT, BASE_WIDTH) ;
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_view * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}

	/*instance = Translate(0.0, 0.5 * BASE_HEIGHT, 0.0)
		* Scale(BASE_WIDTH, BASE_HEIGHT, BASE_WIDTH);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_view * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);*/
}
void upper_arm()
{

	instance = Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) 
		* Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_view * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void lower_arm()
{
	
	instance = Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) 
		* Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_view * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	
}

//BEGIN : VẼ MÓC

GLfloat  y_ong = 0.3 ;
void ong()
{
	material_diffuse = vec4(218.0 / 245, 90.0 / 255, 66.0 / 255, 1);   // mau vat (R, G, B, x) -> Blue
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance = Translate(0, y_ong/2, 0) * Scale(LOWER_ARM_WIDTH, y_ong, LOWER_ARM_WIDTH);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_view * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}


GLfloat x_canh = 0.05, y_canh = 0.25, z_canh = 0.2;
void canh()
{
	// canh phai
	instance = RotateZ(45) * Scale(x_canh, y_canh, z_canh);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_view * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//canh trai
	instance = Translate((y_canh / 2), 0, 0) * RotateZ(-45) * Scale(x_canh, y_canh, z_canh);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_view * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}

void thanh()
{
	
	instance = Translate(y_canh, 0, 0) * RotateZ(180) * Scale(x_canh, y_canh/2, z_canh);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_view * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	
	instance = Translate(-0.1, 0, 0) *  RotateZ(180) * Scale(x_canh, y_canh/2, z_canh);

	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_view * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

// END: VẼ MÓC



void Robot_Arm()
{

	material_diffuse = vec4(233.0 / 255, 245.0 / 255, 66.0 / 255, 1);   // mau vat (R, G, B, x) -> Blue
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	model_view = RotateY(theta[0]) ;
	base();
	model_view = model_view * Translate(0.0, BASE_HEIGHT - (UPPER_ARM_WIDTH / 2), 0.0)
		* RotateZ(theta[1]);
	upper_arm();
	model_view = model_view * Translate(0.0, UPPER_ARM_HEIGHT - (LOWER_ARM_WIDTH / 2), 0.0)
		* RotateZ(theta[2]);
	lower_arm();

	model_view = model_view * Translate(0.0, LOWER_ARM_HEIGHT, 0.0) * RotateZ(90) ;
	ong();

	model_view = model_view * Translate(0.0, y_ong, 0.0);
	canh();

	model_view = model_view * Translate(0, y_canh/2, 0.0);
	thanh();
}




// END: VẼ CÁNH TAY ROBOT









// BEGIN: VẼ THÙNG HÀNG

GLfloat LEFT = 0.2, RIGHT = 0.2, HEIGHT = 0.2;
// xx thay đổi vị trí của thùng hàng theo trục x (xx=-0.8 là giá trị ban đầu phụ thuộc vào băng chuyền)
// yy là để đặt giá vị trí của thùng hàng theo truc y
GLfloat xx = -2, yy = 0.1 , z = -0.6;
GLdouble t = 0;
GLfloat xs = -0.4;
		
void thung_hang()
{

	material_diffuse = vec4(90.0 / 255, 90.0 / 255, 166.0 / 255, 1);   // mau vat (R, G, B, x) -> Blue
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	
	// hàm di chuyển thùng hàng từ băng chuyền
	
	if( xx <= -1)
	{
		ctm = Translate(xx, yy, z) * Scale(LEFT, LEFT, LEFT);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
	else if (xx > -1 && xx < -0.8)
	{
		ctm =  Translate(0, 0.1, 0) * Scale(LEFT, LEFT, LEFT);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_view * ctm);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	}
	else
	{
		ctm = Translate(-0.6, 0, 0) *  Scale(LEFT, LEFT, LEFT);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, xev * ctm);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}


}



// END: VẼ THÙNG HÀNG









// BEGIN: VẼ XE VẬN CHUYỂN


GLfloat u = 0, dv = 0.15;
GLfloat doDayKhung = 0.01;

// di chuyen xe
GLfloat m = 0, n = 0;

// quay xe
GLfloat c;

void thanxe()
{
	instance = Scale(8 * dv, dv / 2, 2 * dv);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, xev * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void banhxe()
{
	for (int i = 0; i < 40; i++)
	{
		u += 15;
		instance = RotateZ(u) * Scale(dv, dv, dv / 3);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, xev * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}

void letren()
{
	instance = Scale(6 * dv, 2.5 * dv, 0.01);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, xev * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}

void lesau()
{
	instance = Scale(dv / 20, 2.5 * dv, 2 * dv);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, xev * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void ngoi()
{
	instance = Scale(2 * dv, 2.5 * dv, 1.5 * dv);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, xev * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}

void xe()
{
	material_diffuse = vec4(255.0 / 255, 155.0 / 255, 255.0 / 255, 1);   // mau vat (R, G, B, x) -> Blue
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	
	xev = Translate(m, 3 * dv / 2, n)  * Translate(1, 0, 0)* RotateY(c);
	thanxe();
	luu = xev; 
	//banh xe 1
	xev = luu * Translate(7 * dv / 2, -dv / 2, dv);
	banhxe();
	//banh xe 2
	xev = luu * Translate(7 * dv / 2, -dv / 2, -dv);
	banhxe();
	//banh xe 3
	xev = luu * Translate(-7 * dv / 2, -dv / 2, dv);
	banhxe();
	//banh xe 4
	xev = luu * Translate(-7 * dv / 2, -dv / 2, -dv);
	banhxe();


	material_diffuse = vec4(255.0 / 255, 155.0 / 255, 155.0 / 255, 1);   // mau vat (R, G, B, x) -> tim
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	// le tren cua xe
	xev = luu * Translate(-dv, 1.5 * dv, -dv);
	letren();
	xev = luu * Translate(-dv, 1.5 * dv, +dv);
	letren();

	// le sau cua xe
	xev = luu * Translate(-4 * dv, 1.5 * dv, 0);
	lesau();
	xev = luu * Translate(2 * dv, 1.5 * dv, 0);
	lesau();

	//ngoi
	material_diffuse = vec4(0 / 255, 155.0 / 255, 155.0 / 255, 1);   // mau vat (R, G, B, x) -> xanh luc
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	xev = luu * Translate(3 * dv, 1 * dv, 0);
	ngoi();


}

mat4 view, projection;
void reshape(int width, int height)
{
	vec4 eye(1, 1, 2, 1);
	vec4 at(0, 0, 0, 1);
	vec4 up(0, 1, 0, 1);

	view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);

	projection = Frustum(-1, 1, -1, 1, 1, 4);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);

	glViewport(0, 0, width, height);
}






// BEGIN: HÀM HIỂN THỊ


void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	const vec3 viewer_pos(0.0, 0.0, 2.0);  /*Trùng với eye của camera*/

	Robot_Arm();
	//ve_thung_hang();
	thung_hang();
	bangchuyen();
	xe();

	
	glutSwapBuffers();
}
// END: HÀM HIỂN THỊ 



// begin bảng điều khiển
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {

	// BEGIN: ĐIỀU KHIỂN BĂNG CHUYỀN
		// comment chức năng các phím ở đây: 
	

	// END: ĐIỀU KHIỂN BĂNG CHUYỀN
		



	// BEGIN: ĐIỀU KHIỂN CÁNH TAY ROBOT
		// comment chức năng các phím ở đây:
		// b,B điều khiển đế quay (base)
		// u, U: điều khiển upper-arm
		// l, L: điều khiển lower-arm 
		

	case 'b':
		xs += 0.1;
		theta[0] += 5;
		if (theta[0] > 360) theta[0] -= 360;
		glutPostRedisplay();
		break;
	case 'B':
		// một số lệnh 
		xs -= 0.1;
		theta[0] -= 5;
		if (theta[0] > 360) theta[0] -= 360;
		glutPostRedisplay();
		break;

	case 'u':
		// một số lệnh 
		theta[1] += 5;
		if (theta[1] > 360) theta[1] -= 360;
		glutPostRedisplay();
		break;
	case 'U':
		// một số lệnh 
		theta[1] -= 5;
		if (theta[1] > 360) theta[1] -= 360;
		glutPostRedisplay();
		break;

	case 'l':
		// một số lệnh 
		theta[2] += 5;
		if (theta[2] > 360) theta[2] -= 360;
		glutPostRedisplay();
		break;
	case 'L':
		// một số lệnh 
		theta[2] -= 5;
		if (theta[2] > 360) theta[2] = 0;
		glutPostRedisplay();
		break;


	//END: ĐIỀU KHIỂN CÁNH TAY ROBOT
	

	//BEGIN: ĐIỀU KHIỂN THÙNG HÀNG

	case 'x': xx += 0.1; // dieu khien ban
		glutPostRedisplay();
		break;
	case 'X': xx -= 0.1; // dieu khien ban
		glutPostRedisplay();
		break;
	case 'y': yy += 0.1; // dieu khien ban
		glutPostRedisplay();
		break;
	case 'Y': yy -= 0.1; // dieu khien ban
		glutPostRedisplay();
		break;
	case 'z': z += 0.1; // dieu khien ban
		glutPostRedisplay();
		break;
	case 'Z': z -= 0.1; // dieu khien ban
		glutPostRedisplay();
		break;

	// END: ĐIỀU KHIỂN THÙNG HÀNG




	// BEGIN: ĐIỀU KHIỂN CÁNH TAY ROBOT
		// comment chức năng các phím ở đây: 


	// END: ĐIỀU KHIỂN CÁNH TAY ROBOT
		    


// BEGIN: ĐIỀU KHIỂN XE CHO HANG

	case 'm': m += 0.1; // dieu khien ban
		glutPostRedisplay();
		break;

	case 'M': m -= 0.1; // dieu khien ban
		glutPostRedisplay();
		break;

	case 'n': n += 0.1; // dieu khien ban
		glutPostRedisplay();
		break;

	case 'N': n -= 0.1; // dieu khien ban
		glutPostRedisplay();
		break;

	case 'c':
		
		c += 5;
		if (theta[0] > 360) theta[0] -= 360;
		glutPostRedisplay();
		break;
	case 'C':
		// một số lệnh 
		
		c -= 5;
		if (theta[0] > 360) theta[0] -= 360;
		glutPostRedisplay();
		break;
	

// END: ĐIỀU KHIỂN XE CHO HANG


		
	default:
		break;
	}
}
// end bảng điều khiển





int main( int argc, char **argv )
{
	// main function: program starts here

    glutInit( &argc, argv );                       
    glutInitDisplayMode( GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize( 640, 640 );                 
	glutInitWindowPosition(100,150);               
    glutCreateWindow( " mô phỏng bốc dỡ hàng từ băng chuyền đến xa vận chuyển bằng cánh tay robot" );            

   
	glewInit();										

    generateGeometry( );                          
    initGPUBuffers( );							   
    shaderSetup( );                               

    glutDisplayFunc( display );                   
    glutKeyboardFunc( keyboard );                  
	glutReshapeFunc(reshape);

	glutMainLoop();
    return 0;
}
