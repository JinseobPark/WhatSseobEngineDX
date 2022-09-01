#pragma once
#include <string>
#include "Component/ComponentHeaders.h"

class Object
{
	typedef unsigned int ObjectID;
public :
	friend class ObjectFactory;
	Object();
	~Object();

	void Initialize();

	void Destroy(Object* obj);

	bool AddComponent(Component* pComponent);
	Component* GetComponent(ComponentType cType);

	//Entity
	ObjectID GetObjectID()
	{
		return m_objectID;
	}
	void SetObjID(ObjectID m_objID)
	{
		m_objectID = m_objID;
	}

	std::string GetName() const
	{
		return p_name;
	}
	void SetName(const std::string& name)
	{
		p_name = name;
	}


	Transform* m_transform;
private:
	//Sprite* m_sprite;
	Model* m_model;
	ModelInstance* m_modelInstance;
	ModelBillboard* m_modelBillboard;
	ModelTessellation* m_modelTessellation;

	DirLight* m_dirLight;
	PointLight* m_pointLight;
	SpotLight* m_spotLight;

	Camera* m_camera;


	ObjectID m_objectID;
	std::string p_name;
};