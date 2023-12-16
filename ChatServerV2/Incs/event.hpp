#ifndef event_HPP
#define event_HPP

#include "data.hpp"

// 메시지를 하나의 유저(client)에게 전송
void SendNotification(t_data* data, int fd, const char* msg);

// 메시지를 모든 유저(client)들에게 전송
void SendMessage(t_data* data, char* arg1, char* arg2, char* msg);

// 유저(client) Fd 에서 이벤트시 호출
void UserEvent(t_data* data, t_userData* userData);

// 서버(Listen) Fd 에서 이벤트시 호출
void NewUeserEvent(t_data* data);

// 현 시간 출력 함수 (UTC)
std::string pTime();

#endif