#include "UnitTestPCH.h"

#include <assimp/scene.h>
#include <RemoveVCProcess.h>
#include <MaterialSystem.h>


using namespace std;
using namespace Assimp;

class RemoveVCProcessTest : public ::testing::Test
{
public:

	virtual void SetUp();
	virtual void TearDown();

protected:

	RemoveVCProcess* piProcess;
	aiScene* pScene;
};

// ------------------------------------------------------------------------------------------------
void RemoveVCProcessTest::SetUp()
{
	// construct the process
	piProcess = new RemoveVCProcess();
	pScene = new aiScene();

	// fill the scene ..
	pScene->mMeshes = new aiMesh*[pScene->mNumMeshes = 2];
	pScene->mMeshes[0] = new aiMesh();
	pScene->mMeshes[1] = new aiMesh();

	pScene->mMeshes[0]->mNumVertices = 120;
	pScene->mMeshes[0]->mVertices = new aiVector3D[120];
	pScene->mMeshes[0]->mNormals = new aiVector3D[120];
	pScene->mMeshes[0]->mTextureCoords[0] = new aiVector3D[120];
	pScene->mMeshes[0]->mTextureCoords[1] = new aiVector3D[120];
	pScene->mMeshes[0]->mTextureCoords[2] = new aiVector3D[120];
	pScene->mMeshes[0]->mTextureCoords[3] = new aiVector3D[120];

	pScene->mMeshes[1]->mNumVertices = 120;
	pScene->mMeshes[1]->mVertices = new aiVector3D[120];

	pScene->mAnimations    = new aiAnimation*[pScene->mNumAnimations = 2];
	pScene->mAnimations[0] = new aiAnimation();
	pScene->mAnimations[1] = new aiAnimation();

	pScene->mTextures = new aiTexture*[pScene->mNumTextures = 2];
	pScene->mTextures[0] = new aiTexture();
	pScene->mTextures[1] = new aiTexture();

	pScene->mMaterials    = new aiMaterial*[pScene->mNumMaterials = 2];
	pScene->mMaterials[0] = new aiMaterial();
	pScene->mMaterials[1] = new aiMaterial();

	pScene->mLights    = new aiLight*[pScene->mNumLights = 2];
	pScene->mLights[0] = new aiLight();
	pScene->mLights[1] = new aiLight();

	pScene->mCameras    = new aiCamera*[pScene->mNumCameras = 2];
	pScene->mCameras[0] = new aiCamera();
	pScene->mCameras[1] = new aiCamera();

	// COMPILE TEST: aiMaterial may no add any extra members,
	// so we don't need a virtual destructor
	char check[sizeof(aiMaterial) == sizeof(aiMaterial) ? 10 : -1];
	check[0] = 0;
	// to remove compiler warning
	EXPECT_TRUE( check );
}

// ------------------------------------------------------------------------------------------------
void RemoveVCProcessTest::TearDown()
{
	delete pScene;
	delete piProcess;
}

// ------------------------------------------------------------------------------------------------
TEST_F(RemoveVCProcessTest, testMeshRemove)
{
	piProcess->SetDeleteFlags(aiComponent_MESHES);
	piProcess->Execute(pScene);

	EXPECT_TRUE(NULL == pScene->mMeshes);
	EXPECT_EQ(0U, pScene->mNumMeshes);
	EXPECT_TRUE(pScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE);
}

// ------------------------------------------------------------------------------------------------
TEST_F(RemoveVCProcessTest, testAnimRemove)
{
	piProcess->SetDeleteFlags(aiComponent_ANIMATIONS);
	piProcess->Execute(pScene);

	EXPECT_TRUE(NULL == pScene->mAnimations);
	EXPECT_EQ(0U, pScene->mNumAnimations);
	EXPECT_EQ(0U, pScene->mFlags);
}

// ------------------------------------------------------------------------------------------------
TEST_F(RemoveVCProcessTest, testMaterialRemove)
{
	piProcess->SetDeleteFlags(aiComponent_MATERIALS);
	piProcess->Execute(pScene);

	// there should be one default material now ...
	EXPECT_TRUE(1 == pScene->mNumMaterials &&
		pScene->mMeshes[0]->mMaterialIndex == 0 &&
		pScene->mMeshes[1]->mMaterialIndex == 0);
	EXPECT_EQ(0U, pScene->mFlags);
}

// ------------------------------------------------------------------------------------------------
TEST_F(RemoveVCProcessTest, testTextureRemove)
{
	piProcess->SetDeleteFlags(aiComponent_TEXTURES);
	piProcess->Execute(pScene);

	EXPECT_TRUE(NULL == pScene->mTextures);
	EXPECT_EQ(0U, pScene->mNumTextures);
	EXPECT_EQ(0U, pScene->mFlags);
}

// ------------------------------------------------------------------------------------------------
TEST_F(RemoveVCProcessTest, testCameraRemove)
{
	piProcess->SetDeleteFlags(aiComponent_CAMERAS);
	piProcess->Execute(pScene);

	EXPECT_TRUE(NULL == pScene->mCameras);
	EXPECT_EQ(0U, pScene->mNumCameras);
	EXPECT_EQ(0U, pScene->mFlags);
}

// ------------------------------------------------------------------------------------------------
TEST_F(RemoveVCProcessTest, testLightRemove)
{
	piProcess->SetDeleteFlags(aiComponent_LIGHTS);
	piProcess->Execute(pScene);

	EXPECT_TRUE(NULL == pScene->mLights);
	EXPECT_EQ(0U, pScene->mNumLights);
	EXPECT_EQ(0U, pScene->mFlags);
}

// ------------------------------------------------------------------------------------------------
TEST_F(RemoveVCProcessTest, testMeshComponentsRemoveA)
{
	piProcess->SetDeleteFlags(aiComponent_TEXCOORDSn(1) | aiComponent_TEXCOORDSn(2) | aiComponent_TEXCOORDSn(3));
	piProcess->Execute(pScene);

	EXPECT_TRUE(pScene->mMeshes[0]->mTextureCoords[0] &&
		!pScene->mMeshes[0]->mTextureCoords[1] &&
		!pScene->mMeshes[0]->mTextureCoords[2] &&
		!pScene->mMeshes[0]->mTextureCoords[3]);
	EXPECT_EQ(0U, pScene->mFlags);
}

// ------------------------------------------------------------------------------------------------
TEST_F(RemoveVCProcessTest, testMeshComponentsRemoveB)
{
	piProcess->SetDeleteFlags(aiComponent_TEXCOORDSn(1) | aiComponent_NORMALS);
	piProcess->Execute(pScene);

	EXPECT_TRUE(pScene->mMeshes[0]->mTextureCoords[0] &&
		pScene->mMeshes[0]->mTextureCoords[1]  &&
		pScene->mMeshes[0]->mTextureCoords[2]  &&     // shift forward ...
		!pScene->mMeshes[0]->mTextureCoords[3] &&
		!pScene->mMeshes[0]->mNormals);
	EXPECT_EQ(0U, pScene->mFlags);
}

// ------------------------------------------------------------------------------------------------
TEST_F(RemoveVCProcessTest, testRemoveEverything)
{
	piProcess->SetDeleteFlags(aiComponent_LIGHTS | aiComponent_ANIMATIONS |
		aiComponent_MATERIALS | aiComponent_MESHES | aiComponent_CAMERAS | aiComponent_TEXTURES);
	piProcess->Execute(pScene);
	EXPECT_EQ(0U, pScene->mNumAnimations);
	EXPECT_EQ(0U, pScene->mNumCameras);
	EXPECT_EQ(0U, pScene->mNumLights);
	EXPECT_EQ(0U, pScene->mNumMeshes);
	EXPECT_EQ(0U, pScene->mNumTextures);
	// Only the default material should remain.
	EXPECT_EQ(1U, pScene->mNumMaterials);
}