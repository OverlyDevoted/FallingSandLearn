#pragma once
#include "IFallingSand.h"
#include <glad/glad.h>

#include "Shader.h"
class ParallelFallingSand : public IFallingSand
{
public:
	ParallelFallingSand();
	~ParallelFallingSand();
	void InitializeSpace(const unsigned int& size, const bool& random) override;
	void IterateSpace() override;
	unsigned int GetCellCount() const override;
	void RenderSpace(const glm::mat4& view, const glm::mat4& projection) override;
private:
	void virtual DeallocateSpace() override;
	
	unsigned int* posSSBO;
	unsigned int* swapSSBO;
	unsigned int* vao;
	Shader* renderShader;
	Shader* determineShader;
	Shader*  swapShader;

};
