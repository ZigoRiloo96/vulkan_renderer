
#include "PCH.h"

std::vector<Texture>
loadMaterialTextures(Model* model, aiMaterial* mat, aiTextureType type, const std::string_view& typeName)
{
  std::vector<Texture> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
  {
    aiString str;
    mat->GetTexture(type, i, &str);
    bool skip = false;
    for (unsigned int j = 0; j < model->textures_loaded.size(); j++)
    {
      if (std::strcmp(model->textures_loaded[j].path.data(), str.C_Str()) == 0)
      {
        textures.push_back(model->textures_loaded[j]);
        skip = true;
        break;
      }
    }
    if (!skip)
    {
      Texture texture;
      texture.type = typeName;
      texture.path = str.C_Str();
      textures.push_back(texture);
      model->textures_loaded.push_back(texture);
    }
  }
  return textures;
}

Mesh
processMesh(Model* model, aiMesh* mesh, const aiScene* scene)
{
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++)
  {
    Vertex vertex;
    glm::vec3 vector;
    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.pos = vector;
    if (mesh->mTextureCoords[0])
    {
      glm::vec2 vec;
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.texCoord = vec;
      vertex.color = { 1, 1, 1 };
    }
    else
      vertex.texCoord = glm::vec2(0.0f, 0.0f);

    vertices.push_back(vertex);
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; i++)
  {
    aiFace face = mesh->mFaces[i];

    for (unsigned int j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }

  aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

  std::vector<Texture> diffuseMaps = loadMaterialTextures(model, material, aiTextureType_DIFFUSE, "texture_diffuse");
  textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

  std::vector<Texture> specularMaps = loadMaterialTextures(model, material, aiTextureType_SPECULAR, "texture_specular");
  textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

  std::vector<Texture> normalMaps = loadMaterialTextures(model, material, aiTextureType_HEIGHT, "texture_normal");
  textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

  std::vector<Texture> heightMaps = loadMaterialTextures(model, material, aiTextureType_AMBIENT, "texture_height");
  textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

  return { vertices, indices, textures };
}

void
processNode(Model* model, aiNode* node, const aiScene* scene)
{
  // process all the node's meshes (if any)
  for (unsigned int i = 0; i < node->mNumMeshes; i++)
  {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    model->meshes.push_back(processMesh(model, mesh, scene));
  }
  // then do the same for each of its children
  for (unsigned int i = 0; i < node->mNumChildren; i++)
  {
    processNode(model, node->mChildren[i], scene);
  }
}

void
LoadModel(Model* model, const std::string_view& filepath)
{
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(filepath.data(), aiProcess_Triangulate | aiProcess_FlipUVs);

  assert(scene || !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || scene->mRootNode);

  model->directory = filepath.substr(0, filepath.find_last_of('/'));

  processNode(model, scene->mRootNode, scene);
}