#ifdef WINDOWS
#include "HQWaterRenderer.h"

#include "ShaderProgram.h"

#include "SceneManager.h"
#include "Scene.h"

#include "Camera.h"
#include "GameObject.h"

#include "VBO.h"
#include "IBO.h"

#include <fftw\fftw3.h>			  

#include "RenderSystem.h"

#include "Application.h"
#include "GameTime.h"
#include "VertexStructures.h"
#include "Utils.h"
#include "Deps/glad/glad.h"

#include <thread>

const std::string HQWaterRenderer::m_vCubeTextures[6] = {
	"Assets/Engine/Textures/Sky/right.jpg",
	"Assets/Engine/Textures/Sky/left.jpg",
	"Assets/Engine/Textures/Sky/top.jpg",
	"Assets/Engine/Textures/Sky/bottom.jpg",
	"Assets/Engine/Textures/Sky/back.jpg",
	"Assets/Engine/Textures/Sky/front.jpg"
};

HQWaterRenderer::HQWaterRenderer(GameObject* a_pGameObject)
	: Renderer(
		a_pGameObject,
		Application::GetInstance()->GetLoadInterface()->LoadShader(
			"Assets/Engine/Shaders/Water/water_high.vsh",
			"Assets/Engine/Shaders/Water/water_high.fsh"
		)
	)
{
	/* Water Renderer */
	m_pGameObject = a_pGameObject;
	m_pTexture = Application::GetInstance()->GetLoadInterface()->LoadCubeTexture(m_vCubeTextures);

	/* Set DUDV Offset Texture */
	m_pCamera = Application::GetInstance()->GetSceneManager()->GetActiveScene()->GetCamera();

	m_pQuad = Application::GetInstance()->GetLoadInterface()->LoadShape(Shape::E_NONE, WATER_MESH_RESOLUTION);

	GLfloat vertices[] = {
        // Positions          // Normals           // Texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

	m_pQuad->GetVBO()->Bind();
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	VBO::Unbind();
	
	SetType(E_RenderWater);

	/* HQ Wave */
	N = WATER_MESH_RESOLUTION;
	M = WATER_MESH_RESOLUTION;
	omega_hat = WATER_WIND_DIRECTION;
	V = WATER_WIND_SPEED;
	L_x = WATER_UNIT_SIZE;
	L_z = WATER_UNIT_SIZE;
	A = 3e-7f * 2;
	lambda = 1;

	kNum = N * M;

	heightField.push_back(new glm::vec3[kNum]);
	heightField.push_back(new glm::vec3[kNum]);
	normalField.push_back(new glm::vec3[kNum]);
	normalField.push_back(new glm::vec3[kNum]);

	value_h_twiddle_0 = new std::complex<float>[kNum];
	value_h_twiddle_0_conj = new std::complex<float>[kNum];
	value_h_twiddle = new std::complex<float>[kNum];

	// Initialize value_h_twiddle_0 and value_h_twiddle_0_conj in Eq26
	for (int n = 0; n < N; ++n) {
		for (int m = 0; m < M; ++m)
		{
			int index = m * N + n;
			glm::vec2 k = K_VEC(n, m);
			value_h_twiddle_0[index] = func_h_twiddle_0(k);
			value_h_twiddle_0_conj[index] = conj(func_h_twiddle_0(k));
		}
	}

	// Generate IBO
	GLuint indexSize = (N - 1) * (M - 1) * 6;
	GLuint *indices = new GLuint[indexSize];

	int p = 0;

	for (int j = 0; j < N - 1; j++)
	for (int i = 0; i < M - 1; i++)
	{
		indices[p++] = i + j * N;
		indices[p++] = (i + 1) + j * N;
		indices[p++] = i + (j + 1) * N;

		indices[p++] = (i + 1) + j * N;
		indices[p++] = (i + 1) + (j + 1) * N;
		indices[p++] = i + (j + 1) * N;
	}

	m_pQuad->GetIBO()->Load(indexSize, sizeof(GLuint), indices, 1);
	IBO::Unbind();

	delete[] indices;

	std::thread t1(&HQWaterRenderer::Build, this, std::ref(m_bThreadEnded));

	t1.detach();
}

HQWaterRenderer::~HQWaterRenderer()
{
	/* Exit seperate thread */
	m_bExitThread = true;
	while (!m_bThreadEnded) {};

	Application::GetInstance()->GetLoadInterface()->DeleteShape(m_pQuad->GetType(), m_pQuad->GetSegmentCount());

	m_pShader = nullptr;

	delete[] heightField[0];
	delete[] heightField[1];
	delete[] normalField[0];
	delete[] normalField[1];

	delete[] value_h_twiddle_0;
	delete[] value_h_twiddle;
	delete[] value_h_twiddle_0_conj;
}

// Eq14
inline float HQWaterRenderer::func_omega(float k) const
{
	return sqrt(g*k);
}

// Eq23 Phillips spectrum 
inline float HQWaterRenderer::func_P_h(glm::vec2 vec_k) const
{
	if (vec_k == glm::vec2(0.0f, 0.0f))
		return 0.0f;

	float L = V * V / g; // Largest possible waves arising from a continuous wind of speed V

	float k = glm::length(vec_k);
	glm::vec2 k_hat = normalize(vec_k);

	float dot_k_hat_omega_hat = glm::dot(k_hat, omega_hat);
	float result = A * exp(-1 / (k*L*k*L)) / pow(k, 4) * pow(dot_k_hat_omega_hat, 2);

	result *= exp(-k * k*l*l);	// Eq24

	return result;
}

// Eq25
inline std::complex<float> HQWaterRenderer::func_h_twiddle_0(glm::vec2 vec_k)
{
	float xi_r = normal_dist(generator);
	float xi_i = normal_dist(generator);
	return sqrtf(0.5f) * std::complex<float>(xi_r, xi_i) * sqrtf(func_P_h(vec_k));
}

// Eq26
inline std::complex<float> HQWaterRenderer::func_h_twiddle(int kn, int km, float t) const
{
	int index = km * N + kn;
	float k = glm::length(K_VEC(kn, km));
	std::complex<float> term1 = value_h_twiddle_0[index] * exp(std::complex<float>(0.0f, func_omega(k)*t));
	std::complex<float> term2 = value_h_twiddle_0_conj[index] * exp(std::complex<float>(0.0f, -func_omega(k)*t));
	return term1 + term2;
}

//Eq19
void HQWaterRenderer::buildField(float time)
{
	fftwf_complex *in_height, *in_slope_x, *in_slope_z, *in_D_x, *in_D_z;
	fftwf_complex *out_height, *out_slope_x, *out_slope_z, *out_D_x, *out_D_z;

	fftwf_plan p_height, p_slope_x, p_slope_z, p_D_x, p_D_z;

	// Eq20 ikh_twiddle
	std::complex<float>* slope_x_term = new std::complex<float>[kNum];
	std::complex<float>* slope_z_term = new std::complex<float>[kNum];

	// Eq29 
	std::complex<float>* D_x_term = new std::complex<float>[kNum];
	std::complex<float>* D_z_term = new std::complex<float>[kNum];

	for (int n = 0; n < N; n++)
		for (int m = 0; m < M; m++)
		{
			int index = m * N + n;

			value_h_twiddle[index] = func_h_twiddle(n, m, time);

			glm::vec2 kVec = K_VEC(n, m);
			float kLength = glm::length(kVec);
			glm::vec2 kVecNormalized = kLength == 0 ? kVec : normalize(kVec);

			slope_x_term[index] = std::complex<float>(0, kVec.x) * value_h_twiddle[index];
			slope_z_term[index] = std::complex<float>(0, kVec.y) * value_h_twiddle[index];
			D_x_term[index] = std::complex<float>(0, -kVecNormalized.x) * value_h_twiddle[index];
			D_z_term[index] = std::complex<float>(0, -kVecNormalized.y) * value_h_twiddle[index];
		}

	// Prepare fft input and output
	in_height = (fftwf_complex*)value_h_twiddle;
	in_slope_x = (fftwf_complex*)slope_x_term;
	in_slope_z = (fftwf_complex*)slope_z_term;
	in_D_x = (fftwf_complex*)D_x_term;
	in_D_z = (fftwf_complex*)D_z_term;

	out_height = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * kNum);
	out_slope_x = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * kNum);
	out_slope_z = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * kNum);
	out_D_x = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * kNum);
	out_D_z = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * kNum);

	p_height = fftwf_plan_dft_2d(N, M, in_height, out_height, FFTW_BACKWARD, FFTW_ESTIMATE);
	p_slope_x = fftwf_plan_dft_2d(N, M, in_slope_x, out_slope_x, FFTW_BACKWARD, FFTW_ESTIMATE);
	p_slope_z = fftwf_plan_dft_2d(N, M, in_slope_z, out_slope_z, FFTW_BACKWARD, FFTW_ESTIMATE);
	p_D_x = fftwf_plan_dft_2d(N, M, in_D_x, out_D_x, FFTW_BACKWARD, FFTW_ESTIMATE);
	p_D_z = fftwf_plan_dft_2d(N, M, in_D_z, out_D_z, FFTW_BACKWARD, FFTW_ESTIMATE);

	fftwf_execute(p_height);
	fftwf_execute(p_slope_x);
	fftwf_execute(p_slope_z);
	fftwf_execute(p_D_x);
	fftwf_execute(p_D_z);
	
	for (int n = 0; n < N; n++)
		for (int m = 0; m < M; m++)
		{
			int index = m * N + n;
			float sign = 1;

			// Flip the sign
			if ((m + n) % 2) sign = -1;

			normalField[1][index] = glm::normalize(glm::vec3(
				sign * out_slope_x[index][0],
				-1,
				sign * out_slope_z[index][0]));

			heightField[1][index] = glm::vec3(
				(n - N / 2) * L_x / N - sign * lambda * out_D_x[index][0],
				sign * out_height[index][0],
				(m - M / 2) * L_z / M - sign * lambda * out_D_z[index][0]);
			
			if (!m_bInitialized)
			{
				normalField[0][index] = normalField[1][index];
				heightField[0][index] = heightField[1][index];
			}
		}

	m_bInitialized = true;

	fftwf_destroy_plan(p_height);
	fftwf_destroy_plan(p_slope_x);
	fftwf_destroy_plan(p_slope_z);
	fftwf_destroy_plan(p_D_x);
	fftwf_destroy_plan(p_D_z);

	// Free
	delete[] slope_x_term;
	delete[] slope_z_term;
	delete[] D_x_term;
	delete[] D_z_term;

	fftwf_free(out_height);
	fftwf_free(out_slope_x);
	fftwf_free(out_slope_z);
	fftwf_free(out_D_x);
	fftwf_free(out_D_z);
}

bool HQWaterRenderer::Render(Camera* a_pCamera)
{
	if (m_fOpacity == 0.0f)
		return false;

	glDisable(GL_CULL_FACE);
	m_pShader->Activate();

	const int fieldArraySize = sizeof(glm::vec3) * N * M;

#ifdef RELEASE
	for (int i = 0; i < N * M; ++i) {
		heightField[0][i] = glm::lerp(heightField[0][i], heightField[1][i], 0.05f);
		normalField[0][i] = glm::lerp(normalField[0][i], normalField[1][i], 0.05f);
	}
#endif
	
	glBufferData(GL_ARRAY_BUFFER, fieldArraySize * 2, NULL, GL_STATIC_DRAW);

	// Copy height and normal to buffer
	
#ifdef RELEASE
	glBufferSubData(GL_ARRAY_BUFFER, 0, fieldArraySize, heightField[0]);
	glBufferSubData(GL_ARRAY_BUFFER, fieldArraySize, fieldArraySize, normalField[0]);	
#else
	glBufferSubData(GL_ARRAY_BUFFER, 0, fieldArraySize, heightField[1]);
	glBufferSubData(GL_ARRAY_BUFFER, fieldArraySize, fieldArraySize, normalField[1]);	
#endif

	m_bUpdated = false;

	/* Enable Attributes */
	auto it = m_pShader->GetAttributes().find("a_position");
	glVertexAttribPointer(it->second.ID, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(it->second.ID);

	auto it2 = m_pShader->GetAttributes().find("a_normal");
	glVertexAttribPointer(it2->second.ID, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(sizeof(glm::vec3) * N * M));
	glEnableVertexAttribArray(it2->second.ID);

	/* Enable Uniforms */
	for (const auto& pUniforms : m_pShader->GetUniforms())
	{
		if (pUniforms.first == "u_projection")
		{
		}
		else if (pUniforms.first == "u_view")
		{
		}
		else if (pUniforms.first == "u_model")
		{
			const glm::mat4 m4Matrix = m_pGameObject->GetTransform()->GetTransformMatrix();
			glUniformMatrix4fv(pUniforms.second.ID, 1, GL_FALSE, &m4Matrix[0][0]);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_normal")
		{
			const glm::mat3 m3Matrix = glm::transpose(glm::inverse(m_pGameObject->GetTransform()->GetTransformMatrix()));
			glUniformMatrix3fv(pUniforms.second.ID, 1, GL_FALSE, &m3Matrix[0][0]);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_pv")
		{
		}
		else if (pUniforms.first == "u_mvp")
		{
			const glm::mat4 m4Matrix = m_pCamera->GetProjectionViewMatrix() * m_pGameObject->GetTransform()->GetTransformMatrix();
			glUniformMatrix4fv(pUniforms.second.ID, 1, GL_FALSE, &m4Matrix[0][0]);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_camPos")
		{
			const glm::vec3& v3CamPos = m_pCamera->GetTransform()->GetWorldPosition();
			glUniform3fv(pUniforms.second.ID, 1, &v3CamPos[0]);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_opacity")
		{
			glUniform1f(pUniforms.second.ID, m_fOpacity);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_lightPosition")
		{
			glUniform3fv(pUniforms.second.ID, 1, &RenderSystem::LightObject->GetTransform()->GetWorldPosition()[0]);
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_viewPos")
		{
			glUniform3fv(pUniforms.second.ID, 1, &a_pCamera->GetTransform()->GetWorldPosition()[0]);	
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_heightMin")
		{
			glUniform1f(pUniforms.second.ID, heightMin);	
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "u_heightMax")
		{
			glUniform1f(pUniforms.second.ID, heightMax);	
			GL_GET_ERROR();
		}
		else if (pUniforms.first == "s_skyTexture")
		{
			if (m_pTexture)
			{
				m_pTexture->Bind(0);
				glUniform1i(pUniforms.second.ID, 0);
				GL_GET_ERROR();
			}
		}
		else
			Debug::Log("Warning! Missing WaterRenderer uniform: " + pUniforms.first);
	}

	/* Draw */
	glDrawElements(GL_TRIANGLES, m_pQuad->GetIBO()->GetCount(), GL_UNSIGNED_INT, 0);
	GL_GET_ERROR();

	glEnable(GL_CULL_FACE);
	glClearColor(0.0096078431, 0.205882353, 0.205882353, 1.0);

	return true;
}

VBO* HQWaterRenderer::GetVBO() const
{
	return m_pQuad->GetVBO();
}

IBO* HQWaterRenderer::GetIBO() const
{
	return m_pQuad->GetIBO();
}

void HQWaterRenderer::Build(std::atomic_bool& a_bEnded)
{
	while (!m_bExitThread) {
		int nVertex = N * M;	
	
		buildField(timer);
		timer += 0.20;

		int p = 0;

		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++)
			{	
				int index = j * N + i;

				if (heightField[1][index].y > heightMax) heightMax = heightField[1][index].y;
				else if (heightField[1][index].y < heightMin) heightMin = heightField[1][index].y;
			}
		}

		m_bUpdated = true;
	}

	a_bEnded = true;
}
#endif //WINDOWS