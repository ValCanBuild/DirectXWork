#include "Grid.h"


Grid::Grid(void)
{
}


Grid::~Grid(void)
{
	Shutdown();
	
}

bool Grid::GenerateGridFromTGA(char* filename){
	TerrainLoader	*terrainLoader = new TerrainLoader();

	if (!terrainLoader->LoadTerrain(filename)){
		return false;
	}

	int width = terrainLoader->GetWidth();
	int depth = terrainLoader->GetDepth();

	gridWidth = width;
	gridDepth = depth;

	mVertexCount = (width*depth);
	vertices = new VertexNT[mVertexCount];

	float dx = CELLSPACING;
	float halfWidth = (width-1)*dx*0.5f;
	float halfDepth = (depth-1)*dx*0.5f;

	for(DWORD i = 0; i < width; ++i){
		float z = halfDepth - i*dx;

		for(DWORD j = 0; j < depth; ++j){
			float x = -halfWidth + j*dx;
			// Graph of this function looks like a mountain range.
			float y = terrainLoader->GetHeight(i,j)*HEIGHT_FACTOR;

			vertices[i*depth+j].pos = Vector3f(x, y, z);
			if (j%8 == 0)
				vertices[i*depth+j].texC = Vector2f(0.0f,0.0f);
			else if (j%8 == 1)
				vertices[i*depth+j].texC = Vector2f(1.0f,0.0f);
			else if (j%8 == 2)
				vertices[i*depth+j].texC = Vector2f(0.0f,1.0f);
			else if (j%8 == 3)
				vertices[i*depth+j].texC = Vector2f(0.0f,1.0f);

			else if (j%8 == 4)
				vertices[i*depth+j].texC = Vector2f(1.0f,0.0f);
			else if (j%8 == 5)
				vertices[i*depth+j].texC = Vector2f(1.0f,1.0f);
			else if (j%8 == 6)
				vertices[i*depth+j].texC = Vector2f(0.0f,0.0f);
			else if (j%8 == 7)
				vertices[i*depth+j].texC = Vector2f(1.0f,0.0f);
		}
	}

	ComputeNormals();
	
	// Iterate over each quad and compute indices.
	mIndexCount = ((width-1)*(depth-1)*6);
	indices = new DWORD[mIndexCount];
	int k = 0;
	for(DWORD i = 0; i < width-1; ++i){
		for(DWORD j = 0; j < depth-1; ++j){
			indices[k] = i*depth+j;
			indices[k+1] = i*depth+j+1;
			indices[k+2] = (i+1)*depth+j;

			indices[k+3] = (i+1)*depth+j;
			indices[k+4] = i*depth+j+1;
			indices[k+5] = (i+1)*depth+j+1;
			k += 6; // next quad
		}
	}

	//initialize the buffers with the index and vertex data
	if (!InitializeBuffers(indices, vertices))
		return false;

	delete terrainLoader;
	terrainLoader = 0;
	return true;
}

///Generate a grid by an arbitrary function GetHeight
bool Grid::GenerateGrid(int width, int depth){
/*
	mVertexCount = (width*depth);
	vertices = new VertexC[mVertexCount];

	float dx = CELLSPACING;
	float halfWidth = (width-1)*dx*0.5f;
	float halfDepth = (depth-1)*dx*0.5f;

	for(DWORD i = 0; i < width; ++i){
		float z = halfDepth - i*dx;

		for(DWORD j = 0; j < depth; ++j){
			float x = -halfWidth + j*dx;
			// Graph of this function looks like a mountain range.
			float y = GetHeight(x,z);
			//float y = 0.0f;
			vertices[i*depth+j].pos = D3DXVECTOR3(x, y, z);
			// Color the vertex based on its height.
			if( y < -10.0f )
				vertices[i*depth+j].color = BEACH_SAND;
			else if( y < 5.0f )
				vertices[i*depth+j].color = LIGHT_YELLOW_GREEN;
			else if( y < 12.0f )
				vertices[i*depth+j].color = DARK_YELLOW_GREEN;
			else if( y < 20.0f )
				vertices[i*depth+j].color = DARKBROWN;
			else
				vertices[i*depth+j].color = WHITE;
		}
	}
	
	// Iterate over each quad and compute indices.
	mIndexCount = ((width-1)*(depth-1)*6);
	indices = new DWORD[mIndexCount];
	int k = 0;
	for(DWORD i = 0; i < width-1; ++i){
		for(DWORD j = 0; j < depth-1; ++j){
			indices[k] = i*depth+j;
			indices[k+1] = i*depth+j+1;
			indices[k+2] = (i+1)*depth+j;

			indices[k+3] = (i+1)*depth+j;
			indices[k+4] = i*depth+j+1;
			indices[k+5] = (i+1)*depth+j+1;
			k += 6; // next quad
		}
	}

	//initialize the buffers with the index and vertex data
	if (!InitializeBuffers(indices, vertices))
		return false;*/

	return true;
}

float Grid::GetHeight(float x, float z)const{
	return 0.3f*( z*sinf(0.1f*x) + x*cosf(0.1f*z) );
}

//compute the vertex normals
void Grid::ComputeNormals(){

	for (int i = 0; i < gridWidth; i++){
		for (int j = 0; j < gridDepth; j++){
			Vector3f v1,v2,v3,v4,v12,v23,v34,v41,v;
			v1 = v2 = v3 = v4 = v12 = v23 = v34 = v41 = v = Vector3f(0.0f,0.0f,0.0f);
			//grab 2 vectors for this
			if (j != gridWidth - 1)
			{
				v1 = Vector3f(vertices[i*gridDepth+j+1].pos-vertices[i*gridDepth+j].pos);
			}
			if (i != gridWidth - 1)
			{
				v2 = Vector3f(vertices[(i+1)*gridDepth+j].pos-vertices[i*gridDepth+j].pos);
			}
			if (j > 0)
			{
				v3 = Vector3f(vertices[i*gridDepth+j-1].pos-vertices[i*gridDepth+j].pos);
			}
			if (i > 0)
			{
				v4 = Vector3f(vertices[(i-1)*gridDepth+j].pos-vertices[i*gridDepth+j].pos);
			}

			D3DXVec3Cross(&v12,&v1,&v2);
			D3DXVec3Normalize(&v12,&v12);

			D3DXVec3Cross(&v23,&v2,&v3);
			D3DXVec3Normalize(&v23,&v23);
			
			D3DXVec3Cross(&v34,&v3,&v4);
			D3DXVec3Normalize(&v34,&v34);
			
			D3DXVec3Cross(&v41,&v4,&v1);
			D3DXVec3Normalize(&v41,&v41);

			
			if (D3DXVec3Length(&v12) > 0.0f)
				v = v + v12;

			if (D3DXVec3Length(&v23) > 0.0f)
				v = v + v23;
			
			if (D3DXVec3Length(&v34) > 0.0f)
				v = v + v34;

			if (D3DXVec3Length(&v41) > 0.0f)
				v = v + v41;

			D3DXVec3Normalize(&v,&v);

			vertices[i*gridDepth+j].normal = v;			
		}
	}
}

//The InitializeBuffers function is where we handle creating the vertex and index buffers. 
bool Grid::InitializeBuffers(DWORD* indices,  VertexNT* vertices){

	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexNT) * mVertexCount;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;

	// Now finally create the vertex buffer.
	result = md3dDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &mVB);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * mIndexCount;
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;

	// Create the index buffer.
	result = md3dDevice->CreateBuffer(&indexBufferDesc, &indexData, &mIB);

	if(FAILED(result)){
		return false;
	}

	stride = sizeof(VertexNT);

	return true;
	
}