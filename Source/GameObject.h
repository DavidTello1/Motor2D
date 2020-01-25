#pragma once
#include "Globals.h"

#include "glew\include\GL\glew.h" //for vbo (GLuint)
#include "MathGeoLib/include/MathBuildConfig.h" //for float4x4 & aabb
#include "MathGeoLib/include/MathGeoLib.h" //for float4x4 & aabb
//#include <vector>

class Hierarchy;
//class Component;

class GameObject
{
	friend class ModuleScene;
	//friend class ResourceScene;
	//ALIGN_CLASS_TO_16 //for float4x4 & aabb

private:
	GameObject(const char* name, GameObject* Parent);
	virtual ~GameObject();

public:
	// Name
	void SetName(const char* new_name) { name = new_name; }
	const char* GetName() const { return name.c_str(); }

	// UID
	UID GetUID() const { return uid; }
	void SetUID(UID id) { uid = id; }

	// Active
	bool IsActive() const { return active; }
	void SetActive(bool act) { active = act; }

	// Parent
	bool HasParent() const { return parent != nullptr; }
	GameObject* GetParent() const { return parent; }
	void ChangeParent(GameObject* new_parent) { parent = new_parent; }

	// Childs
	bool HasChilds() const { return !childs.empty(); }
	std::vector<GameObject*> GetChilds() const { return childs; }
	//void ChildAdded();
	//void ChildDeleted();

	//// Components
	//template<typename ComponentType>
	//ComponentType* GetComponent() { return (ComponentType*)GetComponent(ComponentType::GetType()); }

	//bool HasComponent(Component::Type type);
	//Component* GetComponent(Component::Type type);
	//Component* AddComponent(Component::Type type);
	//void DeleteComponent(Component::Type type);

	//// Position
	//float3 GetPosition() const { return translation; }
	//void SetLocalPosition(const float3& position);

	//// Rotation
	//float3 GetRotation() const { return rotation; }
	//Quat GetRotationQ() const { return rotation_quat; }
	//void SetRotation(const float3& XYZ_euler_rotation);
	//void SetRotation(const Quat& rotation);

	//// Scale
	//float3 GetScale() const { return scale; }
	//void SetLocalScale(const float3& Scale);

	//// Transform
	//float4x4 GetLocalTransform() { return local_transform; }
	//float4x4 GetGlobalTransform() const { return global_transform; }
	//void UpdateTransform();
	//void SetTransform(const float4x4& transform);
	//void ResetTransform();

	//// Velocity
	//float3 GetVelocity() const { return velocity; }
	//void Move(const float3& velocity);
	//void Rotate(float angular_velocity);

	//// Bounding Box
	//const AABB& GetAABB() { return aabb; }
	//const OBB& GetOBB() { return obb; }
	//void UpdateParentBoundingBox();
	//void GenerateBoundingBox();
	//void DeleteBoundingBox();
	//void UpdateBoundingBox();
	//void GetMinMaxVertex(GameObject * obj, float3 * abs_max, float3 * abs_min);

private:
	UID uid = 0;
	std::string name;
	bool active = true;

	GameObject* parent;
	std::vector<GameObject*> childs;
	//std::vector<Component*> components;

	//float3 translation = float3::zero;
	//Quat rotation_quat = Quat::identity;
	//float3 rotation = float3::zero;
	//float3 scale = float3::one;
	//float4x4 local_transform = math::float4x4::identity;
	//float4x4 global_transform = math::float4x4::identity;
	////float3 velocity = float3::zero;

public:
	bool is_static = false;
	bool is_drawn = true;
	bool is_selected = false;

	//// Bounding box
	//bool show_aabb = false;
	//bool show_obb = false;
	//AABB aabb;
	//OBB obb;
	//GLuint aabb_VBO = 0;
	//GLuint obb_VBO = 0;
	//float3 corners[8];
};
