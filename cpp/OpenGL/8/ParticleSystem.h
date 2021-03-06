#pragma once

#include <vector>

#include "Vec3.h"
#include "Mat4.h"
#include "GLProgram.h"
#include "GLBuffer.h"
#include "GLArray.h"
#include "GLTexture2D.h"

class Particle {
public:
	Particle(const Vec3& position, const Vec3& direction, const Vec3& acceleration, 
			 const Vec3& color, float opacity, float maxAge, 
			 const Vec3& minPos, const Vec3& maxPos, bool bounce);
	
	void update(float deltaT);
	void restart(const Vec3& position, const Vec3& direction, const Vec3& acceleration, 
				 const Vec3& color, float opacity, float maxAge);
	bool isDead() const {return age >= maxAge;};
	void setBounce(bool bounce);
	void setColor(const Vec3& color) {this->color = color;}
	void setAcceleration(const Vec3& acceleration) {this->acceleration = acceleration;}
	
	std::vector<float> getData() const;
	
private:
	Vec3 position;
	Vec3 direction;
	Vec3 acceleration;
	Vec3 color;
	float opacity;
	bool bounce;
	
	float maxAge;
	float age;
	
	Vec3 minPos;
	Vec3 maxPos;
};

const Vec3 RANDOM_COLOR{-1.0f,-1.0f,-1.0f};
const Vec3 RAINBOW_COLOR{-2.0f,-2.0f,-2.0f};

class ParticleSystem {
public:
	ParticleSystem(	uint32_t particleCount, const Vec3& center, float spreadRadius,
					float initialSpeed, 
					const Vec3& acceleration, const Vec3& minPos, const Vec3& maxPos,
					float maxAge, float pointSize, const Vec3& color=RANDOM_COLOR);

	void render(const Mat4& v, const Mat4& p);
	void update(float t);
	
	void setCenter(const Vec3& center);
	void setSize(float pointSize) {this->pointSize = pointSize;}
	
	void setColor(const Vec3& color);
	
	void setBounce(bool bounce);
	void setAcceleration(const Vec3& acceleration);
	void setMaxAge(float maxAge) {this->maxAge = maxAge;}

private:
	ParticleSystem(const ParticleSystem&);
	
	Vec3 center;
	float spreadRadius;
	float initialSpeed;
	std::vector<Particle> particles;
	
	GLProgram prog;
	GLint mvpLocation;
	GLint posLocation;
	GLint colLocation;
	GLint texLocation;
	
	GLTexture2D sprite;
	
	Vec3 acceleration;
	
	float pointSize;
	Vec3 color;
	float maxAge;
	GLArray particleArray;
	GLBuffer vbPosColor;
	float lastT;
	
	Vec3 computeCenter() const;
	Vec3 computeDirection() const;
	Vec3 computeColor() const;
	
};