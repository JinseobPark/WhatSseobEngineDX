#include "stdafx.h"
#include "ObjectFactory.h"
#include "ObjectClass.h"
#include <time.h>

ObjectFactory* OBJECT_FACTORY = nullptr;
ObjectFactory::ObjectFactory()
{
	assert(OBJECT_FACTORY == nullptr && "Factory already created! Only can create one.");
	OBJECT_FACTORY = this;
	last_objectID = 1; // There is over 4 billion numbers
}

ObjectFactory::~ObjectFactory()
{
	DestroyAllObjects();
}

void ObjectFactory::Update(float deltaTime)
{
    //Delete all objects in the ObjectToBeDeleted list
    for (auto object : ObjectsToBeDeleted)
    {
        if (object == nullptr)
            continue;

        auto delete_objID = ObjectIDMap.find(object->m_objectID);

        //assert(delete_objID != ObjectIDMap.end() && "Object is bad memory.");
        if (delete_objID != ObjectIDMap.end())
        {
            //delete and remove from the map
            delete object;
            ObjectIDMap.erase(delete_objID);
        }
    }
    ObjectsToBeDeleted.clear();
}

void ObjectFactory::Destroy(Object* gameObject)
{
    ObjectsToBeDeleted.push_back(gameObject);
}

void ObjectFactory::IDGameObject(Object* gameObject)
{
    ++last_objectID;
    gameObject->m_objectID = last_objectID;

    //Store the game object in the ObjectIDMap
    ObjectIDMap[last_objectID] = gameObject;
}

Object* ObjectFactory::CreateEmptyObject()
{
    Object* gameObject = new Object();
    IDGameObject(gameObject);

    return gameObject;
}

Object* ObjectFactory::CreateEmptyObject(std::string name)
{
    Object* gameObject = new Object();
    IDGameObject(gameObject);
    gameObject->SetName(name);

    return gameObject;
}

Object* ObjectFactory::CreateCamera()
{
    Object* gameObject = new Object();
    IDGameObject(gameObject);
    Transform* transform = new Transform();
    gameObject->AddComponent(transform);
    Camera* cm = new Camera();
    gameObject->AddComponent(cm);


    gameObject->SetName("Camera");

    gameObject->Initialize();
    return gameObject;
}

Object* ObjectFactory::CreateCube(std::string name)
{
    Object* gameObject = new Object();
    IDGameObject(gameObject);
    Transform* transform = new Transform();
    gameObject->AddComponent(transform);
    Model* mod = new Model();
    mod->LoadModel("data/cube.txt");
    gameObject->AddComponent(mod);
    gameObject->SetName(name);
    gameObject->Initialize();
    mod->AddTextures(L"data/dirt01.dds", DEFAULT);

    return gameObject;
}

Object* ObjectFactory::CreateCubeInstance(std::string name)
{
    Object* gameObject = new Object();
    IDGameObject(gameObject);
    Transform* transform = new Transform();
    transform->setPosition(XMFLOAT3(0, 0, 0));
    gameObject->AddComponent(transform);
    ModelInstance* model = new ModelInstance();
    model->LoadModel("data/cube.txt");
    gameObject->AddComponent(model);


    gameObject->SetName(name);
    gameObject->Initialize();
    model->AddTextures(L"data/metal001.dds", DEFAULT);

    return gameObject;
}

Object* ObjectFactory::CreatePlane(std::string name)
{
    Object* gameObject = new Object();
    IDGameObject(gameObject);
    Transform* transform = new Transform();
    transform->setScale(5, 5, 1);
    gameObject->AddComponent(transform);
    Model* mod = new Model();
    mod->LoadModel("data/square.txt");
    gameObject->AddComponent(mod);
    gameObject->SetName(name);
    gameObject->Initialize();
    mod->AddTextures(L"data/dirt01.dds", DEFAULT);

    return gameObject;
}

Object* ObjectFactory::CreatePlaneInstance(std::string name)
{
    Object* gameObject = new Object();
    IDGameObject(gameObject);
    Transform* transform = new Transform();
    transform->setPosition(XMFLOAT3(0, 0, 0));
    gameObject->AddComponent(transform);
    ModelInstance* model = new ModelInstance();
    model->LoadModel("data/square.txt");
    gameObject->AddComponent(model);

    gameObject->SetName(name);
    gameObject->Initialize();
    model->AddTextures(L"data/metal001.dds", DEFAULT);

    return gameObject;
}

Object* ObjectFactory::CreateBillboard(std::string name)
{
    Object* gameObject = new Object();
    IDGameObject(gameObject);
    Transform* transform = new Transform();
    gameObject->AddComponent(transform);
    ModelBillboard* mod = new ModelBillboard();
    mod->LoadModel("data/square.txt");
    gameObject->AddComponent(mod);
    gameObject->SetName(name);
    gameObject->Initialize();
    mod->AddTextures(L"data/ground01.dds", DEFAULT);

    return gameObject;
}

Object* ObjectFactory::CreateTessellation(std::string name)
{
    Object* gameObject = new Object();
    IDGameObject(gameObject);
    Transform* transform = new Transform();
    gameObject->AddComponent(transform);
    ModelTessellation* mod = new ModelTessellation();
    mod->LoadModel("data/square.txt");
    gameObject->AddComponent(mod);
    gameObject->SetName(name);
    gameObject->Initialize();
    mod->AddTextures(L"data/ground01.dds", DEFAULT);

    return gameObject;
}

Object* ObjectFactory::CreateSphere(std::string name)
{
    Object* gameObject = new Object();
    IDGameObject(gameObject);
    Transform* transform = new Transform();
    gameObject->AddComponent(transform);
    Model* mod = new Model();
    mod->LoadModel("data/sphere.txt");
    gameObject->AddComponent(mod);
    gameObject->SetName(name);
    gameObject->Initialize();
    mod->AddTextures(L"data/dirt01.dds", DEFAULT);

    return gameObject;
}

Object* ObjectFactory::CreateSphereInstance(std::string name)
{
    Object* gameObject = new Object();
    IDGameObject(gameObject);
    Transform* transform = new Transform();
    transform->setPosition(XMFLOAT3(0, 0, 0));
    transform->setScale(XMFLOAT3(0.3f, 0.3f, 0.3f));
    gameObject->AddComponent(transform);
    ModelInstance* model = new ModelInstance();
    model->LoadModel("data/sphere.txt");
    gameObject->AddComponent(model);


    gameObject->SetName(name);
    gameObject->Initialize();
    model->AddTextures(L"data/metal001.dds", DEFAULT);

    return gameObject;
}

void ObjectFactory::DestroyAllObjects()
{
    for (auto iter : ObjectIDMap)
        delete iter.second;

    ObjectIDMap.clear();
    last_objectID = 1;
}

Object* ObjectFactory::FindObject(const std::string& name) const
{
    for (const auto obj : ObjectIDMap)
    {
        if (obj.second->p_name == name)
            return obj.second;
    }
    return nullptr;
}

Object* ObjectFactory::FindObjectID(const ObjectID id) const
{
    for (const auto obj : ObjectIDMap)
    {
        if (obj.second->GetObjectID() == id)
            return obj.second;
    }
    return nullptr;
}
