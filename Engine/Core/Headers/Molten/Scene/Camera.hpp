/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_SCENE_CAMERA_HPP
#define MOLTEN_CORE_SCENE_CAMERA_HPP

#include "Molten/Math/Matrix.hpp"
#include "Molten/Math/Angle.hpp"

namespace Molten
{

    namespace Scene
    {

        /**
        * @brief Base class of cameras.
        */
        class MOLTEN_CORE_API Camera
        {

        public:

            /**
            * @brief Enumerator of projection types.
            */
            enum class ProjectionType : uint8_t
            {
                Perspective,
                Orthographic
            };

            /**
            * @brief Constructor.
            */
            Camera();

            /**
            * @brief Virtual destructor.
            */
            virtual ~Camera();

            /**
            * @brief Post-process the camera, by updating the public matrices and view frustum.
            */
            virtual void PostProcess();

            virtual void AddPitch(const Angle angle);
            virtual void AddRoll(const Angle angle);
            virtual void AddYaw(const Angle angle);

            /**
            * @brief Get camera's position in world space.
            */
            virtual Vector3f32 GetPosition() const;

            /**
            * @brief Get camera's euler rotation in space.
            */
            virtual Vector3<Angle> GetEulerRotation() const;

            /**
            * @brief Get camera's normalized forward direction.
            */
            virtual Vector3f32 GetForwardDirection() const;

            /**
            * @brief Get camera's normalized up direction.
            */
            virtual Vector3f32 GetUpDirection() const;

            /**
            * @brief Get camera's normalized right direction.
            */
            virtual Vector3f32 GetRightDirection() const;

            /**
            * @brief Get camera's projection type.
            */
            virtual ProjectionType GetProjectionType() const;

            /**
             * @brief Set camera's field of view, from center to left or right.
             *        Only used by PostProcess() when GetProjection == ProjectionType::Perspective.
             */
            virtual Angle GetFieldOfView() const;

            /**
            * @brief Get camera's window size, in pixels.
            */
            virtual Vector2ui32 GetWindowSize() const;

            /**
            * @brief Get camera rotation matrix in world space,
            *        where the different columns represents rotation directions:
            *        * column[0] = pitch direction.
            *        * column[1] = yaw direction.
            *        * column[2] = roll direction.
            */
            virtual const Matrix3x3f32& GetRotationMatrix() const;

            /**
            * @brief Get camera's projection matrix.
            */
            virtual const Matrix4x4f32& GetProjectionMatrix() const;

            /**
            * @brief Get camera's view matrix.
            */
            virtual const Matrix4x4f32& GetViewMatrix() const;

            /**
            * @brief Set camera's position in world space.
            */
            virtual void SetPosition(const Vector3f32& position);

            /**
            * @brief Set camera's euler rotation in space.
            */
            virtual void SetEulerRotation(const Vector3<Angle>& rotation);

            /**
            * @brief Set camera's normalized viewing direction.
            */
            virtual void SetDirection(const Vector3f32& direction);
            
            /**
            * @brief Set camera's projection type.
            */
            virtual void SetProjectionType(const ProjectionType projectionType);

            /**
            * @brief Set camera's field of view, from center to left or right.
            *        Only used by PostProcess() when GetProjection == ProjectionType::Perspective.
            */
            virtual void SetFieldOfView(const Angle fieldOfView);

            /**
            * @brief Set camera's window size, in pixels.
            */
            virtual void SetWindowSize(const Vector2ui32& windowSize);

        protected:

            bool m_projectionUpdated;
            bool m_viewUpdated;
            Vector3f32 m_position;
            Vector3<Angle> m_rotation;
            Vector3f32 m_direction;
            ProjectionType m_projectionType;
            Angle m_fieldOfView;
            Vector2ui32 m_windowSize;
            Matrix3x3f32 m_rotationMatrix;
            Matrix4x4f32 m_projectionMatrix;
            Matrix4x4f32 m_viewMatrix;

        };

    }

}

#endif