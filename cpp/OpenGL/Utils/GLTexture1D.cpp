#include <iostream>

#include "GLTexture1D.h"


GLTexture1D::GLTexture1D(GLint magFilter, GLint minFilter, GLint wrapX) :
	id(0),
	internalformat(0),
	format(0),
	type(0),
    magFilter(magFilter),
    minFilter(minFilter),
    wrapX(wrapX),
    size(0),
    componentCount(0)
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_1D, id);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, wrapX);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, minFilter);
}

GLTexture1D::GLTexture1D(const GLTexture1D& other) :
    GLTexture1D(other.magFilter, other.minFilter, other.wrapX)
{
    setData(other.data, other.size, other.componentCount);
}

GLTexture1D& GLTexture1D::operator=(GLTexture1D other) {
    magFilter = other.magFilter;
    minFilter = other.minFilter;
    wrapX = other.wrapX;
    
    glBindTexture(GL_TEXTURE_1D, id);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, wrapX);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, minFilter);
    
    setData(other.data, other.size, other.componentCount);
    return *this;
}

GLTexture1D::~GLTexture1D() {
	glDeleteTextures(1, &id);
}


GLint GLTexture1D::getId() const {
	return id;
}

void GLTexture1D::setData(const std::vector<GLubyte>& data, uint32_t size, uint32_t componentCount) {
	if (data.size() != componentCount*size) {
		throw GLException{"Data size and texure dimensions do not match."};
	}
	
    this->data = data;
    this->size = size;
    this->componentCount = componentCount;
    
	glBindTexture(GL_TEXTURE_1D, id);

	glPixelStorei(GL_PACK_ALIGNMENT ,1);
	glPixelStorei(GL_UNPACK_ALIGNMENT ,1);
	
	type = GL_UNSIGNED_BYTE;	
	switch (componentCount) {
		case 1 : 
			internalformat = GL_R8;
			format = GL_R;
			break;
		case 2 : 
			internalformat = GL_RG8;
			format = GL_RG;
			break;
		case 3 : 
			internalformat = GL_RGB8;
			format = GL_RGB;
			break;
		case 4 : 
			internalformat = GL_RGBA8;
			format = GL_RGBA;
			break;
	} 
	
	glTexImage1D(GL_TEXTURE_1D, 0, internalformat, GLuint(size), 0, format, type, (GLvoid*)data.data());
}
