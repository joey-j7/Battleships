#pragma once
#ifdef WINDOWS
				 
#include "Renderer.h"

#include <cmath>
#include <random>
#include <complex>

#include <future>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

class Model;
class Shape;
class Camera;

class VBO;
class IBO;

#define WATER_MESH_RESOLUTION 1000
#define WATER_WIND_SPEED 15
#define WATER_WIND_DIRECTION glm::vec2(1, 1)

#define WATER_UNIT_SIZE 1000

// Get k vector from mesh grid (n,m)
#define K_VEC(n,m) glm::vec2(2 * M_PI * (n - N / 2) / WATER_UNIT_SIZE, 2 * M_PI * (m  - M / 2) / WATER_UNIT_SIZE)

class HQWaterRenderer : public Renderer
{
public:
  HQWaterRenderer(GameObject* a_pGameObject);
  virtual ~HQWaterRenderer();

  bool Render(Camera* a_pCamera) override;

  const glm::vec4& GetColor() const
  {
    return m_v4Color;
  }

  void SetColor(const glm::vec4& a_vColor)
  {
    m_v4Color = a_vColor;
  }

  VBO* GetVBO() const override;
  IBO* GetIBO() const override;

private:
	void buildField(float time);

	inline float func_omega(float k) const;
	inline float func_P_h(glm::vec2 vec_k) const;
	inline std::complex<float> func_h_twiddle_0(glm::vec2 vec_k);
	inline std::complex<float> func_h_twiddle(int kn, int km, float t) const;
	
	void Build(std::atomic_bool& a_bEnded);

	GameObject * m_pGameObject = nullptr;
	
	Camera* m_pCamera = nullptr;
	Shape* m_pQuad = nullptr;

	glm::vec4 m_v4Color;

	/* Texture Paths */
	static const std::string m_vCubeTextures[6];
	
	/* HQ vars */
	std::vector<glm::vec3*> heightField;
	std::vector<glm::vec3*> normalField;
	
	std::complex<float> *value_h_twiddle_0 = NULL;
	std::complex<float> *value_h_twiddle_0_conj = NULL;
	std::complex<float> *value_h_twiddle = NULL;
	
	std::default_random_engine generator;
	std::normal_distribution<float> normal_dist;
	
	const float pi = float(M_PI);
	const float g = 9.8f; // Gravitational constant
	const float l = 0.1f;
	float A;
	float V;
	int N, M;
	int kNum;
	glm::vec2 omega_hat;
	float lambda;
	float L_x, L_z;

	float heightMin = 0;
	float heightMax = 0;

	float timer = 0;

	bool m_bUpdated;
	bool m_bInitialized;

	bool m_bExitThread = false;
	std::atomic_bool m_bThreadEnded = false;
};
#endif // WINDOWS