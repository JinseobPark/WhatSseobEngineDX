#pragma once

class System
{
public:
    //Initialize the System

    virtual void Initialize() = 0;
    
    //All Systems are updated every frame
    virtual void Update(float deltaTime) = 0;

    //All Systems need a virtual destructor to their destructors called
    virtual ~System()
    {
        // nothing...
    }
}; //class System
