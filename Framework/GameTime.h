#pragma once

#ifdef RASPBERRY
#include <sys/time.h>
#elif ANDROID
#include <time.h>
#elif WINDOWS
#include <ctime>
#endif

#include <string>

class GameTime
{
public:
	static void Init();
	static void Tick();
	static void Reset();

	static void SetPhysicsTimeStep(float a_fTimeStep);
	static void SetTimeScale(float a_fTimeScale);

	static float GetPhysicsTimeStep() { return m_fPhysicsTimeStep; }
	static float GetDeltaTime() { return m_fDeltaTime; }
	static float GetTime() { return m_fTime; }
	static float GetTimeScale() { return m_fTimeScale; }
	static const std::string& GetTimeString() { return m_sTime; }

	static int GetFrames() { return m_iFrames; }
	static float GetFrameTime() { return m_fFrameTime; }

	static float GetFPS() { return m_fFPS; }

private:
#if defined(RASPBERRY) || defined(ANDROID)
	static void UpdateTime(timeval& m_tTime);
#elif WINDOWS
  static void UpdateTime(double& m_dTime);
#endif

	static float m_fTimeScale;
	static float m_fTime;
	static float m_fDeltaTime;
	static float m_fPhysicsTimeStep;

	static int m_iFrames;
	static float m_fFrameTime;
	static float m_fFPS;

	static std::string m_sTime;

#if defined(RASPBERRY) || defined(ANDROID)
	static timeval m_tCurrentTime;
	static timeval m_tPrevTime;
#elif WINDOWS
	static double m_tCurrentTime;
	static double m_tPrevTime;
#endif
};
