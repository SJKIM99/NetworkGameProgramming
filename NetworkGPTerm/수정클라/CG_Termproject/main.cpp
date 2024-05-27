#include "objReader.h"
#include "NetworkMgr.h"
#include <mutex>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Resize(int w, int h);
void Mouse(int button, int state, int x, int y);
void Motion(int xpos, int ypos);
void Keyboard(unsigned char key, int x, int y);
GLvoid KeyBoardUP(unsigned char Key, int x, int y);

void Timer(int Value);
void make_vertexShaders(); //버텍스 세이더 만들기
void make_fragmentShaders(); //프래그먼트 세이더 만들기
void make_shaderProgram(); //세이더 프로그램 생성
void InitBuffer();
char* filetobuf(const char* file);
void InitTexture(const char* filename, unsigned int* texture);
bool startgame = false;

GLint height, width; //윈도우창 크기 
GLuint shaderProgram; //세이더 프로그램 이름
GLuint vertexShader;//버텍스 세이더 객체
GLuint fragmentShader;//프래그먼트 세이더 객체
float scene_[]
{
-0.5f, -0.5f, 0.f, 0.0, 0.0, 1.0, 0.0, 0.0,
0.5f, -0.5f, 0.f, 0.0, 0.0, 1.0, 1.0, 0.0,
0.5f, 0.5f, 0.f, 0.0, 0.0, 1.0, 1.0, 1.0,
0.5f, 0.5f, 0.f, 0.0, 0.0, 1.0, 1.0, 1.0,
-0.5f, 0.5f, 0.f, 0.0, 0.0, 1.0, 0.0, 1.0,
-0.5f, -0.5f, 0.f, 0.0, 0.0, 1.0, 0.0, 0.0
};


int hp = 100;
int u_hpscalex = 5;
struct OBJ {
	unsigned int texture;
	GLuint VAO, VBO_pos, VBO_normal, VBO_uv;
	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec2> m_uvs;
	std::vector<glm::vec3> m_normals;
};
OBJ tankbody;
OBJ block;
OBJ map;
OBJ item;
OBJ bullet;
OBJ hpblock;
OBJ startpage;
OBJ menu_hp;
OBJ winpage;
OBJ losepage;
//랜덤출력
using namespace std;

random_device rd;
default_random_engine dre(rd());
uniform_int_distribution<int> uid(-28, 28);


//함수선언
void convertDeviceXY2OpenglXY(int x, int y, float* xpos, float* ypos);
void start_page();

//충돌체크
bool collision_Chk(float aL, float aR, float aT, float aB, float bL, float bR, float bT, float bB);


bool collide_check_3(float aL, float aR, float aT, float aB, float aD, float aU, float bL, float bR, float bT, float bB, float bD, float bU);
void item_colliCHK();
bool mov_coliiCHK();
bool bullet_colliCHK(bool isfreeze);
void Setup_Block(); // 블럭 초기화
void get_Block(); // 블록 출력

//색상
struct COLOR
{
	float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;

};
COLOR color[2];


struct Player_tank {
	int id;
	int hp = 100;
	float x = 0.0, y = 0.0, z = 0.0;
	float scalex = 3;
	float speed;
	short bullet_cnt;

	float yaw;
	float bodyYaw;
	bool inGame = false;
	bool isalive = true;
}Player;

Player_tank g_players[2];

int g_attackid;

float rotate_bigY = 180; // 객체 자전

float pi = 3.141592;

//----------------------------------------------------------------------------------------------------------
GLint mapobject = loadOBJ("Resource/cube.obj", map.m_vertices, map.m_uvs, map.m_normals);
GLint blockobject = loadOBJ("Resource/cube.obj", block.m_vertices, block.m_uvs, block.m_normals);
//탱크
GLint tank_object = loadOBJ("Resource/try.obj", tankbody.m_vertices, tankbody.m_uvs, tankbody.m_normals);
GLint sphere_object = loadOBJ("Resource/sphere.obj", bullet.m_vertices, bullet.m_uvs, bullet.m_normals);
GLint itemobject = loadOBJ("Resource/cube.obj", item.m_vertices, item.m_uvs, item.m_normals);
GLint hpbar_ = loadOBJ("Resource/cube.obj", hpblock.m_vertices, hpblock.m_uvs, hpblock.m_normals);

// 장애물
struct Map_Block
{
	bool exist = false;
	float height = 0;

	COLOR b_color;

	//좌표
	float x = 0.0;
	float y = 0.0;
	float z = 0.0;

	//크기
	float x_scale = 1.0;
	float y_scale = 2.0;
	float z_scale = 1.0;

};

Map_Block Block[30][30];

bool chaseOn = true;

//----------------------------------------------------------------------------------------------------------
//  마우스
struct Mouse_Handling {
	bool left_button;
	bool first_mouse_pos_flag = true;

	float first_mouse_pos_x;
	float first_mouse_pos_y;

	float delta_mouse_x;
	float delta_mouse_y;

	float mouse_sensitivity = 179;

	float drgree_x = 90;
	float drgree_y = -30;

	float mouse_x;
	float mouse_y;
};
Mouse_Handling MH;


//----------------------------------------------------------------------------------------------------------
//카메라
struct cameraLoc
{
	//pos
	float x = 1.0, y = 1.0, z = 3.0;
	// up
	float uX = 0.0, uY = 1.0, uZ = 0.0;

}CamPos;

int position = 1; //카메라 변경

struct Camera_Option {
	float camera_distance = 3.0;

	float camera_delta_y = 0;

};
Camera_Option CO;

//카메라
glm::vec3 cameraPos = glm::vec3(CamPos.x, CamPos.y, CamPos.z); //--- 카메라 위치
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
glm::vec3 cameraUp = glm::vec3(CamPos.uX, CamPos.uY, CamPos.uZ); //--- 카메라 위쪽 방향
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);


float pitch, yaw, bodyYaw;
bool firstMouse = true;
float xpos, ypos;
float lastX, lastY;

//------------------------------------------------------------------------------------------------------
//포탄 

int k = 0; //포탄 넘버

struct Sphere_ {
	bool launch = false;
	float sphere_z = 0;
	float sphere_zz = 0;
	float now_yaw = 0;
	float x;
	float z;
	bool isfreeze;
};

struct Sphere_ sphere_[2]; // 포탄의 갯수 
glm::mat4 spheres_pos;
glm::vec4 sphere_position; //포탄의 위치 (충돌처리용)

struct Heart {
	bool exist = true;
	float x = 3.0;
	float z = 0.0;
};

struct Wheel {
	bool exist = true;
	float x = 5.0;
	float z = 0.0;
};

struct ICE {
	bool exist = true;
	float x = 9.0;
	float z = 0.0;
};

struct Heart heart[3];
struct Wheel speedup[3];
struct ICE ice[3];


//일정시간만 아이템 유지
int time_god = 0;
bool collide_god = false;
int time_ice = 0;
bool collide_ice = false;

//=======================================================================================================

bool start = false;

NetworkMgr networkmgr;
int g_myid;
int g_bullet_num;
bool g_ready;
bool g_AllPlayerReady;
bool g_bullet_reload_done = false;
bool g_want_bullet_reload = false;
bool g_not_reloadbullet = true;	// 이 변수는 장전하기 전 초기에 10발 주어졌을때 쓰는 변수
bool g_gameStart = false;
bool g_GameEnd = false;
CRITICAL_SECTION cs_recvThread;
bool g_setItem = false;
bool g_loginOk = false;

///////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD WINAPI Timer(LPVOID lpParam)
{
	while (true) {
		if (g_want_bullet_reload) {
			std::future<void> result = std::async(std::launch::async, []() {
				std::this_thread::sleep_for(std::chrono::seconds(3));
				std::cout << "장전이 완료되었습니다." << std::endl;
			});

			result.get();

			g_bullet_reload_done = true;
			g_want_bullet_reload = false;
		}

	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD WINAPI RecvThread(LPVOID lpParam)
{
	while (true) {
		int len = 0;
		char buf[BUFSIZE];
		recv(networkmgr.GetSocket(), (char*)&len, sizeof(int), MSG_WAITALL);
		recv(networkmgr.GetSocket(), buf, len, MSG_WAITALL);

		switch (buf[0]) {
		case SC_LOGIN_OK: {
			SC_LOGIN_OK_PACKET* packet = reinterpret_cast<SC_LOGIN_OK_PACKET*>(buf);
			if (g_loginOk == false) {
				g_myid = packet->id;
				g_loginOk = true;
			}
			std::cout << "[플레이어입장]  " << packet->name << "님이 입장하였습니다." << std::endl;
			g_players[g_myid].hp = packet->hp;
			g_players[g_myid].bullet_cnt = packet->bullet_cnt;
			g_players[g_myid].speed = packet->speed;
		}
						break;
		case SC_READY_OK: {
			SC_READY_OK_PACKET* packet = reinterpret_cast<SC_READY_OK_PACKET*>(buf);
		}
						break;
		case SC_GAMESTART: {
			EnterCriticalSection(&cs_recvThread);
			g_gameStart = true;
			LeaveCriticalSection(&cs_recvThread);
		}
						 break;
		case SC_UPDATE: {	//여기는 초당 30프레임으로 계속 수신받는 월드업데이트 패킷이야
			SC_UPDATE_PACKET* packet = reinterpret_cast<SC_UPDATE_PACKET*>(buf);
			EnterCriticalSection(&cs_recvThread);
			g_players[packet->id].hp = packet->hp;
			g_players[packet->id].x = packet->x;
			g_players[packet->id].y = packet->y;
			g_players[packet->id].z = packet->z;
			g_players[packet->id].speed = packet->speed;
			g_players[packet->id].bullet_cnt = packet->bullet_cnt;
			g_players[packet->id].yaw = packet->yaw;
			g_players[packet->id].id = packet->id;
			g_players[packet->id].bodyYaw = packet->bodyYaw;
			LeaveCriticalSection(&cs_recvThread);
		}
					  break;
		case SC_SET_ITEM: {
			SC_SET_ITEM_PACKET* packet = reinterpret_cast<SC_SET_ITEM_PACKET*>(buf);
			//HEAL
			if (packet->item_type == HEAL) {
				for (int i = 0; i < 3; ++i) {
					heart[i].exist = packet->exist[i];
					heart[i].x = packet->x[i];
					heart[i].z = packet->z[i];
				}
			}
			else if (packet->item_type == SPEEDUP) {
				for (int i = 0; i < 3; ++i) {
					speedup[i].exist = packet->exist[i];
					speedup[i].x = packet->x[i];
					speedup[i].z = packet->z[i];
				}
			}
			else if (packet->item_type == FREEZE) {
				for (int i = 0; i < 3; ++i) {
					ice[i].exist = packet->exist[i];
					ice[i].x = packet->x[i];
					ice[i].z = packet->z[i];
				}
			}
		}
						break;
		case SC_ALL_ITEM_SET: {
			cout << "all item setting" << endl;
			g_setItem = true;
		}
							break;

		case SC_ATTACK: {
			SC_ATTACK_PACKET* packet = reinterpret_cast<SC_ATTACK_PACKET*>(buf);
			g_attackid = packet->id;
			sphere_[g_attackid].now_yaw = packet->now_yaw;
			sphere_[g_attackid].x = packet->x;
			sphere_[g_attackid].z = packet->z;
			sphere_[g_attackid].launch = packet->isshoot;
			sphere_[g_attackid].isfreeze = packet->isfreeze;

			cout << packet->id << endl;
			cout << packet->now_yaw << endl;
			cout << packet->x << endl;
			cout << packet->z << endl;
			cout << packet->isshoot << endl;
			cout << packet->isfreeze << endl;

		}
							 break;
		case SC_RELOAD: {
			SC_RELOAD_PACKET* packet = reinterpret_cast<SC_RELOAD_PACKET*>(buf);
			g_players[g_myid].bullet_cnt = packet->bullet_num;
			cout << g_myid << "번 클라이언트 재장전 완료 남은 총알 수 : " << packet->bullet_num << endl;
		}
					  break;
		case SC_DIE_PLAYER: {
			SC_DIE_PACKET* packet = reinterpret_cast<SC_DIE_PACKET*>(buf);
			int playerdie_id = packet->id;
			g_players[playerdie_id].isalive = false;
			cout << playerdie_id << "번 플레이어 사망" << endl;
			cout << "게임을 종료합니다." << endl;
			g_GameEnd = true;
			return 0;
		}
						  break;
		}
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{ //--- 윈도우 생성하기\
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	color[0].r = 0.4; color[0].g = 0.8; color[0].b = 0.4;
	color[1].r = 0.7; color[1].g = 0.4; color[1].b = 0.7;
	InitializeCriticalSection(&cs_recvThread);
	if (!networkmgr.Init()) {
		std::cout << "Init Socket error " << std::endl;
		return;
	}
	char name[NAME_SIZE];
	cout << "이름을 입력하세요 : ";
	cin >> name;


	CS_LOGIN_PACKET* packet = new CS_LOGIN_PACKET;
	packet->type = CS_LOGIN;
	strncpy(packet->name, name, NAME_SIZE);
	networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(CS_LOGIN_PACKET));
	delete packet;

	///////////////////////////////////////////////////////////////////////////////////////////////////////

	height = 700;
	width = 700;
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
	glutInitWindowSize(width, height); // 윈도우의 크기 지정
	glutCreateWindow("project");
	glewExperimental = GL_TRUE;
	glewInit();


	//make shaders
	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	make_shaderProgram(); //--- 세이더 프로그램 만들기

	InitBuffer();
	InitTexture("Texture/T_grass.png", &block.texture);
	InitTexture("Texture/T_lifehart.png", &hpblock.texture);
	InitTexture("Texture/T_grass.png", &map.texture);
	InitTexture("Texture/startpage2.png", &startpage.texture);
	InitTexture("Texture/T_tank.png", &tankbody.texture);
	//InitTexture("Texture/T_item1.png", &item.texture);
	InitTexture("Texture/menu_hp3.png", &menu_hp.texture);
	InitTexture("Texture/winpage.png", &winpage.texture);
	InitTexture("Texture/losepage.png", &losepage.texture);
	//call back
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	
	glutKeyboardFunc(Keyboard);		//키보드
	glutKeyboardUpFunc(KeyBoardUP);

	glutPassiveMotionFunc(Motion);//마우스 움직임
	glutMouseFunc(Mouse);//마우스 콜백
	glutTimerFunc(30, Timer, 1); //애니매이션 타이머

	//Recv쓰레드 생성 - 서버로부터 데이터 수신 계속 받을거얌
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	HANDLE TimerThread = CreateThread(NULL, 0, Timer, 0, 0, 0);
	if (TimerThread == NULL) {
		closesocket(networkmgr.GetSocket());
	}
	else {
		CloseHandle(TimerThread);
	}
	HANDLE hThread = CreateThread(NULL, 0, RecvThread, 0, 0, 0);
	if (hThread == NULL) {
		closesocket(networkmgr.GetSocket());
	}
	else {
		CloseHandle(hThread);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////

	glutMainLoop(); // 이벤트 처리 시작 

	DeleteCriticalSection(&cs_recvThread);
}

//====================================================================================================



GLvoid InitBuffer() {

	//---------------------------------------------------------------------------------------------------------------------------
	//탱크 버퍼

	glGenVertexArrays(1, &tankbody.VAO);
	glBindVertexArray(tankbody.VAO);

	glGenBuffers(1, &tankbody.VBO_pos);
	glBindBuffer(GL_ARRAY_BUFFER, tankbody.VBO_pos);
	glBufferData((GL_ARRAY_BUFFER), tankbody.m_vertices.size() * sizeof(glm::vec3), &tankbody.m_vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &tankbody.VBO_normal);
	glBindBuffer(GL_ARRAY_BUFFER, tankbody.VBO_normal);
	glBufferData(GL_ARRAY_BUFFER, tankbody.m_normals.size() * sizeof(glm::vec3), &tankbody.m_normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &tankbody.VBO_uv);
	glBindBuffer(GL_ARRAY_BUFFER, tankbody.VBO_uv);
	glBufferData(GL_ARRAY_BUFFER, tankbody.m_uvs.size() * sizeof(glm::vec2), &tankbody.m_uvs.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(2);

	//맵 버퍼---------------------------------------------------------------------------------------------------------------------------

	glGenVertexArrays(1, &map.VAO);
	glBindVertexArray(map.VAO);

	glGenBuffers(1, &map.VBO_pos);
	glBindBuffer(GL_ARRAY_BUFFER, map.VBO_pos);
	glBufferData((GL_ARRAY_BUFFER), map.m_vertices.size() * sizeof(glm::vec3), &map.m_vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &map.VBO_normal);
	glBindBuffer(GL_ARRAY_BUFFER, map.VBO_normal);
	glBufferData(GL_ARRAY_BUFFER, map.m_normals.size() * sizeof(glm::vec3), &map.m_normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &map.VBO_uv);
	glBindBuffer(GL_ARRAY_BUFFER, map.VBO_uv);
	glBufferData(GL_ARRAY_BUFFER, map.m_uvs.size() * sizeof(glm::vec2), &map.m_uvs.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(2);

	//hp바--------------------------------------------------------------------------------------------------------------------------------

	glGenVertexArrays(1, &hpblock.VAO);
	glGenBuffers(1, &hpblock.VBO_pos);
	glGenBuffers(1, &hpblock.VBO_normal);
	glGenBuffers(1, &hpblock.VBO_uv);

	glUseProgram(shaderProgram);
	glBindVertexArray(hpblock.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, hpblock.VBO_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(scene_), scene_, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, hpblock.VBO_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(scene_), scene_, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, hpblock.VBO_uv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(scene_), scene_, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	//시작화면-------------------------------------------------------------------------------------------------------------------------
	glGenVertexArrays(1, &startpage.VAO);
	glGenBuffers(1, &startpage.VBO_pos);
	glGenBuffers(1, &startpage.VBO_normal);
	glGenBuffers(1, &startpage.VBO_uv);

	glUseProgram(shaderProgram);
	glBindVertexArray(startpage.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, startpage.VBO_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(scene_), scene_, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, startpage.VBO_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(scene_), scene_, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, startpage.VBO_uv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(scene_), scene_, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//블록-----------------------------------------------------------------------------------------------------------------------------
	glGenVertexArrays(1, &block.VAO);
	glBindVertexArray(block.VAO);

	glGenBuffers(1, &block.VBO_pos);
	glBindBuffer(GL_ARRAY_BUFFER, block.VBO_pos);
	glBufferData((GL_ARRAY_BUFFER), block.m_vertices.size() * sizeof(glm::vec3), &block.m_vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &block.VBO_normal);
	glBindBuffer(GL_ARRAY_BUFFER, block.VBO_normal);
	glBufferData(GL_ARRAY_BUFFER, block.m_normals.size() * sizeof(glm::vec3), &block.m_normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	//-------------------------------------------------------------------------------------------------------------------------------
	//포탄
	glGenVertexArrays(1, &bullet.VAO);
	glBindVertexArray(bullet.VAO);

	glGenBuffers(1, &bullet.VBO_pos);
	glBindBuffer(GL_ARRAY_BUFFER, bullet.VBO_pos);
	glBufferData((GL_ARRAY_BUFFER), bullet.m_vertices.size() * sizeof(glm::vec3), &bullet.m_vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &bullet.VBO_normal);
	glBindBuffer(GL_ARRAY_BUFFER, bullet.VBO_normal);
	glBufferData(GL_ARRAY_BUFFER, bullet.m_normals.size() * sizeof(glm::vec3), &bullet.m_normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	//아이템 -------------------------------------------------------------------------------------------------------------------
	glGenVertexArrays(1, &item.VAO);
	glBindVertexArray(item.VAO);

	glGenBuffers(1, &item.VBO_pos);
	glBindBuffer(GL_ARRAY_BUFFER, item.VBO_pos);
	glBufferData((GL_ARRAY_BUFFER), item.m_vertices.size() * sizeof(glm::vec3), &item.m_vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &item.VBO_normal);
	glBindBuffer(GL_ARRAY_BUFFER, item.VBO_normal);
	glBufferData(GL_ARRAY_BUFFER, item.m_normals.size() * sizeof(glm::vec3), &item.m_normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	//menu_hp-------------------------------------------------------------------------------------------------------------------
	glGenVertexArrays(1, &menu_hp.VAO);
	glGenBuffers(1, &menu_hp.VBO_pos);
	glGenBuffers(1, &menu_hp.VBO_normal);
	glGenBuffers(1, &menu_hp.VBO_uv);

	glUseProgram(shaderProgram);
	glBindVertexArray(menu_hp.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, menu_hp.VBO_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(scene_), scene_, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, menu_hp.VBO_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(scene_), scene_, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, menu_hp.VBO_uv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(scene_), scene_, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// -------------------------------------------------------------------------------------------------------------------------
	//light
	glUseProgram(shaderProgram);
	unsigned int lightPosLocation = glGetUniformLocation(shaderProgram, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, 0.0, 0.0, 2.0);
	unsigned int lightColorLocation = glGetUniformLocation(shaderProgram, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, 1.0, 1.0, 1.0);


	unsigned int viewPosLocation = glGetUniformLocation(shaderProgram, "viewPos"); //--- viewPos 값 전달: 카메라 위치
	glUniform3f(viewPosLocation, CamPos.x, CamPos.y, CamPos.z);
}
//
//====================================================================================================


void start_page() {

	glClearColor(0.0, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//뷰 변환
	glm::mat4 view = glm::mat4(1.0f);
	cameraPos.x = 0.0; //카메라 위치를 탱크 위치로 고정
	cameraPos.z = 0.0;
	cameraPos.y = 65.0; //카메라를 위쪽(y축)으로 조정

	glm::vec3 Diretion = glm::vec3(0.0, 0.0, 0.0);
	cameraUp = glm::vec3(0.0, 0.0, -1.0); //--- 카메라 위쪽 방향


	view = glm::lookAt(cameraPos, Diretion, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(shaderProgram, "viewTransform"); //--- 뷰잉 변환 설정
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);


	//투영변환
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -3.0));//--- 공간을 약간 뒤로 미뤄줌
	unsigned int projectionLocation = glGetUniformLocation(shaderProgram, "projectionTransform"); //--- 투영 변환 값 설정
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

}


GLvoid drawObj() {
	int modeltrans = glGetUniformLocation(shaderProgram, "modeltrans");
	unsigned int objColorLocation = glGetUniformLocation(shaderProgram, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
	if (startgame) {
		for (int index = 0; index < MAX_USER; ++index) {
			//탱크
			if (g_players[index].isalive == true)
			{

			}
			glBindVertexArray(tankbody.VAO);
			glm::mat4 model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(g_players[index].x, g_players[index].y, g_players[index].z));
			model = glm::rotate(model, glm::radians(rotate_bigY), glm::vec3(0.0, 1.0, 0.0));

			if (index == g_myid) {
				model = glm::rotate(model, glm::radians(MH.drgree_x), glm::vec3(0.0, 1.0, 0.0));
			}
			else {
				model = glm::rotate(model, glm::radians(g_players[index].bodyYaw), glm::vec3(0.0, 1.0, 0.0));
			}

			modeltrans = glGetUniformLocation(shaderProgram, "modeltrans");
			glUniformMatrix4fv(modeltrans, 1, GL_FALSE, glm::value_ptr(model));
			int objColorLocation = glGetUniformLocation(shaderProgram, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
			glUniform3f(objColorLocation, color[index].r, color[index].g, color[index].b);
			glBindTexture(GL_TEXTURE_2D, tankbody.texture);

			//glDrawArrays(GL_TRIANGLES, 0, tankbody.m_vertices.size());
			//35931 (vertex number)
			//body 
			glBindVertexArray(tankbody.VAO);
			glDrawArrays(GL_TRIANGLES, 0, 21000);


			glm::mat4 head = glm::mat4(1.0);
			if (index == g_myid) {
				head = glm::rotate(head, glm::radians(yaw), glm::vec3(0.0, 1.0, 0.0));
			}
			else {
				head = glm::rotate(head, glm::radians(g_players[index].yaw), glm::vec3(0.0, 1.0, 0.0));
			}
			head = model * head;
			glUniformMatrix4fv(modeltrans, 1, GL_FALSE, glm::value_ptr(head));

			//head
			glBindVertexArray(tankbody.VAO);
			glDrawArrays(GL_TRIANGLES, 21000, tank_object);


			//glDrawArrays(GL_TRIANGLES, 0, tank_object);

			//-------------------------------------------------------------------------------------------------------------------------------
			//맵
			glBindVertexArray(map.VAO);
			glm::mat4 map_floor = glm::mat4(1.0);
			map_floor = glm::translate(map_floor, glm::vec3(0, -1.5, 0));
			map_floor = glm::scale(map_floor, glm::vec3(30.0, 0.1, 30.0));

			unsigned int floor_model = glGetUniformLocation(shaderProgram, "modeltrans");
			glUniformMatrix4fv(floor_model, 1, GL_FALSE, glm::value_ptr(map_floor));

			unsigned int mapColor = glGetUniformLocation(shaderProgram, "objectColor");
			glUniform3f(objColorLocation, 0.3, 0.3, 0.3);

			glBindTexture(GL_TEXTURE_2D, map.texture);
			glDrawArrays(GL_TRIANGLES, 0, map.m_vertices.size());



			//장애물 - 블록
			get_Block();


			//--------------------------------------------------------------------------------------------------
			//포탄
			if (sphere_[index].launch) {
				glm::mat4 spheres_scale = glm::mat4(1.0);
				spheres_pos = glm::mat4(1.0);
				if (!(g_players[index].x == 0 && g_players[index].y == 0 && g_players[index].z == 0)) { //탱크의 위치가 원점이 아닌 경우 
					spheres_pos = glm::translate(spheres_pos, glm::vec3(sphere_[index].x, 0, sphere_[index].z)); //원점에서 회전 후 탱크의 이동으로 위치 변경 
				}
				spheres_pos = glm::rotate(spheres_pos, glm::radians(sphere_[index].now_yaw), glm::vec3(0.0, 1.0, 0.0));
				spheres_pos = glm::translate(spheres_pos, glm::vec3(0.0, 0.0, sphere_[index].sphere_zz));
				spheres_scale = glm::scale(spheres_scale, glm::vec3(0.17, 0.17, 0.17));
				spheres_pos = spheres_pos * spheres_scale;
				glUniformMatrix4fv(modeltrans, 1, GL_FALSE, glm::value_ptr(spheres_pos));
				glBindVertexArray(bullet.VAO);
				glUniform3f(objColorLocation, 1.0, 1.0, 1.0);
				glDrawArrays(GL_TRIANGLES, 0, item.m_vertices.size());
				glDrawArrays(GL_TRIANGLES, 0, itemobject);
			}
		}
		//---------------------------------------------------------------------------------------------------------
		//아이템

		if (g_setItem) {
			//체력 회복
			for (int i = 0; i < 3; i++) {
				if (heart[i].exist) {
					glm::mat4 hearts = glm::mat4(1.0);
					hearts = glm::translate(hearts, glm::vec3(heart[i].x, 0, heart[i].z));
					hearts = glm::scale(hearts, glm::vec3(0.2, 0.2, 0.2));
					glUniformMatrix4fv(modeltrans, 1, GL_FALSE, glm::value_ptr(hearts));
					glBindVertexArray(item.VAO);
					glUniform3f(objColorLocation, 1.0, 0.0, 0.0);
					glDrawArrays(GL_TRIANGLES, 0, itemobject);
				}
			}
			//속도 증가
			for (int i = 0; i < 3; i++) {
				if (speedup[i].exist) {
					glm::mat4 velocity = glm::mat4(1.0);
					velocity = glm::translate(velocity, glm::vec3(speedup[i].x, 0, speedup[i].z));
					velocity = glm::scale(velocity, glm::vec3(0.2, 0.2, 0.2));
					glUniformMatrix4fv(modeltrans, 1, GL_FALSE, glm::value_ptr(velocity));
					glBindVertexArray(item.VAO);
					glUniform3f(objColorLocation, 0.0, 0.0, 1.0);
					glDrawArrays(GL_TRIANGLES, 0, itemobject);
				}
			}
			//좀비 얼리기
			for (int i = 0; i < 3; i++) {
				if (ice[i].exist) {
					glm::mat4 freeze = glm::mat4(1.0);
					freeze = glm::translate(freeze, glm::vec3(ice[i].x, 0, ice[i].z));
					freeze = glm::scale(freeze, glm::vec3(0.2, 0.2, 0.2));
					glUniformMatrix4fv(modeltrans, 1, GL_FALSE, glm::value_ptr(freeze));
					glBindVertexArray(item.VAO);
					glUniform3f(objColorLocation, 1.0, 1.0, 0.0);
					glDrawArrays(GL_TRIANGLES, 0, itemobject);
				}
			}

		}
	}
}



GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//배경색 지정 
	glClearColor(0.3, 0.5f, 0.8f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//깊이 검사 
	glEnable(GL_DEPTH_TEST);
	glUseProgram(shaderProgram);

	if (start) {
		glViewport(0, 0, width, height);
		start_page();
		drawObj();
	}

	else {


		//-------------------------------------------------------------------------------------------------------------------------------


		//뷰 변환
		glm::mat4 view = glm::mat4(1.0f);

		//카메라 위치
		cameraPos.x = g_players[g_myid].x;
		cameraPos.z = g_players[g_myid].z;
		cameraPos.y = g_players[g_myid].y + 1.5;


		cameraDirection.x = g_players[g_myid].x - sin((MH.drgree_x) * pi / 180);
		cameraDirection.y = g_players[g_myid].y + 1.3;
		cameraDirection.z = g_players[g_myid].z - cos((MH.drgree_x) * pi / 180);


		cameraUp = glm::vec3(0.0, 1.0, 0.0);

		//1인칭
		glViewport(0, 0, width, height);
		glm::vec3 cameraPos_1;
		cameraPos_1.x = g_players[g_myid].x; //카메라 위치를 탱크 위치로 고정
		cameraPos_1.z = g_players[g_myid].z;
		cameraPos_1.z -= 1.3;
		cameraPos_1.y = 0.7; //카메라를 위쪽(y축)으로 조정
		glm::vec3 cameraFront_1 = glm::vec3(0.0, 0.0, -1.0);

		//1인칭
		if (position == 0) {
			view = glm::lookAt(cameraPos_1, cameraPos_1 + cameraFront_1, cameraUp);
		}
		//3인칭
		else if (position == 1) {
			view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		}

		/*view = glm::lookAt(cameraPos, cameraDirection, cameraUp);*/
		unsigned int viewLocation = glGetUniformLocation(shaderProgram, "viewTransform"); //--- 뷰잉 변환 설정
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
		drawObj();


		//-------------------------------------------------------------------------------------------------------------------------------
		//투영변환
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
		projection = glm::translate(projection, glm::vec3(0.0, 0.0, -3.0));//--- 공간을 약간 뒤로 미뤄줌
		unsigned int projectionLocation = glGetUniformLocation(shaderProgram, "projectionTransform"); //--- 투영 변환 값 설정
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);


		drawObj();

		//충돌처리----------------------------------------------------------------------------------------------
		//충돌 처리용 포탄 위치 기록
		glm::vec3 temp_position = glm::vec3(1.0);
		sphere_position = spheres_pos * glm::vec4(temp_position, 1.0);
		if (startgame == false) {
			//startpage
			{glViewport(0, 0, width, height);
			view = glm::mat4(1.0f);
			glm::vec3 map_pos = glm::vec3(0.0, 0.0, 1.0); //--- 카메라 위치
			glm::vec3 mapDir = glm::vec3(0.0, 0.0f, 0.0f); //--- 카메라 바라보는 방향
			glm::vec3 mapUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
			view = glm::lookAt(map_pos, mapDir, mapUp);

			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
			unsigned int viewLocation = glGetUniformLocation(shaderProgram, "viewTransform");
			glBindVertexArray(startpage.VAO);
			glm::mat4 model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(0, 0, 0));
			model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));
			model = glm::scale(model, glm::vec3(3.5, 3.5, 3.5));

			int modeltrans = glGetUniformLocation(shaderProgram, "modeltrans");
			glUniformMatrix4fv(modeltrans, 1, GL_FALSE, glm::value_ptr(model));
			unsigned int objColorLocation = glGetUniformLocation(shaderProgram, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
			glUniform3f(objColorLocation, 1.0, 1.0, 1.0);

			glBindTexture(GL_TEXTURE_2D, startpage.texture);
			glBindVertexArray(startpage.VAO);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glUseProgram(shaderProgram); }
		}
		//menu_hp
		{
			{glViewport(0, 0, width, height);
			view = glm::mat4(1.0f);
			glm::vec3 map_pos = glm::vec3(0.0, 0.0, 1.0); //--- 카메라 위치
			glm::vec3 mapDir = glm::vec3(0.0, 0.0f, 0.0f); //--- 카메라 바라보는 방향
			glm::vec3 mapUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
			view = glm::lookAt(map_pos, mapDir, mapUp);

			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
			unsigned int viewLocation = glGetUniformLocation(shaderProgram, "viewTransform");
			glBindVertexArray(hpblock.VAO);
			glm::mat4 model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-1, 3, -5));
			model = glm::rotate(model, glm::radians(0.f), glm::vec3(0.0, 1.0, 0.0));
			model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));

			int modeltrans = glGetUniformLocation(shaderProgram, "modeltrans");
			glUniformMatrix4fv(modeltrans, 1, GL_FALSE, glm::value_ptr(model));
			unsigned int objColorLocation = glGetUniformLocation(shaderProgram, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
			glUniform3f(objColorLocation, 1.0, 0.0, 0.0);

			glBindTexture(GL_TEXTURE_2D, menu_hp.texture);
			glBindVertexArray(menu_hp.VAO);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glUseProgram(shaderProgram); }
		}
		int hpbar = g_players[g_myid].hp / 25;
		//hpbar
		for (int i{}; i < hpbar; ++i) {
			{glViewport(0, 0, width, height);
			view = glm::mat4(1.0f);
			glm::vec3 map_pos = glm::vec3(0.0, 0.0, 1.0); //--- 카메라 위치
			glm::vec3 mapDir = glm::vec3(0.0, 0.0f, 0.0f); //--- 카메라 바라보는 방향
			glm::vec3 mapUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
			view = glm::lookAt(map_pos, mapDir, mapUp);

			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
			unsigned int viewLocation = glGetUniformLocation(shaderProgram, "viewTransform");
			glBindVertexArray(hpblock.VAO);
			glm::mat4 model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(i * 0.5 - 0.5, 3, -5));
			model = glm::rotate(model, glm::radians(rotate_bigY), glm::vec3(0.0, 1.0, 0.0));
			model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));

			int modeltrans = glGetUniformLocation(shaderProgram, "modeltrans");
			glUniformMatrix4fv(modeltrans, 1, GL_FALSE, glm::value_ptr(model));
			unsigned int objColorLocation = glGetUniformLocation(shaderProgram, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
			glUniform3f(objColorLocation, 1.0, 0.0, 0.0);

			glBindTexture(GL_TEXTURE_2D, hpblock.texture);
			glBindVertexArray(hpblock.VAO);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glUseProgram(shaderProgram); }


		}
		if (g_players[0].isalive == false || g_players[1].isalive == false) {   //gameend
			if (g_players[g_myid].isalive == false) { //죽음
				{glViewport(0, 0, width, height);
				view = glm::mat4(1.0f);
				glm::vec3 map_pos = glm::vec3(0.0, 0.0, 1.0); //--- 카메라 위치
				glm::vec3 mapDir = glm::vec3(0.0, 0.0f, 0.0f); //--- 카메라 바라보는 방향
				glm::vec3 mapUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
				view = glm::lookAt(map_pos, mapDir, mapUp);

				glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
				unsigned int viewLocation = glGetUniformLocation(shaderProgram, "viewTransform");
				glBindVertexArray(startpage.VAO);
				glm::mat4 model = glm::mat4(1.0);
				model = glm::translate(model, glm::vec3(0, 0, 0));
				model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));
				model = glm::scale(model, glm::vec3(3.5, 3.5, 3.5));

				int modeltrans = glGetUniformLocation(shaderProgram, "modeltrans");
				glUniformMatrix4fv(modeltrans, 1, GL_FALSE, glm::value_ptr(model));
				unsigned int objColorLocation = glGetUniformLocation(shaderProgram, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색.
				glUniform3f(objColorLocation, 1.0, 1.0, 1.0);

				glBindTexture(GL_TEXTURE_2D, losepage.texture);
				glBindVertexArray(startpage.VAO);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glUseProgram(shaderProgram); }
			}
			else {
				{glViewport(0, 0, width, height);
				view = glm::mat4(1.0f);
				glm::vec3 map_pos = glm::vec3(0.0, 0.0, 1.0); //--- 카메라 위치
				glm::vec3 mapDir = glm::vec3(0.0, 0.0f, 0.0f); //--- 카메라 바라보는 방향
				glm::vec3 mapUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
				view = glm::lookAt(map_pos, mapDir, mapUp);

				glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
				unsigned int viewLocation = glGetUniformLocation(shaderProgram, "viewTransform");
				glBindVertexArray(startpage.VAO);
				glm::mat4 model = glm::mat4(1.0);
				model = glm::translate(model, glm::vec3(0, 0, 0));
				model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));
				model = glm::scale(model, glm::vec3(3.5, 3.5, 3.5));

				int modeltrans = glGetUniformLocation(shaderProgram, "modeltrans");
				glUniformMatrix4fv(modeltrans, 1, GL_FALSE, glm::value_ptr(model));
				unsigned int objColorLocation = glGetUniformLocation(shaderProgram, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
				glUniform3f(objColorLocation, 1.0, 1.0, 1.0);

				glBindTexture(GL_TEXTURE_2D, winpage.texture);
				glBindVertexArray(startpage.VAO);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glUseProgram(shaderProgram); }
			}

		}
		//-------------------------------------------------------------------------------------------------
	}
	glutSwapBuffers(); // 화면에 출력하기
}

//====================================================================================================
//애니메이션
void Timer(int Value) 
{
	item_colliCHK();

	MH.first_mouse_pos_flag = true;

	//포탄 
	for (int i = 0; i < MAX_USER; ++i) {
		if (sphere_[i].isfreeze == true) {
			if (sphere_[i].launch) {
				sphere_[i].sphere_zz -= 2.0;

				if (sphere_[i].sphere_zz < -4.0)
				{
					if (bullet_colliCHK(sphere_[i].isfreeze))
					{
						sphere_[i].launch = false;
						sphere_[i].sphere_zz = 0;
					}
				}
				if (sphere_[i].sphere_zz == -30.0f) {
					sphere_[i].launch = false;
					sphere_[i].sphere_zz = 0;
				}
			}
		}
		else {
			if (sphere_[i].launch) {
				sphere_[i].sphere_zz -= 2.0;

				if (sphere_[i].sphere_zz < -4.0)
				{
					if (bullet_colliCHK(sphere_[i].isfreeze))
					{
						sphere_[i].launch = false;
						sphere_[i].sphere_zz = 0;
					}
				}
				if (sphere_[i].sphere_zz == -30.0f) {
					sphere_[i].launch = false;
					sphere_[i].sphere_zz = 0;
				}
			}
		}
	}

	//hpbar();
	glutTimerFunc(30, Timer, 1);
	glutPostRedisplay();

}

//====================================================================================================
//키보드 입력

//float speed = 0.3;
void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {

	//---------------------------------------------------------		
	//탱크 이동
	case 'w': {
		CS_MOVE_PACKET* packet = new CS_MOVE_PACKET;
		packet->type = CS_MOVE;
		packet->direction = DIRECTION::UP;
		packet->bodyYaw = bodyYaw;
		networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(CS_MOVE_PACKET));
		delete packet;
	}
			break;
	case 'a': {
		CS_MOVE_PACKET* packet = new CS_MOVE_PACKET;
		packet->type = CS_MOVE;
		packet->direction = DIRECTION::LEFT;
		packet->bodyYaw = bodyYaw;
		//cout << "Input a" << endl;
		networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(CS_MOVE_PACKET));
		delete packet;
	}
			break;
	case 's': {
		CS_MOVE_PACKET* packet = new CS_MOVE_PACKET;
		packet->type = CS_MOVE;
		packet->direction = DIRECTION::DOWN;
		packet->bodyYaw = bodyYaw;
		//cout << "Input s" << endl;
		networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(CS_MOVE_PACKET));
		delete packet;
	}
			break;
	case 'd': {
		CS_MOVE_PACKET* packet = new CS_MOVE_PACKET;
		packet->type = CS_MOVE;
		packet->direction = DIRECTION::RIGHT;
		packet->bodyYaw = bodyYaw;
		//cout << "Input d" << endl;
		networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(CS_MOVE_PACKET));
		delete packet;
	}
			break;

	//---------------------------------------------------------		
	// 레디
	case 'r': {
		if (g_AllPlayerReady) {
			cout << "이미 게임이 시작되어 레디키를 누를 수 없습니다." << endl;
			break;
		}
		CS_READY_PACKET* packet = new CS_READY_PACKET;
		packet->type = CS_READY;
		if (g_ready == false) {
			cout << "클라이언트가 레디합니다." << endl;
			g_ready = true;
		}
		else {
			cout << "클라이언트가 언레디합니다." << endl;
			g_ready = false;
		}
		networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(CS_READY_PACKET));
		delete packet;
	}
			break;
	//---------------------------------------------------------		
	// 탱크 머리 회전
	case 'q': {
		yaw += 10;

		if (yaw > 89.0f)
			yaw = 89.0f;
		if (yaw < -89.0f)
			yaw = -89.0f;


		CS_YAW_PACKET* packet = new CS_YAW_PACKET;
		packet->type = CS_YAW;
		packet->yaw = yaw;
		packet->bodyYaw = MH.drgree_x;
		networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(CS_YAW_PACKET));
		delete packet;
	}	break;

	case 'e': { 
		yaw -= 10;

		if (yaw > 89.0f)
			yaw = 89.0f;
		if (yaw < -89.0f)
			yaw = -89.0f;


		CS_YAW_PACKET* packet = new CS_YAW_PACKET;
		packet->type = CS_YAW;
		packet->yaw = yaw;
		packet->bodyYaw = MH.drgree_x;
		networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(CS_YAW_PACKET));
		delete packet;
	} break;

	//---------------------------------------------------------		
	// 재장전

	case 'f': {
		if (! g_want_bullet_reload) g_want_bullet_reload = true;
		g_not_reloadbullet = false;
		CS_RELOAD_PACKET* packet = new CS_RELOAD_PACKET;
		packet->type = CS_RELOAD;
		networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(CS_RELOAD_PACKET));
		delete packet;
	} break;


	case VK_SPACE:
	{
		//포탄 발사
		if (g_setItem) {
			if (g_bullet_reload_done || g_not_reloadbullet) {
				if (g_players[g_myid].bullet_cnt > 0) {
					//sphere_[k].now_yaw = yaw;
					//sphere_[k].launch = true;
					PlaySound(TEXT("gun.wav"), NULL, SND_FILENAME | SND_ASYNC);

					CS_ATTACK_PACKET* packet = new CS_ATTACK_PACKET;
					packet->tpye = CS_ATTACK;
					packet->now_yaw = yaw + bodyYaw;
					packet->x = g_players[g_myid].x;
					packet->z = g_players[g_myid].z;
					packet->isshoot = true;

					networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof CS_ATTACK_PACKET);
					delete packet;
				}
			}
		}
	}
		break;

		//게임 시작
	case '1':
		if (start) start = false;
		else start = true;
		break;
	case 'h':
		startgame = true;
		break;
	//디버깅용
	//종료

	case 'm': {
		CS_RELOAD_PACKET* packet = new CS_RELOAD_PACKET;
		packet->type = CS_RELOAD;
		networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(CS_RELOAD_PACKET));
		delete packet;
	}
		break;

	case 'M':
		glutLeaveMainLoop();
		break;



	}

	glutPostRedisplay();
}


GLvoid KeyBoardUP(unsigned char Key, int x, int y) 
{

}


//==충돌 체크=============================================================================================
bool collision_Chk(float aL, float aR, float aT, float aB, float bL, float bR, float bT, float bB) {

	if (bB <= aT || bT >= aB || bR <= aL || bL >= aR) return false;
	return true;
}

//벽&탱크
bool mov_coliiCHK() {
	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 30; j++)
		{
			if (Block[i][j].exist)
			{
				if (collision_Chk(Block[i][j].x - 1.0, Block[i][j].x + 1.0, Block[i][j].z - 2.0, Block[i][j].z + 2.0,
					Player.x - 0.5, Player.x + 0.5, Player.z - 0.5, Player.z + 0.5))
				{

					return true;
				}
			}

		}

	}
	return false;

}


//3차원 충돌 (수정 필요..)
bool collide_check_3(float aL, float aR, float aT, float aB, float aD, float aU, float bL, float bR, float bT, float bB, float bD, float bU) {
	if (aL <= bR && aR >= bL
		&& aB <= bT && aT >= bB
		&& aD <= bU && aU >= bD)
		return true;
	return false;
}


bool bullet_colliCHK(bool isfreeze) {
	glm::vec3 temp_position = glm::vec3(1.0);
	sphere_position = spheres_pos * glm::vec4(temp_position, 1.0);

	for (int i = 0; i < MAX_USER; i++)
	{
		// 상대방 정보로 비교
		if (g_myid != g_players[i].id)	// 제작 중 확인해 보니 클라 id를 안받아오고 초기화 되어있는 상태 그대로임 -> 수정함
		{
			if (collision_Chk(
				g_players[i].x - 1.2, g_players[i].x + 1.2, g_players[i].z - 0.7, g_players[i].z + 0.7,
				sphere_position.x - 0.1, sphere_position.x + 0.1, sphere_position.z - 0.1, sphere_position.z + 0.1)
				)
			{
				//cout << "상대 위치" << g_players[i].x << " / " << g_players[i].z << endl;
				//cout << "내 위치" << g_players[g_myid].x << " / " << g_players[g_myid].z << endl;
				//cout << "총알 위치" << sphere_position.x << " / " << sphere_position.z << endl;

				g_players[i].hp -= 10;
				//cout << g_myid << "가 상대를 맞췄습니다. 상대방 남은 체력 : " << g_players[i].hp << endl;

				CS_HIT_PACKET* packet = new CS_HIT_PACKET;
				packet->type = CS_HIT;
				packet->hp = g_players[i].hp;
				packet->id = g_players[i].id;
				packet->freeze_bullet = isfreeze;

				networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(CS_HIT_PACKET));

				return true;
			}
		}
	}
	return false;

}

void item_colliCHK() {
	//체력 회복
	for (int i = 0; i < 3; i++)
	{
		if (heart[i].exist)
		{
			if (collision_Chk(heart[i].x - 0.3, heart[i].x + 0.3, heart[i].z - 0.3, heart[i].z + 0.3,
				g_players[g_myid].x - 0.3, g_players[g_myid].x + 0.3, g_players[g_myid].z - 0.3, g_players[g_myid].z + 0.3))
			{
				PlaySound(TEXT("item.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
				//heart[i].exist = false;
				//Player.hp += 30;


				CS_ITEM_PACKET* packet = new CS_ITEM_PACKET;
				packet->type = CS_ITEM;
				packet->item = HEAL;
				packet->num = i;
				packet->exist = false;
				networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(CS_ITEM_PACKET));
				delete packet;
			}
		}
	}

	//속도 증가
	for (int i = 0; i < 3; i++)
	{
		if (speedup[i].exist)
		{
			if (collision_Chk(speedup[i].x - 0.3, speedup[i].x + 0.3, speedup[i].z - 0.3, speedup[i].z + 0.3,
				g_players[g_myid].x - 0.3, g_players[g_myid].x + 0.3, g_players[g_myid].z - 0.3, g_players[g_myid].z + 0.3))
			{
				PlaySound(TEXT("item.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
				//speedup[i].exist = false;
				//speed = 0.7;

				CS_ITEM_PACKET* packet = new CS_ITEM_PACKET;
				packet->type = CS_ITEM;
				packet->item = SPEEDUP;
				packet->num = i;
				packet->exist = false;
				networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(CS_ITEM_PACKET));
				delete packet;
			}
		}
	}

	for (int i = 0; i < 3; i++)
	{
		if (ice[i].exist)
		{
			if (collision_Chk(ice[i].x - 0.3, ice[i].x + 0.3, ice[i].z - 0.3, ice[i].z + 0.3,
				g_players[g_myid].x - 0.3, g_players[g_myid].x + 0.3, g_players[g_myid].z - 0.3, g_players[g_myid].z + 0.3))
			{
				PlaySound(TEXT("item.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
				//ice[i].exist = false;
				//speed = 0.7;

				CS_ITEM_PACKET* packet = new CS_ITEM_PACKET;
				packet->type = CS_ITEM;
				packet->item = FREEZE;
				packet->num = i;
				packet->exist = false;
				networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(CS_ITEM_PACKET));
				delete packet;
			}
		}
	}
}

//==탱크 움직임=============================================================================================

void Mouse(int button, int state, int x, int y)
{

	if (button == GLUT_LEFT_BUTTON)
	{		
		if (state == GLUT_DOWN)
		{
			MH.left_button = true;
		}
		else
		{
			MH.left_button = false;
		}
	}


	//카메라 변경
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		position++;
		position %= 2;
	}

	glutPostRedisplay();
}

//마우스 좌표계 변환 함수
void MouseChange(int x, int y) {

	MH.mouse_x = 2 / (float)width * (float)x - 1;
	MH.mouse_y = -2 / (float)height * (float)y + 1;

} 

void Motion(int xpos, int ypos) {

	MouseChange(xpos, ypos);
	MH.left_button = true;

	if (MH.left_button == true)
	{
		if (MH.first_mouse_pos_flag) {
			MH.first_mouse_pos_x = MH.mouse_x;
			MH.first_mouse_pos_y = MH.mouse_y;

			MH.first_mouse_pos_flag = false;
		}

		MH.delta_mouse_x = MH.mouse_x - MH.first_mouse_pos_x;
		MH.delta_mouse_y = MH.mouse_y - MH.first_mouse_pos_y;

		MH.drgree_x += MH.delta_mouse_x * MH.mouse_sensitivity;
		MH.drgree_y += MH.delta_mouse_y * MH.mouse_sensitivity;

		//if (MH.drgree_y > 89.0) {
		//	MH.drgree_y = 89.0;
		//}
		//if (MH.drgree_y < -89.0) {
		//	MH.drgree_y = -89.0;
		//}
		//
		//if (MH.drgree_x > 89.0f)  MH.drgree_x = 89.0f;
		//if (MH.drgree_x < -89.0f) MH.drgree_x = -89.0f;

		CS_YAW_PACKET* packet = new CS_YAW_PACKET;
		packet->type = CS_YAW;
		packet->yaw = yaw;
		packet->bodyYaw = MH.drgree_x;
		networkmgr.SendPacket(reinterpret_cast<char*>(packet), sizeof(CS_YAW_PACKET));
		delete packet;

		g_players[g_myid].bodyYaw = MH.drgree_x;
		bodyYaw = MH.drgree_x;

	}

	glutPostRedisplay();
}


//==맵 블럭 초기화========================================================================================

void Setup_Block() {


	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 30; j++)
		{
			Block[i][j].b_color.r = 0.2;
			Block[i][j].b_color.g = 0.1;
			Block[i][j].b_color.b = 0.0;


			Block[i][j].x = -29 + j * 2;
			Block[i][j].z = -29 + i * 2;
		}
	}


	// 테두리 벽

	for (int i = 0; i < 30; i++)
	{
		Block[0][i].exist = true;
		Block[i][0].exist = true;

		Block[29][i].exist = true;
		Block[i][29].exist = true;
	}
}

void make_map() {

	for (int i = 0; i < 3; i++)
	{
		//작은 꺽쇄
		Block[8][8 + i].exist = true;
		Block[8 + i][8].exist = true;

		Block[20][8 + i].exist = true;
		Block[20 - i][8].exist = true;


		Block[8][20 - i].exist = true;
		Block[8 + i][20].exist = true;


		Block[20][20 - i].exist = true;
		Block[20 - i][20].exist = true;

	}


	for (int i = 0; i < 2; i++)
	{
		//중앙 사각형
		//Block[13][14 + i].exist = true;
		//Block[16][14 + i].exist = true;
		//
		//Block[14+i][13].exist = true;
		//Block[14+i][16].exist = true;
	}



	for (int i = 0; i < 4; i++)
	{
		//큰 꺽쇄
		Block[4][4 + i].exist = true;
		Block[4 + i][4].exist = true;

		Block[25][4 + i].exist = true;
		Block[25 - i][4].exist = true;


		Block[4][25 - i].exist = true;
		Block[4 + i][25].exist = true;


		Block[25][25 - i].exist = true;
		Block[25 - i][25].exist = true;
	}

}

void get_Block() {


	Setup_Block();
	make_map();
	glBindTexture(GL_TEXTURE_2D, block.texture);
	glBindVertexArray(block.VAO);

	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 30; j++)
		{
			if (Block[i][j].exist)
			{
				glm::mat4 map_block = glm::mat4(1.0);
				map_block = glm::translate(map_block, glm::vec3(Block[i][j].x, Block[i][j].y, Block[i][j].z));
				map_block = glm::scale(map_block, glm::vec3(Block[i][j].x_scale, Block[i][j].y_scale, Block[i][j].z_scale));


				unsigned int block_model = glGetUniformLocation(shaderProgram, "modeltrans");
				glUniformMatrix4fv(block_model, 1, GL_FALSE, glm::value_ptr(map_block));

				unsigned int blockColor = glGetUniformLocation(shaderProgram, "objectColor");
				glUniform3f(blockColor, Block[i][j].b_color.r, Block[i][j].b_color.g, Block[i][j].b_color.b);

				glDrawArrays(GL_TRIANGLES, 0, block.m_vertices.size());
			}


		}
	}


}



//==콜백 함수==========================================================================================

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
{

	glViewport(0, 0, w, h);
}

GLvoid Resize(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);
}

//==쉐이더 함수========================================================================================

void make_vertexShaders() {


	GLchar* vertexSource;

	//버텍스 세이더 읽어와서 객체 만들고 컴파일하기
	vertexSource = filetobuf("vertex.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader); //버텍스 컴파일 


	//에러코드 작성
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}

}


void make_fragmentShaders() {

	GLchar* fragmentSource;

	//프래그먼트 세이더 읽어와서 객체 만들고 컴파일하기
	fragmentSource = filetobuf("fragment.glsl");
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader); //버텍스 컴파일 


	//에러코드 작성
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}

}


void make_shaderProgram() {

	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);


	//프로그램 생성 후 객체 삭제 가능
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	//에러코드 작성
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(shaderProgram, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(shaderProgram, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
	glUseProgram(shaderProgram);

}

//파일 읽기-----------------------
char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading 
	if (!fptr) // Return NULL on failure 
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file 
	length = ftell(fptr); // Find out how many bytes into the file we are 
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator 
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file 
	fread(buf, length, 1, fptr);// Read the contents of the file in to the buffer 
	fclose(fptr); // Close the file 
	buf[length] = 0;
	// Null terminator 
	return buf; // Return the buffer 
}

//==========================================================================================

void convertDeviceXY2OpenglXY(int x, int y, float* xpos, float* ypos)
{
	int w = width;
	int h = height;
	*xpos = (float)((x - (float)w / 2.0) * (float)(1.0 / (float)(w / 2.0)));
	*ypos = -(float)((y - (float)h / 2.0) * (float)(1.0 / (float)(h / 2.0)));
}
void InitTexture(const char* filename, unsigned int* texture)
{

	glGenTextures(1, texture);
	stbi_set_flip_vertically_on_load(true);
	glBindTexture(GL_TEXTURE_2D, *texture);
	// 텍스처 wrapping/filtering 옵션 설정(현재 바인딩된 텍스처 객체에 대해)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// 텍스처 로드 및 생성
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}