#include "meshutils.h"
#include <mathutils.h>
#include <glm/gtx/normal.hpp>
#include <triangleiterator.h>

namespace nap
{
	bool isTriangleMesh(const MeshShape& shape)
	{
		switch (shape.getDrawMode())
		{
		case opengl::EDrawMode::LINE_LOOP:
		case opengl::EDrawMode::LINE_STRIP:
		case opengl::EDrawMode::LINES:
		case opengl::EDrawMode::POINTS:
		case opengl::EDrawMode::UNKNOWN:
			return false;
		case opengl::EDrawMode::TRIANGLES:
		case opengl::EDrawMode::TRIANGLE_FAN:
		case opengl::EDrawMode::TRIANGLE_STRIP:
			return true;
		default:
			assert(false);
			return false;
		}
	}
	

	glm::vec3 computeTriangleNormal(const std::array<int, 3>& indices, const VertexAttribute<glm::vec3>& vertices)
	{
		assert(indices[0] < vertices.getCount());
		assert(indices[1] < vertices.getCount());
		assert(indices[2] < vertices.getCount());
		return glm::cross((vertices[indices[0]] - vertices[indices[1]]), (vertices[indices[0]] - vertices[indices[2]]));
	}


	void  setTriangleIndices(MeshShape& mesh, int number, const std::array<int, 3>& indices)
	{
		// Copy triangle index over
		MeshShape::IndexList& mesh_indices = mesh.getIndices();

		switch (mesh.getDrawMode())
		{
		case opengl::EDrawMode::TRIANGLES:
		{
			// Make sure our index is in range
			assert((number * 3) + 2 < mesh_indices.size());

			// Fill the data
			unsigned int* id = mesh_indices.data() + (number * 3);
			*(id + 0) = indices[0];
			*(id + 1) = indices[1];
			*(id + 2) = indices[2];
			break;
		}
		case opengl::EDrawMode::TRIANGLE_FAN:
		{
			assert(number + 2 < mesh_indices.size());
			unsigned int* id = mesh_indices.data();
			*id = indices[0];
			*(id + number + 1) = indices[1];
			*(id + number + 2) = indices[2];
			break;
		}
		case opengl::EDrawMode::TRIANGLE_STRIP:
		{
			assert(number + 2 < mesh_indices.size());
			unsigned int* id = mesh_indices.data() + number;
			*(id + 0) = indices[0];
			*(id + 1) = indices[1];
			*(id + 2) = indices[2];
			break;
		}
		default:
			assert(false);
			break;
		}
	}


	void computeBoundingBox(const MeshInstance& mesh, math::Box& outBox)
	{
		glm::vec3 min = { nap::math::max<float>(), nap::math::max<float>(), nap::math::max<float>() };
		glm::vec3 max = { nap::math::min<float>(), nap::math::min<float>(), nap::math::min<float>() };

		const nap::VertexAttribute<glm::vec3>& positions = mesh.getAttribute<glm::vec3>(VertexAttributeIDs::getPositionName());
		for (const auto& point : positions.getData())
		{
			if (point.x < min.x) { min.x = point.x; }
			if (point.x > max.x) { max.x = point.x; }
			if (point.y < min.y) { min.y = point.y; }
			if (point.y > max.y) { max.y = point.y; }
			if (point.z < min.z) { min.z = point.z; }
			if (point.z > max.z) { max.z = point.z; }
		}
		outBox.mMinCoordinates = min;
		outBox.mMaxCoordinates = max;
	}


	nap::math::Box computeBoundingBox(const MeshInstance& mesh)
	{
		math::Box box;
		computeBoundingBox(mesh, box);
		return box;
	}


	void computeNormals(const MeshInstance& meshInstance, const VertexAttribute<glm::vec3>& positions, VertexAttribute<glm::vec3>& outNormals)
	{
		assert(outNormals.getCount() == positions.getCount());
		
		// Total number of attributes
		int attr_count = positions.getCount();

		// Normal data
		std::vector<glm::vec3>& normal_data = outNormals.getData();
		
		// Reset normal data so we can accumulate data into it. Note that this is a memset
		// instead of a loop to improve performance
		std::memset(normal_data.data(), 0, sizeof(glm::vec3) * normal_data.size());

		// Accumulate all normals into the normals array
		glm::vec3* normal_data_ptr = normal_data.data();

		// Go over all triangles in all triangle shapes
		TriangleIterator iterator(meshInstance);
		while (!iterator.isDone())
		{
			Triangle triangle = iterator.next();

			const TriangleData<glm::vec3>& triangleData = triangle.getVertexData(positions);
			glm::vec3 normal = glm::cross((triangleData.first() - triangleData.second()), (triangleData.first()- triangleData.third()));

			normal_data_ptr[triangle.firstIndex()] += normal;
			normal_data_ptr[triangle.secondIndex()] += normal;
			normal_data_ptr[triangle.thirdIndex()] += normal;
		}

		// Normalize to deal with shared vertices
		for (glm::vec3& normal : normal_data)
			normal = glm::normalize(normal);
	}


	void reverseWindingOrder(MeshInstance& mesh)
	{
		TriangleIterator iterator(mesh);
		while (!iterator.isDone())
		{
			Triangle triangle = iterator.next();
			Triangle::IndexArray indices = triangle.indices();
			std::swap(indices[0], indices[2]);

			int shapeIndex = triangle.getShapeIndex();
			setTriangleIndices(mesh.getShape(shapeIndex), triangle.getTriangleIndex(), indices);
		}
	}


	void generateIndices(nap::MeshShape& shape, int vertexCount, int offset)
	{
		MeshShape::IndexList& indices = shape.getIndices();
		indices.resize(vertexCount);
		for (int vertex = 0; vertex < vertexCount; ++vertex)
			indices[vertex] = vertex + offset;
	}


	void computeConnectivity(const MeshInstance& mesh, MeshConnectivityMap& outConnectivityMap)
	{
		// Resize to number of indices
		outConnectivityMap.resize(mesh.getNumVertices());

		TriangleIterator iterator(mesh);
		while (!iterator.isDone())
		{
			Triangle triangle = iterator.next();
			outConnectivityMap[triangle.firstIndex()].emplace_back(triangle);
			outConnectivityMap[triangle.secondIndex()].emplace_back(triangle);
			outConnectivityMap[triangle.thirdIndex()].emplace_back(triangle);
		}
	}
}
