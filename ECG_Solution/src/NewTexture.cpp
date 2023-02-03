#include "NewTexture.h"



NewTexture::NewTexture(std::string file)
{
	file.erase(0, 1);
	file = "assets/3d" + file;

	glGenTextures(1, &_handle);
	glBindTexture(GL_TEXTURE_2D, _handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char *data = stbi_load(file.c_str(), &width, &height, &nrChan, 0);
	if (data)
	{
		GLenum format;
		if (nrChan == 1)
			format = GL_RED;
		else if (nrChan == 3)
			format = GL_RGB;
		else if (nrChan == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		std::cout << "loaded file: " + file << std::endl;
	}
	else std::cout << "Failed to load: " + file << std::endl;
	stbi_image_free(data);
}


NewTexture::~NewTexture()
{
}

void NewTexture::bind(unsigned int unit)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _handle);
}
