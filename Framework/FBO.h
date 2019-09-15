#pragma once

#include "GLIncludes.h"
#include "Texture.h"

class FBO
{
public:
	FBO(GLuint a_uiWidth = 256, GLuint a_uiHeight = 256)
	{
		/* Setup Framebuffer */
		glGenFramebuffers(1, &m_uiIdentity);
		glBindFramebuffer(GL_FRAMEBUFFER, m_uiIdentity);
		GL_GET_ERROR();

		/* Setup Texture */
		m_pTexture = new Texture(a_uiWidth, a_uiHeight, true, true, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pTexture->GetId(), 0);
		GL_GET_ERROR();

		/* Setup Renderbuffer */
		glGenRenderbuffers(1, &m_uiRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_uiRenderBuffer);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB565, a_uiWidth, a_uiHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_uiRenderBuffer);
		GL_GET_ERROR();

		/* Setup Depthbuffer */
		if (m_bHasDepthbuffer)
		{
			glGenRenderbuffers(1, &m_uiDepthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, m_uiDepthBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, a_uiWidth, a_uiHeight);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_uiDepthBuffer);
		}

		/* Check FBO Status */
		GLenum eStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (eStatus != GL_FRAMEBUFFER_COMPLETE) {
			printf("Problem with OpenGL framebuffer: n%xn", eStatus);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			delete this;
		}

		glBindRenderbuffer(GL_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	virtual ~FBO()
	{
		glDeleteRenderbuffers(1, &m_uiRenderBuffer);
		glDeleteRenderbuffers(1, &m_uiDepthBuffer);

		glDeleteBuffers(1, &m_uiIdentity);

		delete m_pTexture;
	}

	GLuint GetID() const
	{
		return m_uiIdentity;
	}

	Texture* GetTexture() const
	{
		return m_pTexture;
	}

	void Bind() const
	{
		glEnable(GL_DEPTH_TEST);

		glBindFramebuffer(GL_FRAMEBUFFER, m_uiIdentity);

		glViewport(0, 0, m_pTexture->GetWidth(), m_pTexture->GetHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

private:
	GLuint m_uiIdentity;
	GLuint m_uiRenderBuffer;
	GLuint m_uiDepthBuffer;

	bool m_bHasDepthbuffer = false;

	Texture* m_pTexture;
};
