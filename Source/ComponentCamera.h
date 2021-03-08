#pragma once
#include "Globals.h"

struct Camera
{
//	//friend class ModuleCamera;
//
//public:
//	Camera();
//
//	void CleanUp();
//
//	void Bind();
//	void Unbind();
//	uint GetTextId();
//
//	void SetPosition(const float2& pos);
//	const float2 GetPosition();
//
//	void SetRotationDegrees(float rotation_degrees);
//	float GetRotationDegrees() const;
//
//	void SetZDir(const float3& front);
//	void SetYDir(const float3& up);
//	float3 GetZDir();
//	float3 GetYDir();
//
//	std::vector<float2> GetCorners();
//
//	void SetNearPlaneDistance(const float& set);
//	void SetFarPlaneDistance(const float& set);
//	void SetViewportSize(float width, float height);
//	void SetSize(float size);
//
//	float2 GetViewportSize() const;
//
//	const float GetSize() const;
//	const float GetNearPlaneDistance() const;
//	const float GetFarPlaneDistance() const;
//
//	const float GetViewScale() const;
//
//	const float4x4 GetViewMatrix() const;
//	const float4x4 GetProjectionMatrix() const;
//	const float4x4 GetOpenGLViewMatrix() const;
//	const float4x4 GetOpenGLProjectionMatrix() const;
//
//	void MoveFront(const float& speed);
//	void MoveBack(const float& speed);
//	void MoveRight(const float& speed);
//	void MoveLeft(const float& speed);
//	void MoveUp(const float& speed);
//	void MoveDown(const float& speed);
//
//	LineSegment ShootRay(const Rect& bounds_rect, const float2& shoot_pos);
//
//	bool CheckInsideFrustum(const AABB& box);
//	void SetFrustumCulling(bool set);
//	bool GetFrustumCulling();
//
//	Frustum GetFrustum();
//
//	void SetComponentCamera(ComponentCamera* comp);
//	ComponentCamera* GetComponentCamera() const;
//
//private:
//	void UpdateTransform();
//
//private:
//	Frustum		     frustum;
//	float2		     viewport_size = float2::zero;
//	float		     size = 1;
//
//	bool	         frustum_culling = true;
//
//	RenderTexture* render_tex = nullptr;
//
//	ComponentCamera* component_camera = nullptr;
};
