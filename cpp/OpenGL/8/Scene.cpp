#include "Scene.h"

#include "bmp.h"

Scene::Scene() :
	sphere{Tesselation::genSphere({0,0,0}, 0.4f, 10, 10)},
	vbBallPos{GL_ARRAY_BUFFER},
	vbBallNorm{GL_ARRAY_BUFFER},
	vbBallTan{GL_ARRAY_BUFFER},
	vbBallTc{GL_ARRAY_BUFFER},
	ibBall{GL_ELEMENT_ARRAY_BUFFER},
	ballArray{},
	ballAlbedo{GL_LINEAR, GL_LINEAR},
	ballNormalMap{GL_LINEAR, GL_LINEAR},
	
	square{Tesselation::genRectangle({0,0,0},4,4)},
	vbWallPos{GL_ARRAY_BUFFER},
	vbWallNorm{GL_ARRAY_BUFFER},
	vbWallTan{GL_ARRAY_BUFFER},
	vbWallTc{GL_ARRAY_BUFFER},
	ibWall{GL_ELEMENT_ARRAY_BUFFER},
	wallArray{},
	brickWallAlbedo{GL_LINEAR, GL_LINEAR},
	brickWallNormalMap{GL_LINEAR, GL_LINEAR},
	
	floorAlbedo{GL_LINEAR, GL_LINEAR},
	floorNormalMap{GL_LINEAR, GL_LINEAR},
	
	ceilingAlbedo{GL_LINEAR, GL_LINEAR},
	ceilingNormalMap{GL_LINEAR, GL_LINEAR},
	
	progNormalMap{GLProgram::createFromFile("normalMapVertex.glsl", "normalMapFragment.glsl")},
	mvpLocationNormalMap{progNormalMap.getUniformLocation("MVP")},
	mLocationNormalMap{progNormalMap.getUniformLocation("M")},
	mitLocationNormalMap{progNormalMap.getUniformLocation("Mit")},
	invVLocationNormalMap{progNormalMap.getUniformLocation("invV")},
	lpLocationNormalMap{progNormalMap.getUniformLocation("vLightPos")},
	texRescaleLocationNormalMap{progNormalMap.getUniformLocation("texRescale")},
	texLocationNormalMap{progNormalMap.getUniformLocation("textureSampler")}, 
	normMapLocationNormalMap{progNormalMap.getUniformLocation("normalSampler")},  
	
	fresnelBall{nullptr},
	particleSystem{5000, Vec3(0.0f,0.0f,0.0f), 0.2f, 0.1f, {0,0,0}, Vec3{-1.9f,-1.9f,-1.9f}, Vec3{1.9,1.9,1.9}, 1, 100},
	
	showFresnelFrame{false}

{
	// generate ball resources and load textures
	vbBallPos.setData(sphere.getVertices(),3);
	vbBallNorm.setData(sphere.getNormals(),3);
	vbBallTan.setData(sphere.getTangents(),3);
	vbBallTc.setData(sphere.getTexCoords(),2);	
	ibBall.setData(sphere.getIndices());
	BMP::Image ballAlbedoImage{BMP::load("ballAlbedo.bmp")};
	ballAlbedo.setData(ballAlbedoImage.data, ballAlbedoImage.width, ballAlbedoImage.height, ballAlbedoImage.componentCount);
	BMP::Image ballNormalImage{BMP::load("ballNormal.bmp")};
	ballNormalMap.setData(ballNormalImage.data, ballNormalImage.width, ballNormalImage.height, ballNormalImage.componentCount);

	fresnelBall = std::make_unique<FresnelVisualizer>(vbBallPos,vbBallNorm,vbBallTan,sphere.getVertices().size()/3);

	
	// generate wall geoemtry (for all 5 walls)	
	vbWallPos.setData(square.getVertices(),3);
	vbWallNorm.setData(square.getNormals(),3);
	vbWallTan.setData(square.getTangents(),3);
	vbWallTc.setData(square.getTexCoords(),2);
	ibWall.setData(square.getIndices());
	
	// load brick wall textures (sides
	BMP::Image brickWallAlbedoImage{BMP::load("brickWallAlbedo.bmp")};
	brickWallAlbedo.setData(brickWallAlbedoImage.data,brickWallAlbedoImage.width, brickWallAlbedoImage.height, brickWallAlbedoImage.componentCount);
	BMP::Image brickWallNormalImage{BMP::load("brickWallNormal.bmp")};
	brickWallNormalMap.setData(brickWallNormalImage.data,brickWallNormalImage.width, brickWallNormalImage.height, brickWallNormalImage.componentCount);

	// load brick wall textures (floor)
	BMP::Image floorAlbedoImage{BMP::load("floorAlbedo.bmp")};
	floorAlbedo.setData(floorAlbedoImage.data,floorAlbedoImage.width, floorAlbedoImage.height, floorAlbedoImage.componentCount);
	BMP::Image floorNormalImage{BMP::load("floorNormal.bmp")};
	floorNormalMap.setData(floorNormalImage.data,floorNormalImage.width, floorNormalImage.height, floorNormalImage.componentCount);

	// load brick wall textures (ceiling)
	BMP::Image ceilingAlbedoImage{BMP::load("ceilingAlbedo.bmp")};
	ceilingAlbedo.setData(ceilingAlbedoImage.data, ceilingAlbedoImage.width, ceilingAlbedoImage.height, ceilingAlbedoImage.componentCount);
	BMP::Image ceilingNormalImage{BMP::load("ceilingNormal.bmp")};
	ceilingNormalMap.setData(ceilingNormalImage.data, ceilingNormalImage.width, ceilingNormalImage.height, ceilingNormalImage.componentCount);
	
	ballArray.bind();
	ballArray.connectVertexAttrib(vbBallPos,progNormalMap,"vPos",3);
	ballArray.connectVertexAttrib(vbBallNorm,progNormalMap,"vNorm",3);
	ballArray.connectVertexAttrib(vbBallTan,progNormalMap,"vTan",3);
	ballArray.connectVertexAttrib(vbBallTc,progNormalMap,"vTc",2);
	ballArray.connectIndexBuffer(ibBall);
	
	wallArray.bind();
	wallArray.connectVertexAttrib(vbWallPos,progNormalMap,"vPos",3);
	wallArray.connectVertexAttrib(vbWallNorm,progNormalMap,"vNorm",3);
	wallArray.connectVertexAttrib(vbWallTan,progNormalMap,"vTan",3);
	wallArray.connectVertexAttrib(vbWallTc,progNormalMap,"vTc",2);
	wallArray.connectIndexBuffer(ibWall);
}

void Scene::render(float t0, const Mat4& v, const Mat4& p, const Dimensions& dim) {
	// animate lightpos
	progNormalMap.enable();
	Vec3 lightPos{Mat4::rotationY(t0*55)*Vec3{0,0,1}};
	progNormalMap.setUniform(lpLocationNormalMap, lightPos);

	// ************* the ball
	
	// setup texures
	progNormalMap.setTexture(normMapLocationNormalMap,ballNormalMap,0);
	progNormalMap.setTexture(texLocationNormalMap,ballAlbedo,1);

	// bind geometry
	ballArray.bind();
	
	// setup transformations
	const Mat4 mBall{Mat4::translation({0.0f,0.0f,0.8f})*Mat4::rotationX(t0*157)*Mat4::translation({0.8f,0.0f,0.0f})*Mat4::rotationY(t0*47)};
	progNormalMap.setUniform(texRescaleLocationNormalMap, 1.0f);
	progNormalMap.setUniform(mvpLocationNormalMap, {mBall*v*p});
	progNormalMap.setUniform(mLocationNormalMap, mBall);
	progNormalMap.setUniform(mitLocationNormalMap, Mat4::inverse(mBall), true);
	progNormalMap.setUniform(invVLocationNormalMap, Mat4::inverse(v));
		
	// render geometry
	glDrawElements(GL_TRIANGLES, sphere.getIndices().size(), GL_UNSIGNED_INT, (void*)0);        

	// ************* the left wall
	
	// setup texures (shader is already active)
	progNormalMap.setTexture(normMapLocationNormalMap,brickWallNormalMap,0);
	progNormalMap.setTexture(texLocationNormalMap,brickWallAlbedo,1);

	// bind geometry
	wallArray.bind();

	const Mat4 mLeftWall{Mat4::rotationY(90)*Mat4::translation(-2.0f,0.0f,0.0f)};
	progNormalMap.setUniform(mvpLocationNormalMap, {mLeftWall*v*p});
	progNormalMap.setUniform(mLocationNormalMap, mLeftWall);
	progNormalMap.setUniform(mitLocationNormalMap, Mat4::inverse(mLeftWall), true);

	// render geometry
	glDrawElements(GL_TRIANGLES, square.getIndices().size(), GL_UNSIGNED_INT, (void*)0);

	// ************* the right wall
	
	const Mat4 mRightWall{Mat4::rotationY(-90)*Mat4::translation(2.0f,0.0f,0.0f)};
	progNormalMap.setUniform(mvpLocationNormalMap, mRightWall*v*p);
	progNormalMap.setUniform(mLocationNormalMap, mRightWall);
	progNormalMap.setUniform(mitLocationNormalMap, Mat4::inverse(mRightWall), true);

	// render geometry
	glDrawElements(GL_TRIANGLES, square.getIndices().size(), GL_UNSIGNED_INT, (void*)0);

	// ************* the top wall
	
	const Mat4 mTopWall{Mat4::rotationX(90)*Mat4::translation(0.0f,2.0f,0.0f)};
	progNormalMap.setUniform(mvpLocationNormalMap, mTopWall*v*p);
	progNormalMap.setUniform(mLocationNormalMap, mTopWall);
	progNormalMap.setUniform(mitLocationNormalMap, Mat4::inverse(mTopWall), true);

	progNormalMap.setTexture(normMapLocationNormalMap,ceilingNormalMap,0);
	progNormalMap.setTexture(texLocationNormalMap,ceilingAlbedo,1);

	// render geometry
	glDrawElements(GL_TRIANGLES, square.getIndices().size(), GL_UNSIGNED_INT, (void*)0);

	// ************* the bottom wall
	
	const Mat4 mBottomWall{Mat4::rotationX(-90)*Mat4::translation(0.0f,-2.0f,0.0f)};
	progNormalMap.setUniform(mvpLocationNormalMap, {mBottomWall*v*p});
	progNormalMap.setUniform(mLocationNormalMap, mBottomWall);
	progNormalMap.setUniform(mitLocationNormalMap, Mat4::inverse(mBottomWall), true);
	
	progNormalMap.setTexture(normMapLocationNormalMap,floorNormalMap,0);
	progNormalMap.setTexture(texLocationNormalMap,floorAlbedo,1);

	// render geometry
	glDrawElements(GL_TRIANGLES, square.getIndices().size(), GL_UNSIGNED_INT, (void*)0);

	// ************* the back wall
	
	const Mat4 mBackWall{Mat4::translation(0.0f,0.0f,-2.0f)};
	progNormalMap.setUniform(mvpLocationNormalMap, {mBackWall*v*p});
	progNormalMap.setUniform(mLocationNormalMap, mBackWall);
	progNormalMap.setUniform(mitLocationNormalMap, Mat4::inverse(mBackWall), true);

	// render geometry
	glDrawElements(GL_TRIANGLES, square.getIndices().size(), GL_UNSIGNED_INT, (void*)0);

	// ************* particles

	particleSystem.setSize(dim.height/30);
	particleSystem.setCenter(mBall * Vec3(0.0f,0.0f,0.0f));

	particleSystem.render(v,p);
	particleSystem.update(t0);

	// ************* particles
	if (showFresnelFrame) fresnelBall->render(mBall*v,p);	
}