#pragma once

struct 
Vertex
{
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec2 texCoord;

  bool operator==(const Vertex& other) const
  {
    return pos == other.pos && color == other.color && texCoord == other.texCoord;
  }
};

struct 
Texture
{
  uint32_t id;
  std::string type;
  std::string path;
};

struct 
Mesh
{
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  std::vector<Texture> textures;
};

struct 
Model
{
  std::vector<Texture> textures_loaded;
  std::vector<Mesh> meshes;
  std::string directory;
};

std::vector<Texture>
loadMaterialTextures(Model* model, aiMaterial* mat, aiTextureType type, const std::string_view& typeName);

Mesh
processMesh(Model* model, aiMesh* mesh, const aiScene* scene);

void
processNode(Model* model, aiNode* node, const aiScene* scene);

void
LoadModel(Model* model, const std::string_view& filepath);
