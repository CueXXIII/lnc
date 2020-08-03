#include "GLTexture2D.h"


GLTexture2D::GLTexture2D(GLint magFilter, GLint minFilter, GLint wrapX, GLint wrapY) :
	id(0),
	internalformat(0),
	format(0),
	type(0),
    magFilter(magFilter),
    minFilter(minFilter),
    wrapX(wrapX),
    wrapY(wrapY),
    width(0),
    height(0),
    componentCount(0)
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapX);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
}

GLTexture2D::~GLTexture2D() {
	glDeleteTextures(1, &id);
}

GLTexture2D::GLTexture2D(const GLTexture2D& other) :
    GLTexture2D(other.magFilter, other.minFilter, other.wrapX, other.wrapY)
{
    setData(other.data, other.height, other.width, other.componentCount);
}

GLTexture2D& GLTexture2D::operator=(GLTexture2D other) {
    magFilter = other.magFilter;
    minFilter = other.minFilter;
    wrapX = other.wrapX;
    wrapY = other.wrapY;
    
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapX);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapY);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    
    setData(other.data, other.height, other.width, other.componentCount);
    return *this;
}

GLint GLTexture2D::getId() const {
	return id;
}

void GLTexture2D::setData(const std::vector<GLubyte>& data, uint32_t width, uint32_t height, uint32_t componentCount) {
	if (data.size() != componentCount*width*height) {
		throw GLException{"Data size and texure dimensions do not match."};
	}
	
    this->data = data;
    this->width = width;
    this->height = height;
    this->componentCount = componentCount;

    
	glBindTexture(GL_TEXTURE_2D, id);

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
	
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, GLuint(width), GLuint(height), 0, format, type, (GLvoid*)data.data());
}
