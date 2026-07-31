#define _CRT_SECURE_NO_WARNINGS
#include <glad/glad.h>
#include <cglm/cglm.h>
#include <Windows.h>
#include <stdio.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "mesh.h"
#include "expect.h"

static void
mesh_build(mesh_t *mesh,
	vertex_t *vertices,
	GLuint *indices,
	GLsizei vsize,
	GLsizei isize)
{
	// create opengl objects
	glGenVertexArrays(1, &mesh->vao);
	glGenBuffers(1, &mesh->vbo);
	glGenBuffers(1, &mesh->ebo);
	// bind opengl objects
	glBindVertexArray(mesh->vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER,
		vsize,
		vertices,
		GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
		isize, 
		indices,
		GL_STATIC_DRAW);
	// write to aPos
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex_t),
		(void *)offsetof(vertex_t, position));
	glEnableVertexAttribArray(0);
	// write to aNormal
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex_t),
		(void *)offsetof(vertex_t, normal));
	glEnableVertexAttribArray(1);
	// write to aUV
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex_t),
		(void *)offsetof(vertex_t, uv));
	glEnableVertexAttribArray(2);
	// set element count
	mesh->element_count = isize / sizeof(GLuint);
}

BOOL
mesh_load(mesh_t *mesh,
	const char *path)
{
    if (!mesh || !path) {
        return FALSE;
    }

    const struct aiScene *scene = aiImportFile(path, 
        aiProcess_Triangulate |           
        aiProcess_JoinIdenticalVertices | 
        aiProcess_GenSmoothNormals |      
        aiProcess_FlipUVs                 
    );

    if (!scene || !scene->mRootNode) {
        fprintf(stderr, "[Mesh Error] Assimp failed to load '%s': %s\n", path, aiGetErrorString());
        return FALSE;
    }

    unsigned int total_vertices = 0;
    unsigned int total_indices = 0;
    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        const struct aiMesh* ai_mesh = scene->mMeshes[m];
        total_vertices += ai_mesh->mNumVertices;
        total_indices += ai_mesh->mNumFaces * 3; 
    }

    GLsizei vsize = total_vertices * sizeof(vertex_t);
    GLsizei isize = total_indices * sizeof(GLuint);
    vertex_t *vertices = malloc(vsize);
    GLuint *indices = malloc(isize);
    if (!vertices || !indices) {
        fprintf(stderr, "[Mesh Error] Out of memory allocating staging buffers for '%s'\n", path);
        free(vertices);
        free(indices);
        aiReleaseImport(scene);
        return FALSE;
    }

    unsigned int vertex_offset = 0;
    unsigned int index_offset = 0;

    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        const struct aiMesh *ai_mesh = scene->mMeshes[m];

        for (unsigned int v = 0; v < ai_mesh->mNumVertices; ++v) {
            unsigned int current_idx = vertex_offset + v;

            // positions
            vertices[current_idx].position[0] = ai_mesh->mVertices[v].x;
            vertices[current_idx].position[1] = ai_mesh->mVertices[v].y;
            vertices[current_idx].position[2] = ai_mesh->mVertices[v].z;

            // normals
            vertices[current_idx].normal[0] = ai_mesh->mNormals[v].x;
            vertices[current_idx].normal[1] = ai_mesh->mNormals[v].y;
            vertices[current_idx].normal[2] = ai_mesh->mNormals[v].z;

            // uvs
            if (ai_mesh->mTextureCoords[0]) {
                vertices[current_idx].uv[0] = ai_mesh->mTextureCoords[0][v].x;
                vertices[current_idx].uv[1] = ai_mesh->mTextureCoords[0][v].y;
            } else {
                vertices[current_idx].uv[0] = 0.0f;
                vertices[current_idx].uv[1] = 0.0f;
            }
        }

        // parse indices
        for (unsigned int f = 0; f < ai_mesh->mNumFaces; ++f) {
            const struct aiFace *face = &ai_mesh->mFaces[f];

            indices[index_offset++] = (GLuint)face->mIndices[0] + vertex_offset;
            indices[index_offset++] = (GLuint)face->mIndices[1] + vertex_offset;
            indices[index_offset++] = (GLuint)face->mIndices[2] + vertex_offset;
        }

        vertex_offset += ai_mesh->mNumVertices;
    }

    mesh_build(mesh, vertices, indices, vsize, isize);

    free(vertices);
    free(indices);
    aiReleaseImport(scene);

    return TRUE;
}

void
mesh_draw(mesh_t *mesh)
{
	glBindVertexArray(mesh->vao);
	glDrawElements(GL_TRIANGLES, mesh->element_count, GL_UNSIGNED_INT, NULL);
}

void 
mesh_destroy(mesh_t *mesh)
{
	glDeleteVertexArrays(1, &mesh->vao);
	glDeleteBuffers(1, &mesh->vbo);
	glDeleteBuffers(1, &mesh->ebo);
}