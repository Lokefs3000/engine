#include "util_compilemesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <spdlog/spdlog.h>
#define error(...) spdlog::get("PROJECTC")->error(__VA_ARGS__)

#include "fileutil.h"

struct vec3
{
	float x, y, z;
};

struct vec2
{
	float x, y;
};

struct mesh_vertex
{
	vec3 position;
	vec2 uv;
	vec3 normal;
};

struct mesh_data
{
	std::string name;
	std::vector<mesh_vertex> vertices;
	std::vector<uint32_t> indices;
};

#define mesh_id { 'M', 'E', 'S', 'H' }
#define mesh_ver 1

struct mesh_header
{
	char id[4] = mesh_id;
	uint16_t version = mesh_ver;

	uint16_t namelen;

	uint32_t vertices;
	uint32_t indices;
};

void process_node(const aiScene* scene, aiNode* node, std::vector<mesh_data>& meshes);
mesh_data process_mesh(const aiScene* scene, aiNode* root, aiMesh* mesh);

void util_compilemesh::execute(std::vector<std::string> args)
{
	std::string mesh = "";

	for (size_t i = 0; i < args.size(); i++)
	{
		std::string& arg = args[i];
		if (arg == "--mesh")
			mesh = args[++i];
	}

	uint32_t loadFlags = 0;
#ifdef DEBUG
	loadFlags |= aiProcessPreset_TargetRealtime_Fast;
#elif RELEASE
	loadFlags |= aiProcessPreset_TargetRealtime_Quality;
#elif DIST
	loadFlags |= aiProcessPreset_TargetRealtime_MaxQuality;
#endif

	Assimp::Importer importer{};
	const aiScene* scene = importer.ReadFile(mesh, loadFlags);

	if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr)
	{
		error("failed to load mesh: \"{}\", reason provided: {}", mesh, importer.GetErrorString());
		return;
	}

	std::vector<mesh_data> meshes;
	process_node(scene, scene->mRootNode, meshes);

	std::string name = mesh.substr(0, mesh.find_last_of(".") + 1);

	for (mesh_data& mesh : meshes)
	{
		filestream* stream = fileutil::openbin(name + mesh.name + ".mesh");
		if (stream == nullptr)
			continue;

		mesh_header header{};
		header.namelen = mesh.name.size();
		header.vertices = mesh.vertices.size();
		header.indices = mesh.indices.size();
		fileutil::write(stream, (char*)&header, sizeof(header));

		fileutil::write(stream, mesh.name);

		fileutil::write(stream, (char*)mesh.vertices.data(), sizeof(mesh_vertex) * mesh.vertices.size());
		fileutil::write(stream, (char*)mesh.indices.data(), sizeof(uint32_t) * mesh.indices.size());

		fileutil::close(stream);
	}
}

void process_node(const aiScene* scene, aiNode* node, std::vector<mesh_data>& meshes)
{
	for (size_t i = 0; i < node->mNumMeshes; i++)
		meshes.push_back(process_mesh(scene, node, scene->mMeshes[node->mMeshes[i]]));
	for (size_t i = 0; i < node->mNumChildren; i++)
		process_node(scene, node->mChildren[i], meshes);
}

mesh_data process_mesh(const aiScene* scene, aiNode* root, aiMesh* mesh)
{
	mesh_data ret{};

	ret.vertices.resize(mesh->mNumVertices);
	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		mesh_vertex vertex{};

		memcpy(&vertex.position, &mesh->mVertices[i], sizeof(vec3));
		if (mesh->mTextureCoords[0] != nullptr)
			memcpy(&vertex.uv, &mesh->mTextureCoords[0][i], sizeof(vec2));
		else
			memset(&vertex.uv, 0, sizeof(vec2));
		memcpy(&vertex.normal, &mesh->mNormals[i], sizeof(vec3));

		ret.vertices[i] = vertex;
	}

	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace& face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++)
			ret.indices.push_back(face.mIndices[j]);
	}

	ret.name = mesh->mName.data;
	return ret;
}
