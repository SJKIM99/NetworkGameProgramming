#pragma once

#include "protocol.h"

#define HP 100

#define X_1 -15
#define Z_1 25

#define X_2 15
#define Z_2 -25

#define SPEED 0.3
#define BULLET_CNT 10

struct SocketInfo {
	SOCKET client_socket;
	short id;
};

class Session {
private:
	SocketInfo* m_socketinfo;

	char* m_name;
	float m_x, m_y, m_z;	//��ũ ��ǥ
	short m_hp;		//��ũ ü��
	float m_speed;	//��ũ �ӵ�	
	bool m_online;	// ���� ���� ����
	short m_bullet_cnt;	//��ũ �Ѿ� ��
	bool m_accept_player;	// ���� ���� ����
	bool m_ready_player=false;	// 
	float m_yaw;
	float m_bodyYaw;
	bool m_collision; // ��ũ�浹����
	bool m_wallCollision; // ��ũ-�� �浹����
	
public:

	Session();
	~Session();

	void SetSocket(SOCKET socket);
	void SetId(short id);
	void SetName(char* name);
	void SetX(float x);
	void SetY(float y);
	void SetZ(float y);
	void SetHp(short hp);
	void SetSpeed(float speed);
	void SetOnline(bool online);
	void setBulletCnt(short bullet_cnt);
	void SetAcceptPlayer(bool accept_player);
	void SetReadyPlayer(bool ready_player);
	void setYaw(float yaw);
	void setBodyYaw(float bodyYaw);
	void SetCollision(bool collid) { m_collision = collid; }
	void SetWallCollision(bool collid) { m_wallCollision = collid; }

	SOCKET GetSocket() const;
	SocketInfo* GetSocketInfo() const;
	short GetId() const;
	char* GetName() const;
	float GetX() const;
	float GetY() const;
	float GetZ() const;
	short GetHp() const;
	float GetSpeed() const;
	bool GetOnline() const;
	short GetBulletCnt() const;
	bool GetAcceptPlayer() const;
	bool GetReadyPlayer() const;
	float GetYaw() const;
	float GetBodyYaw() const;
	bool GetCollision() const { return m_collision; }
	bool GetWallCollision() const { return m_wallCollision; }
};