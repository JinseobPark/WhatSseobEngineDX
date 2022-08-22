#include "stdafx.h"
#include "GameLogic.h"
#include "Systems/SystemHeaders.h"
#include "Component/ComponentHeaders.h"
#include "Object/ObjectFactory.h"
#include "Object/ObjectClass.h"
#include <time.h>


GameLogic* mGAMELOGIC = nullptr;

GameLogic::GameLogic()
{
	assert(mGAMELOGIC == nullptr && "No more than one instance of Application system!");
	mGAMELOGIC = this;
}

GameLogic::~GameLogic()
{
}

void GameLogic::Initialize()
{
	//for random
	srand(time(NULL));

	//camera
	camera = OBJECT_FACTORY->CreateCamera();

	Object* obj;
	obj = OBJECT_FACTORY->CreateCube("piter");

	Object* obj3;
	obj3 = OBJECT_FACTORY->CreateBillboard("piter3");

	Object* sphere;
	sphere = OBJECT_FACTORY->CreateSphere("sphere1");
	sphere->m_transform->setPosition(XMFLOAT3(1, 1, 3));

	Object* sphere2;
	sphere2 = OBJECT_FACTORY->CreateSphere("sphere2");
	sphere2->m_transform->setPosition(XMFLOAT3(2, 2, 3));


	//Instance Object
	Object* sphereInstance;
	sphereInstance = OBJECT_FACTORY->CreateSphereInstance("piter5");
	{
		float LO = -10.0f;
		float HI = 10.0f;
		float x, y, z;
		ModelInstance* model = (ModelInstance*)sphereInstance->GetComponent(CT_MODEL_INSTANCE);
		// Add childrens only transform
		for (int i = 0; i < 200; i++)
		{
			Transform* transformsub = new Transform();
			x = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
			y = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
			z =  LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
			transformsub->setPosition(XMFLOAT3(x, y, z));
			model->AddTransform(transformsub);
		}
		if (!model->ChangeInstanceNewBuffer()) return;
	}

	//object planes
	Object* plane1;
	plane1 = OBJECT_FACTORY->CreatePlane("Floor");
	plane1->m_transform->setPosition(XMFLOAT3(0, -2, 0));
	plane1->m_transform->setRotation(XMFLOAT3(90, 0, 0));

	Object* plane2;
	plane2 = OBJECT_FACTORY->CreatePlane("Floor2");
	plane2->m_transform->setPosition(XMFLOAT3(5, 0, 0));
	plane2->m_transform->setRotation(XMFLOAT3(0, 90, 0));

	Object* plane3;
	plane3 = OBJECT_FACTORY->CreatePlane("Floor3");
	plane3->m_transform->setPosition(XMFLOAT3(-5, 0, 0));
	plane3->m_transform->setRotation(XMFLOAT3(0, -90, 0));

	//object lights
	Object* light1;
	light1 = OBJECT_FACTORY->CreateEmptyObject("light1");
	{
		DirLight* dirL = new DirLight();
		dirL->m_diffuse = XMFLOAT4(0.0f, 0.5, 0.5, 1.0f);
		dirL->m_direction = XMFLOAT3(0.0f, 0.0f, -1.0f);
		light1->AddComponent(dirL);

		DirLight* dirL2 = new DirLight();
		dirL2->m_diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		dirL2->m_direction = XMFLOAT3(0.5f, -0.5f, 0.0f);
		light1->AddComponent(dirL2);
		light1->Initialize();
	}
	

	Object* light2;
	light2 = OBJECT_FACTORY->CreateEmptyObject("light2");
	{
		PointLight* pointL2 = new PointLight();
		pointL2->m_diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
		light2->AddComponent(pointL2);

		float LO = 0.1f;
		float HI = 0.5f;
		float x, y, z;
		for (int i = 0; i < 2; i++)
		{
			PointLight* pointL2 = new PointLight();

			x = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
			y = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
			z = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
			pointL2->m_diffuse = XMFLOAT4(x, y, z, 1.0f);

			LO = -0.5f;
			HI = 0.5f;
			x = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
			y = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
			z = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
			pointL2->m_position = XMFLOAT3(x, y, z);
			light2->AddComponent(pointL2);
		}

		for (int i = 0; i < 25; i++)
		{
			SpotLight* spotL2 = new SpotLight();
			x = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
			y = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
			z = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
			spotL2->m_diffuse = XMFLOAT4(x, y, z, 1.0f);

			LO = -0.5f;
			HI = 0.5f;
			x = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
			y = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
			z = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
			spotL2->m_direction = XMFLOAT3(x, y, z);
			spotL2->m_position = XMFLOAT3(x, y, z);
			light2->AddComponent(spotL2);
		}
	}
	light2->Initialize();

}

void GameLogic::Update(float dt)
{
	//camera speed
	float cameraSpeed = 0.01f;
	//sound test
	if(mINPUT->IsPressed(DIK_3))
		mSOUND->PlayWaveFile();

	// Camera position move
	//Todo : update by camera UP, FRONT, BACK... 
	if (mINPUT->IsPressed(DIK_W))
		camera->m_transform->position.z += cameraSpeed * dt;
	if (mINPUT->IsPressed(DIK_S))
		camera->m_transform->position.z -= cameraSpeed * dt;
	if (mINPUT->IsPressed(DIK_A))
		camera->m_transform->position.x -= cameraSpeed * dt;
	if (mINPUT->IsPressed(DIK_D))
		camera->m_transform->position.x += cameraSpeed * dt;
	if (mINPUT->IsPressed(DIK_Q))
		camera->m_transform->position.y += cameraSpeed * dt;
	if (mINPUT->IsPressed(DIK_E))
		camera->m_transform->position.y -= cameraSpeed * dt;

	// if hold mouse right button, the camera can rotate moving as unreal engine
	if (mINPUT->IsRightDown())
	{
		Camera* cm = (Camera*)camera->GetComponent(CT_CAMERA);
		cm->Rotation(mINPUT->GetMouseMoveY(), mINPUT->GetMouseMoveX());
	}
	 


}
