/*
* MIT License
*
* Copyright (c) 2020 Jimmie Bergmann
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/

#include "Curse/Scene/Camera.hpp"

namespace Curse
{

    namespace Scene
    {

        Camera::Camera() :
            m_projectionUpdated(true),
            m_viewUpdated(true),
            m_position{ 0.0f, 0.0f, 0.0f },
            m_rotation(Radians(0.0f), Radians(0.0f), Radians(0.0f)),
            m_direction{ 0.0f, 1.0f, 0.0f },
            m_projectionType(ProjectionType::Perspective),
            m_fieldOfView(Degrees(60)),
            m_windowSize { 0, 0 },
            m_rotationMatrix(Matrix3x3f32::Identity()),
            m_projectionMatrix(Matrix4x4f32::Identity()),
            m_viewMatrix(Matrix4x4f32::Identity())
        { }

        Camera::~Camera()
        { }

        void Camera::PostProcess()
        {
            if (m_projectionUpdated)
            {
                m_projectionUpdated = false;

                if (m_projectionType == ProjectionType::Perspective)
                {
                    float aspect = static_cast<float>(m_windowSize.x) / static_cast<float>(m_windowSize.y);
                    m_projectionMatrix = Matrix4x4f32::Perspective(m_fieldOfView, aspect, 0.1f, 100.0f);
                }
                else
                {
                    m_projectionMatrix = Matrix4x4f32::Orthographic(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 100.0f);
                }
            }

            if (m_viewUpdated)
            {
                m_viewUpdated = false;

                Vector3f32 rads = {
                    m_rotation.x.AsRadians<float>(),
                    m_rotation.y.AsRadians<float>(),
                    m_rotation.z.AsRadians<float>()
                };

                Matrix3x3f32 rotX =
                {
                    {1.0f, 0.0f, 0.0f},
                    {0.0f, std::cos(rads.x), -std::sin(rads.x)},
                    {0.0f, std::sin(rads.x), std::cos(rads.x)},
                };

                Matrix3x3f32 rotY =
                {
                    {std::cos(rads.y), 0.0f, std::sin(rads.y)},
                    {0.0f, 1.0f, 0.0f},
                    {-std::sin(rads.y), 0.0f, std::cos(rads.y)},
                };

                Matrix3x3f32 rotZ =
                {
                    {std::cos(rads.z), -std::sin(rads.z), 0.0f},
                    {std::sin(rads.z), std::cos(rads.z), 0.0f},
                    {0.0f, 0.0f, 1.0f},
                };

                m_rotationMatrix = rotX * rotY * rotZ;

                m_direction = Vector3f32{ 0.0f, 1.0f, 0.0f };
                m_direction = m_rotationMatrix * m_direction;

                Vector3f32 up = Vector3f32{ 0.0f, 1.0f, 0.0f };
                float rotUpRads = Degrees(90.0f).AsRadians<float>();
                Matrix3x3f32 rotUp =
                {
                    {1.0f, 0.0f, 0.0f},
                    {0.0f, std::cos(rotUpRads), -std::sin(rotUpRads)},
                    {0.0f, std::sin(rotUpRads), std::cos(rotUpRads)},
                };

                up = rotUp * m_rotationMatrix * up;

                m_viewMatrix = Curse::Matrix4x4f32::LookAtDirection(m_position, m_direction, up);
            }                   
        }

        void Camera::AddYaw(const Angle angle)
        {
            auto prevRot = m_rotation.z;
            auto newRot = (m_rotation.z + angle).Normal();
            if (newRot != prevRot)
            {
                m_rotation.z = newRot;
                m_viewUpdated = true;
            }
        }

        void Camera::AddRoll(const Angle angle)
        {
            auto prevRot = m_rotation.y;
            auto newRot = (m_rotation.y + angle).Normal();
            if (newRot != prevRot)
            {
                m_rotation.y = newRot;
                m_viewUpdated = true;
            }
        }

        void Camera::AddPitch(const Angle angle)
        {
            auto prevRot = m_rotation.x;
            auto newRot = (m_rotation.x + angle).Normal();
            if (newRot != prevRot)
            {
                m_rotation.x = newRot;
                m_viewUpdated = true;
            }
        }

        Vector3f32 Camera::GetPosition() const
        {
            return m_position;
        }

        Vector3<Angle> Camera::GetEulerRotation() const
        {
            return m_rotation;
        }

        Vector3f32 Camera::GetForwardDirection() const
        {
            return { m_rotationMatrix.e[1], m_rotationMatrix.e[4], m_rotationMatrix.e[7] };
        }

        Vector3f32 Camera::GetUpDirection() const
        {
            return { m_rotationMatrix.e[2], m_rotationMatrix.e[5], m_rotationMatrix.e[8] };
        }

        Vector3f32 Camera::GetRightDirection() const
        {
            return { m_rotationMatrix.e[0], m_rotationMatrix.e[3], m_rotationMatrix.e[6] };
        }

        Camera::ProjectionType Camera::GetProjectionType() const
        {
            return m_projectionType;
        }

        Angle Camera::GetFieldOfView() const
        {
            return m_fieldOfView;
        }

        Vector2ui32 Camera::GetWindowSize() const
        {
            return m_windowSize;
        }

        const Matrix3x3f32& Camera::GetRotationMatrix() const
        {
            return m_rotationMatrix;
        }

        const Matrix4x4f32& Camera::GetProjectionMatrix() const
        {
            return m_projectionMatrix;
        }

        const Matrix4x4f32& Camera::GetViewMatrix() const
        {
            return m_viewMatrix;
        }

        void Camera::SetPosition(const Vector3f32& position)
        {
            if (m_position != position)
            {
                m_position = position;
                m_viewUpdated = true;
            }
        }

        void Camera::SetEulerRotation(const Vector3<Angle>& rotation)
        {
            auto prevRot = m_rotation;
            m_rotation =
            {
                rotation.x.Normal(),  rotation.y.Normal(),  rotation.z.Normal()
            };

            if (rotation != prevRot)
            {
                m_viewUpdated = true;
            }
        }

        void Camera::SetDirection(const Vector3f32& direction)
        {
            /// FIX WITH ROTATIONS!
            Vector3f32 newDirection = direction.Normal();
            if (m_direction != newDirection)
            {
                m_direction = newDirection;
                m_viewUpdated = true;
            }
            /// FIX WITH ROTATIONS!
        }

        void Camera::SetProjectionType(const ProjectionType projectionType)
        {
            if (m_projectionType != projectionType)
            {
                m_projectionType = projectionType;
                m_projectionUpdated = true;
            }
        }

        void Camera::SetFieldOfView(const Angle fieldOfView)
        {
            if (m_fieldOfView != fieldOfView)
            {
                m_fieldOfView = fieldOfView;
                m_projectionUpdated = true;
            }
        }

        void Camera::SetWindowSize(const Vector2ui32& windowSize)
        {
            if (m_windowSize != windowSize)
            {
                m_windowSize = windowSize;
                m_projectionUpdated = true;
            }
            
        }

    }

}