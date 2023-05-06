#pragma once
#include "IFallingSand.h"
#include <glad/glad.h>

#include "Shader.h"
class ParallelFallingSand : public IFallingSand
{
public:
	ParallelFallingSand();
	~ParallelFallingSand() override;
	std::pair<int, int> InitializeSpace(const unsigned int& size, const bool& random) override;
	float IterateSpace() override;
	unsigned int GetCellCount() const override;
	void RenderSpace(const glm::mat4& view, const glm::mat4& projection) override;
	void DeallocateSpace() override;

private:
	unsigned int* posSSBO;
	unsigned int* swapSSBO;
	unsigned int* vao;
	Shader* renderShader;
	Shader* determineShader;
	Shader*  swapShader;

};
