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
	float m_x, m_y, m_z;	//攀农 谅钎
	short m_hp;		//攀农 眉仿
	float m_speed;	//攀农 加档	
	bool m_online;	// 蜡历 立加 咯何
	short m_bullet_cnt;	//攀农 醚舅 荐
	bool m_accept_player;	// 蜡历 荐侩 咯何
	bool m_ready_player=false;	// 
	float m_yaw;
	float m_bodyYaw;
	bool m_collision; // 攀农面倒咯何
	bool m_wallCollision; // 攀农-寒 面倒咯何
	
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