#include "Resource.h"

#include "Application.h"
#include "ModuleFileSystem.h"

#define STB_IMAGE_IMPLEMENTATION    
#include "stb_image.h"

#include "mmgr/mmgr.h"

bool ResourceTexture::Create(const char* path, UID uid)
{
	size_t index = data.files_library.size();
	Add(path, LIBRARY_TEXTURES_FOLDER, EXTENSION_TEXTURE, uid); //initialize data

	if (App->file_system->Exists(data.files_library[index].c_str())) //if already loaded don't import
	{
		// ***if last mod date not equal reimport
		LOG("File has already been imported", 'd');

		return true;
	}
	else if (Load(index)) //import & save meta
	{
		SaveMeta(index);
		return true;
	}
	return false;
}

bool ResourceTexture::Remove(size_t index) const
{
	return true;
}

bool ResourceTexture::Import(size_t index) const
{
	return true;
}

void ResourceTexture::SaveMeta(size_t index) const
{
	// Create Config file
	Config config;
	config.AddUID("ID", data.ids[index]);
	config.AddString("File", data.files_assets[index].c_str());

	// Save as .meta file
	char* buffer = nullptr;
	uint size = config.Save(&buffer, "meta file");
	if (size > 0)
	{
		std::string path = data.files_assets[index] + ".meta";
		App->file_system->Save(path.c_str(), buffer, size);
	}
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
