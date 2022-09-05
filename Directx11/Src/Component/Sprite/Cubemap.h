//***************************************************************************************
// Cubemap.h by Frank Luna (C) 2011 All Rights Reserved.
//   
// Simple class that renders a sky using a cube map.
//***************************************************************************************

#ifndef SKY_H
#define SKY_H


class Camera;

class Cubemap
{
public:
	Cubemap(ID3D11Device* device, const WCHAR* cubemapFilename, float skySphereRadius);
	~Cubemap();

	ID3D11ShaderResourceView* CubeMapSRV();

	void Draw(ID3D11DeviceContext* dc, const Camera& camera);

private:
	Cubemap(const Cubemap& rhs);
	Cubemap& operator=(const Cubemap& rhs);

private:
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;

	ID3D11ShaderResourceView* mCubeMapSRV;

	UINT mIndexCount;
};

#endif // SKY_H