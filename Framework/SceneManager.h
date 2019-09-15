#pragma once
#include <vector>
#include <stack>
#include <functional>

class Scene;
class Texture;
class Shape;
class ShaderProgram;

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	///Loads a new scene synchronously and replaces previous scene
	void LoadScene(Scene* a_pScene);

	///Pauses the current scene and adds it to the scene stack
	///New scene loads synchronously and replaces the previous one
	void PushScene(Scene* a_pScene);

	///Deletes the current scene and resumes the scene at the top of the stack
	void PopScene();

	void UpdateScene(float a_deltaTime);
	void DrawScene() const;

	bool SwapScenes();
	bool HasScene() const;

	Scene* GetActiveScene() const { return m_vScenes.top(); }

private:
	bool m_bRunDefferedFunc;
	std::function<void()> m_defferedFunc;
	std::stack<Scene*> m_vScenes;
};
