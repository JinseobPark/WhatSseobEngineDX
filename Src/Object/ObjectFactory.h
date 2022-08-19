#pragma once
#include "Systems/System.h"
#include <map>
#include <vector>
#include <string>

class Object;
using ObjectID = unsigned;

class ObjectFactory : public System
{
public:
    ObjectFactory();
    ~ObjectFactory();

    void Initialize() override {}
    void Update(float deltaTime) override;

    //Add the object to destroy list for delay destruction
    void Destroy(Object* gameObject);

    //ID the object and store it in the object list(map) 
    //Picking ID system is later..
    void IDGameObject(Object* gameObject /*, ObjectID id*/);


    //Empty
    Object* CreateEmptyObject(/*ObjectID id*/);
    Object* CreateEmptyObject(std::string name);


    //Cube
    Object* CreateCube(std::string name);
    Object* CreateCubeInstance(std::string name);
    Object* CreateTessellation(std:: string name);

    //Sphere
    Object* CreateSphere(std::string name);
    Object* CreateSphereInstance(std::string name);

    //Plane
    Object* CreatePlane(std::string name);
    Object* CreatePlaneInstance(std::string name);
    Object* CreateBillboard(std::string name);


    //Camera
    Object* CreateCamera();


    void DestroyAllObjects();

    //Find Object
    Object* FindObject(const std::string& name) const;
    Object* FindObjectID(const ObjectID id) const;

private:
    //Used to incrementally generate unique IDs
    ObjectID last_objectID;

    //Map of Object to their IDs used for safe referencing
    std::map<ObjectID, Object*> ObjectIDMap;
    //List of objects to be deleted
    //To prevent problems of early or multi call of Object's destruction
    std::vector<Object*> ObjectsToBeDeleted;
};

extern ObjectFactory* OBJECT_FACTORY;