#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <learnopengl/bone.h>
#include <functional>
#include <learnopengl/animdata.h>
#include <learnopengl/model_animation.h>

// 这个结构体将帮助我们将动画从Assimp提取出来
struct AssimpNodeData
{
	glm::mat4	transformation;
	std::string name;
	int			childrenCount;
	std::vector<AssimpNodeData> children;
};

class Animation
{
public:
	Animation() = default;

	// 模型数据和动画数据是分开的
	Animation(const std::string& animationPath, Model* model)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
		assert(scene && scene->mRootNode);

		// 取第一个动画clip
		auto animation = scene->mAnimations[0];
		m_Duration		 = animation->mDuration;
		m_TicksPerSecond = animation->mTicksPerSecond;// 表示的动画速度

		// 这两行代码什么都没干，可以注释掉
		aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
		globalTransformation = globalTransformation.Inverse();
		// 这两行代码什么都没干，可以注释掉

		ReadHierarchyData(m_RootNode, scene->mRootNode);
		ReadMissingBones(animation, *model);
	}

	~Animation()
	{
	}

	// 根据骨骼名查找骨骼
	Bone* FindBone(const std::string& name)
	{
		auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
			[&](const Bone& Bone)
			{
				return Bone.GetBoneName() == name;
			}
		);

		if (iter == m_Bones.end())
			return nullptr;
		else
			return &(*iter);
	}

	
	inline float GetTicksPerSecond()
	{
		return m_TicksPerSecond;
	}

	inline float GetDuration()
	{
		return m_Duration;
	}

	inline const AssimpNodeData& GetRootNode()
	{
		return m_RootNode;
	}

	inline const std::map<std::string,BoneInfo>& GetBoneIDMap() 
	{ 
		return m_BoneInfoMap;
	}

private:
	// 在Assimp 中，一个 Animation 下面会有很多个 Channel ，每个Channel 的名字都对应着 一个Bone的名字。
	// 每个Channel 影响着 同名的Bone。
	void ReadMissingBones(const aiAnimation* animation, Model& model)
	{
		int size = animation->mNumChannels;//暂时来说，mNumChannels跟总的骨骼数是一样的

		auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
		int&  boneCount   = model.GetBoneCount();  //getting the m_BoneCounter from Model class

		//reading channels(bones engaged in an animation and their keyframes)
		for (int i = 0; i < size; i++)
		{
			aiNodeAnim* channel  = animation->mChannels[i];// 每个node的animation就是单根骨骼的animation
			std::string boneName = channel->mNodeName.data;

			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				boneInfoMap[boneName].id = boneCount;
				boneCount++;
			}

			// 骨骼的ID，name，channel信息都是绑定在一起的
			m_Bones.push_back(Bone(	channel->mNodeName.data,
										boneInfoMap[channel->mNodeName.data].id,
										channel));
		}

		m_BoneInfoMap = boneInfoMap;
	}

	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
	{
		assert(src);

		dest.name			= src->mName.data;
		dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
		dest.childrenCount	= src->mNumChildren;

		for (int i = 0; i < src->mNumChildren; i++)
		{
			AssimpNodeData newData;
			ReadHierarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}


	float m_Duration;
	int   m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
};

