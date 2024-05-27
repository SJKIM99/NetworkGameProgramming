#include "Session.h"

Session::Session()
{
	m_socketinfo = new SocketInfo;
	m_x = 0;
	m_z = 0;
	m_hp = 0;
	m_speed = 0;
	m_online = false;
	m_bullet_cnt = 0;
	m_accept_player = false;
	m_ready_player = false;
	m_collision = false;
}

Session::~Session()
{
	delete m_name;
	delete m_socketinfo;
}

void Session::SetSocket(SOCKET socket) { m_socketinfo->client_socket = socket; }


void Session::SetX(float x) { m_x = x; }

void Session::SetY(float y){m_y = y;}

void Session::SetId(short id) { m_socketinfo->id = id; }

void Session::SetZ(float z) { m_z = z; }


void Session::SetHp(short hp) { m_hp = hp; }

void Session::SetSpeed(float speed) { m_speed = speed; }

void Session::SetName(char* name) { 
	m_name = new char[strlen(name) + 1];
	strcpy(m_name, name); 
}

void Session::SetOnline(bool online) { m_online = online; }

void Session::setBulletCnt(short bullet_cnt) { m_bullet_cnt = bullet_cnt; }

void Session::SetAcceptPlayer(bool accept_player) { m_accept_player = accept_player; }

void Session::SetReadyPlayer(bool ready_player) { m_ready_player = ready_player; }

void Session::setYaw(float yaw) { m_yaw = yaw; }

void Session::setBodyYaw(float bodyYaw) { m_bodyYaw = bodyYaw; }


SOCKET Session::GetSocket() const { return m_socketinfo->client_socket; }

float Session::GetX() const{ return m_x; }

float Session::GetY() const { return m_y; }

float Session::GetZ() const{ return m_z; }

short Session::GetHp() const{ return m_hp; }

short Session::GetId() const { return m_socketinfo->id; }

float Session::GetSpeed() const{ return m_speed; }

SocketInfo* Session::GetSocketInfo() const { return m_socketinfo; }

bool Session::GetOnline() const{ return m_online; }


short Session::GetBulletCnt() const{ return m_bullet_cnt; }

bool Session::GetAcceptPlayer() const { return m_accept_player; }

char* Session::GetName() const { return m_name; }

bool Session::GetReadyPlayer() const { return m_ready_player; }

float Session::GetYaw() const { return m_yaw; }

float Session::GetBodyYaw() const {	return m_bodyYaw; }






