#include "Application.h"

#include "SceneManager.h"
#include "GameTime.h"
#include "VertexStructures.h"
#include "Camera.h"
#include "IBO.h"
#include "VBO.h"

#ifdef RASPBERRY
  #include "bcm_host.h"

  static bool bSIGKeepRunning = true;

  void SIGHandler(int a_iDummy)
  {
    bSIGKeepRunning = false;
  }

  void SignalInit()
  {
    signal(SIGINT, SIGHandler);
    signal(SIGQUIT, SIGHandler);
  }
#elif WINDOWS
#include "Shlwapi.h"
#include <ctime>
#endif

Application* Application::m_pInstance = nullptr;

Application::Application()
{ 
#ifdef ANDROID
}
void Application::Init(engine* a_pEngine)
{
#endif

	m_pInstance = this;
	m_bExit = false;

#ifdef ANDROID
	m_pGLContext = new GLContext(a_pEngine);
	if (m_bInitialized) return;
#else
	m_pGLContext = new GLContext();
#endif

	m_pLoadInterface = new LoadInterface();
	m_pSceneManager = new SceneManager();

	m_pInput = new InputManager(m_pGLContext->GetGLWindow());
	m_pUIRenderer = new UIRenderer();
	m_pUICursor = new UICursor();
	m_pClient = new NetworkClient(12000, 2);

#ifdef ANDROID  
	m_bInitialized = true;
#endif
}

#ifdef ANDROID  
void Application::ClearContext()
{
	eglMakeCurrent(m_pGLContext->GetDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

	if (m_pGLContext->GetSurface() != EGL_NO_SURFACE) {
        eglDestroySurface(m_pGLContext->GetDisplay(), m_pGLContext->GetSurface());
        m_pGLContext->GetDisplay() = EGL_NO_SURFACE;
    }
}
#endif

void Application::Run()
{
#ifdef RASPBERRY
	SignalInit();
#endif

#ifndef ANDROID
	//Draw splash screen before initial loading
	if (!m_sSplashPath.empty())
		DrawSplashScreen();

	m_pUIRenderer->Init();
	OnCreate();
	GameTime::Init();

	while (!m_bExit & m_pInput->Update())
	{
#endif
	
#ifdef ANDROID
		/* Context can be null if app is unfocussed */
		if (!GetGLContext()) return;

		if (!m_bInitialized)
			return;

		/* Create EGL context if needed */
		m_pGLContext->Init();

		/* Load everything when context and window are initialized */
		if (!m_bActivated) {
			m_pUIRenderer->Init();

			OnCreate();
			GameTime::Init();
      
			m_bActivated = true;
		}

		m_pInput->Update();
#endif
		if (m_pSceneManager->SwapScenes())
			GameTime::Init();

		GameTime::Tick();
		
		m_pClient->Update();
		m_pGLContext->Clear();
		m_pUIRenderer->InitFrame();

		OnUpdate();
		m_pSceneManager->UpdateScene(GameTime::GetDeltaTime());
		m_pUICursor->Update();

		OnDraw();

		m_pSceneManager->DrawScene();
		m_pUICursor->Draw();

		if (GameTime::GetFrameTime() > 1.f)
			GameTime::Reset();
		
#ifdef WINDOWS
		if (m_pInput->KeyPressed(Input::E_F11))
			m_pGLContext->SetFullScreen(!m_pGLContext->IsFullScreen());
#endif

		m_pInput->Sync();

		m_pGLContext->Swap();

#ifndef ANDROID
	}
#endif
}

void Application::DrawSplashScreen() const
{
	Texture* pLoadingTexture = GetLoadInterface()->LoadTexture(m_sSplashPath);
	Shape* pLoadingQuad = GetLoadInterface()->LoadShape(Shape::E_QUAD, 1);
	ShaderProgram* pLoadingShader = GetLoadInterface()->LoadShader(
		"Assets/Engine/Shaders/Default/Unlit2D.vsh",
		"Assets/Engine/Shaders/Default/Unlit2D.fsh"
	);

	/* Make sure nothing else is visible except loading screen */
	GetGLContext()->Clear();

	/* Bind */
	pLoadingShader->Activate();
	pLoadingQuad->GetVBO()->Bind();

	const size_t szSize = VertexData::GetSize(VertexData::E_DEFAULT);
	const std::vector<int> vOffsets = VertexData::GetOffsets(VertexData::E_DEFAULT);

	for (auto& pAttribute : pLoadingShader->GetAttributes())
	{
		glEnableVertexAttribArray(pAttribute.second.ID);

		if (pAttribute.first == "a_position")
			glVertexAttribPointer(pAttribute.second.ID, 3, GL_FLOAT, GL_FALSE, szSize, (GLvoid*)vOffsets[0]);
		else if (pAttribute.first == "a_normal")
			glVertexAttribPointer(pAttribute.second.ID, 3, GL_FLOAT, GL_FALSE, szSize, (GLvoid*)vOffsets[1]);
		else if (pAttribute.first == "a_uv")
			glVertexAttribPointer(pAttribute.second.ID, 2, GL_FLOAT, GL_FALSE, szSize, (GLvoid*)vOffsets[2]);
		else
			Debug::Log("Warning! Missing LoadingScreen attribute: " + pAttribute.first);
	}

	pLoadingQuad->GetIBO()->Bind();

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Calculate matrix */
	const glm::uvec2& v2ScreenSize = m_pGLContext->GetGLWindow()->GetSize();
	const glm::vec3 v3Center = glm::vec3(v2ScreenSize, 0) * 0.5f;
	const unsigned int uiSize = glm::min(v2ScreenSize.x, v2ScreenSize.y) - 50;

	glm::mat4 m4ModelMatrix = glm::translate(v3Center);
	m4ModelMatrix[0][0] = uiSize;
	m4ModelMatrix[1][1] = uiSize;

	/* Draw */
	for (const auto& pUniforms : pLoadingShader->GetUniforms())
	{
		if (pUniforms.first == "u_ortho")
		{
			const glm::mat4 m4Matrix = Camera::GetOrthoMatrix();
			glUniformMatrix4fv(pUniforms.second.ID, 1, GL_FALSE, &m4Matrix[0][0]);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_model")
		{
			glUniformMatrix4fv(pUniforms.second.ID, 1, GL_FALSE, &m4ModelMatrix[0][0]);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_orthoModel")
		{
			const glm::mat4 m4Matrix = Camera::GetOrthoMatrix() * m4ModelMatrix;
			glUniformMatrix4fv(pUniforms.second.ID, 1, GL_FALSE, &m4Matrix[0][0]);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_opacity")
		{
			glUniform1f(pUniforms.second.ID, 1.0f);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_color")
		{
			glm::vec3 v3Color(1.f, 1.f, 1.f);
			glUniform3fv(pUniforms.second.ID, 1, &v3Color[0]);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "s_texture")
		{
			if (pLoadingTexture)
			{
				pLoadingTexture->Bind(0);
				glUniform1i(pUniforms.second.ID, 0);
				GL_GET_ERROR();
			}
		}
		else
			Debug::Log("Warning! Missing LoadingTexture uniform: " + pUniforms.first);
	}

	glDrawElements(GL_TRIANGLES, pLoadingQuad->GetIBO()->GetCount(), GL_UNSIGNED_SHORT, 0);
	GL_GET_ERROR();

	/* Unbind */
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	pLoadingShader->DisableAttributes();
	ShaderProgram::Deactivate();

	if (pLoadingTexture) pLoadingTexture->Unbind();

	VBO::Unbind();
	IBO::Unbind();

	m_pGLContext->Swap();

	GetLoadInterface()->DeleteTexture(pLoadingTexture->GetPath());
	GetLoadInterface()->DeleteShader(pLoadingShader->GetVertShader()->GetPath(), pLoadingShader->GetFragShader()->GetPath());
	GetLoadInterface()->DeleteShape(Shape::E_QUAD, 1);

	GL_GET_ERROR();
}

Application::~Application()
{
	delete m_pSceneManager;

	delete m_pInput;
	delete m_pUIRenderer;
	delete m_pUICursor;
	delete m_pClient;

	delete m_pLoadInterface;
	delete m_pGLContext;
}
