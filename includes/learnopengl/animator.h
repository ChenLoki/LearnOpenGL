#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <learnopengl/animation.h>
#include <learnopengl/bone.h>

class Animator
{
private:
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;

public:
	Animator(Animation* animation)
	{
		m_CurrentTime      = 0.0;
		m_CurrentAnimation = animation;

		m_FinalBoneMatrices.reserve(100);

		for (int i = 0; i < 100; i++)
			m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
	}

	void UpdateAnimation(float dt)
	{
		// GetTicksPerSecond 每秒多少帧
		// m_CurrentTime 当前帧数
		// m_CurrentAnimation->GetDuration() 总共的帧数

		m_DeltaTime = dt;
		if (m_CurrentAnimation)
		{
			// TicksPerSecond 表示动画播放的速度
			// GetTicksPerSecond() * dt  ===  动画总的播放时间 = 速度 * 变化的时间
			// 以 m_TicksPerSecond 的速率推进 m_CurrentTime
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;//
			m_CurrentTime =  fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());// 取余数

			// 从动画的根节点开始更新，递归更新子节点
			CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
	}

	void PlayAnimation(Animation* pAnimation)
	{
		m_CurrentAnimation = pAnimation;
		m_CurrentTime      = 0.0f;
	}

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName      = node->name;
		glm::mat4   nodeTransform = node->transformation;// 如果没有骨骼，则使用默认值初始化骨骼

		Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

		// 如果节点有骨骼，则用节点的骨骼变换初始化nodeTransform
		if (Bone)
		{
			Bone->Update(m_CurrentTime);// 根据时间，插值计算出当前的bone的变换矩阵
			nodeTransform = Bone->GetLocalTransform();//
		}

		// 每级累计叠加变换
		// globalTransformation这个矩阵能把bone上绑定的顶点，直接变换到世界空间
		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;// 将模型空间的点变换到骨骼空间
			m_FinalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}

	std::vector<glm::mat4> GetFinalBoneMatrices()
	{
		return m_FinalBoneMatrices;
	}



};
