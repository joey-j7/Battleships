#include "GameTime.h"
#include <cstdio>

#if defined(RASPBERRY) || defined(ANDROID)
timeval GameTime::m_tCurrentTime;
timeval GameTime::m_tPrevTime;
#elif WINDOWS
#include <glfw3.h>

double GameTime::m_tCurrentTime;
double GameTime::m_tPrevTime;
#endif

float GameTime::m_fTime = 0.f;
float GameTime::m_fDeltaTime = 0.f;
float GameTime::m_fPhysicsTimeStep = 0.f;
float GameTime::m_fTimeScale = 1.f;

int GameTime::m_iFrames = 0;
float GameTime::m_fFrameTime = 0;
float GameTime::m_fFPS = 0;

std::string GameTime::m_sTime;

void GameTime::Init()
{
	UpdateTime(m_tPrevTime);
}

void GameTime::Tick()
{
	UpdateTime(m_tCurrentTime);

#if defined(RASPBERRY) || defined(ANDROID)
	const float fDeltaTime = (float)(m_tCurrentTime.tv_sec - m_tPrevTime.tv_sec + (m_tCurrentTime.tv_usec - m_tPrevTime.tv_usec) * 0.000001f);
	m_fDeltaTime = fDeltaTime * m_fTimeScale;
	m_fTime = m_tCurrentTime.tv_sec;
	m_tPrevTime = m_tCurrentTime;

	/* Get time string */
	time_t nowtime;
	struct tm *nowtm;
	char tmbuf[12];

	nowtime = m_tCurrentTime.tv_sec;
	nowtm = localtime(&nowtime);
	strftime(tmbuf, sizeof tmbuf, "%H:%M:%S", nowtm);

	m_sTime = tmbuf;
#elif WINDOWS
	m_fDeltaTime = (m_tCurrentTime - m_tPrevTime) * m_fTimeScale;
	m_fTime = m_tCurrentTime;
	m_tPrevTime = m_tCurrentTime;

	/* Get time string */
	time_t rawtime;
	struct tm timeinfo;
	char buffer[80];

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);
	m_sTime = buffer;
#endif

	m_iFrames++;
	m_fFrameTime += m_fDeltaTime;

	/* Just to be sure */
	if (m_fFrameTime != 0)
		m_fFPS = m_iFrames / m_fFrameTime;
}

void GameTime::Reset()
{
	m_iFrames = 0;
	m_fFrameTime = 0;
}

void GameTime::SetPhysicsTimeStep(float a_fTimeStep)
{
	m_fPhysicsTimeStep = a_fTimeStep;
}

void GameTime::SetTimeScale(float a_fTimeScale)
{
	m_fTimeScale = a_fTimeScale;
	if (m_fTimeScale < 0.f)
		m_fTimeScale = 0.f;
}

#if defined(RASPBERRY) || defined(ANDROID)
void GameTime::UpdateTime(timeval& m_tTime)
{
	gettimeofday(&m_tTime, NULL);
}
#elif WINDOWS
void GameTime::UpdateTime(double& m_dTime)
{
	m_dTime = glfwGetTime();
}
#endif
