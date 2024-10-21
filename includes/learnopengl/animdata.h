#pragma once

#include<glm/glm.hpp>

struct BoneInfo
{
	/*id is index in finalBoneMatrices*/
	int id;

	/*offset matrix transforms vertex from model space to bone space*/
	// mOffsetMatrix 保存的是将Bone 变换到世界空间的矩阵的逆矩阵。
	// 世界坐标系的点 经过这个矩阵的偏移 就变换到了骨骼坐标系中了。骨骼动画是在骨骼坐标系中进行的。
	// 准确的说，offset matrix是将模型空间的点，变换到骨骼空间，然后由骨骼空间的globalMatrix变换到世界空间
	glm::mat4 offset;

};
#pragma once
