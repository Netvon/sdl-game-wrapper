#ifndef SGW_GL_RENDERER_H
#define SGW_GL_RENDERER_H

//#include <glad/glad.h>
//#include <glm/glm.hpp>
//#include "mesh.h"
//
//namespace sgw {
//	template <typename Mesh>
//	void draw_mesh(const Mesh& mesh, GLenum mode) {
//		if constexpr (Mesh::type == sgw::mesh_type::array) {
//			glDrawArrays(mode, 0, mesh.get_vertex_count());
//		}
//		else if constexpr (Mesh::type == sgw::mesh_type::elements) {
//			glDrawElements(mode, mesh.get_index_count(), GL_UNSIGNED_INT, nullptr);
//		}
//	}
//
//	template <typename Mesh>
//	void draw_mesh_instanced(const Mesh& mesh, GLenum mode, std::size_t instance_count) {
//		if constexpr (Mesh::type == sgw::mesh_type::array) {
//			glDrawArraysInstanced(mode, 0, mesh.get_vertex_count(), instance_count);
//		}
//		else if constexpr (Mesh::type == sgw::mesh_type::elements) {
//			glDrawElementsInstanced(mode, mesh.get_index_count(), GL_UNSIGNED_INT, nullptr, instance_count);
//		}
//	}
//
//}




#endif