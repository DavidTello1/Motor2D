#include "ResourceTexture.h"
#include "ModuleFileSystem.h"

#define STB_IMAGE_IMPLEMENTATION    
#include "stb_image.h"
#include "Glew/include/glew.h"

#include "mmgr/mmgr.h"

bool ResourceTexture::Create(const char* path, const char* name, UID uid)
{
	Add(path, name, LIBRARY_TEXTURE_FOLDER, EXTENSION_TEXTURE, uid); // Initialize Data
	Save(); // Save Library File

	return true;
}

void ResourceTexture::Remove(size_t index)
{
	UnLoad(index);
	data.Remove(index);

	texture.buffer.erase(texture.buffer.begin() + index);
	texture.width.erase(texture.width.begin() + index);
	texture.height.erase(texture.height.begin() + index);
}

bool ResourceTexture::Save() const
{
	//App->file_system->Save(data.files_library.back().c_str(), this, sizeof(ResourceTexture)); //***CHANGE TO STB SAVEFILE

	return true;
}

bool ResourceTexture::Load(size_t index)
{
	stbi_set_flip_vertically_on_load(true);

	int x, y, channels;
	uint8_t* image = stbi_load(data.files_assets[index].c_str(), &x, &y, &channels, STBI_rgb_alpha);
	if (image == nullptr) {
		LOG("Texture with index [%d] could not be loaded", index, 'e');
		return false;
	}

	uint32_t tex = 0;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	//glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(image);

	texture.width[index] = x;
	texture.height[index] = y;
	texture.buffer[index] = tex;

	return true;
}

void ResourceTexture::UnLoad(size_t index)
{
	glDeleteTextures(1, &texture.buffer[index]);

	texture.width[index] = 0;
	texture.height[index] = 0;
	texture.buffer[index] = 0;
}
