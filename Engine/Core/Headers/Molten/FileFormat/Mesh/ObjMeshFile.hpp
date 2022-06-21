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

#ifndef MOLTEN_CORE_FILEFORMAT_MESH_OBJ_HPP
#define MOLTEN_CORE_FILEFORMAT_MESH_OBJ_HPP

#include "Molten/Math/Vector.hpp"
#include "Molten/FileFormat/TextFileFormatResult.hpp"
#include "Molten/System/Signal.hpp"
#include <string>
#include <string_view>
#include <filesystem>
#include <array>
#include <vector>
#include <variant>
#include <optional>
#include <future>

namespace Molten
{

    /** Forward declarations. */
    class ThreadPool;
    class ObjMeshFileReader;


    /** Obj mesh file format.
     * The obj file format specification supports many different types of ordered and in-place commands,
     * but this implementation only supports the most basic ones for loading a scene or objects with multiple sub-meshes and material attachments.
     * Worth noting is that this implementation does not supports all different kind of grouping combinations or in-place commands.
     * For example, quads are not supported, but automatically converted into two triangles, simply because Molten does not care about quads.
     *
     * ObjMeshFile structure:
     * - Triangles per SmoothingGroup
     * - SmoothingGroups per Group
     * - Groups(material usage or grouped by name) per Object
     * - Vertex data per Object
     *
     * Supported commands:
     * - # - Comments.
     * - o - Object.
     * - g - Group.
     * - s - Smoothing group
     * - usemtl - Using material.
     * - mtllib - Loading material file.
     * - v - Vertex coordinate.
     * - vn - Vertex normal.
     * - vt - Vertex texture coordinate.
     * - f - Face (Quads are split into two triangles)
     *
     * Documentation:
     * - http://paulbourke.net/dataformats/mtl/
     * - http://www.martinreddy.net/gfx/3d/OBJ.spec
     * - https://www.fileformat.info/format/material/
     */
    class MOLTEN_CORE_API ObjMeshFile
    {

    public:

        /** Texture options struct. */
        struct TextureOptions
        {
            //std::optional<bool> horizontalBlend; ///< -blendu = on | off  (Default on)
            //std::optional<bool> verticalBlend; ///< -blendv = on | off  (Default on)
            //std::optional<float> boostMipmapSharpness; ///< -boost
            std::optional<Vector2f32> modifier; ///< -mm = { brightness, contrast }
            std::optional<Vector3f32> originOffset; ///< -o = { x, [y, [z]] }  (Default { 0.0, 0.0, 0.0})
            std::optional<Vector3f32> scale; ///< -s = { x, [y, [z]] }  (Default { 1.0, 1.0, 1.0})
            //std::optional<Vector3f32> turbulence; ///< -t = { x, [y, [z]] }  (Default { 0.0, 0.0, 0.0})
            std::optional<bool> clamp; ///< -clamp = on | off
            //std::optional<float> bumpMultiplier; ///< bm
        };

        /** Texture struct, represented by filename and options. */
        struct MaterialTexture
        {
            std::string filename;
            TextureOptions options;
        };

        /** Material with optional properties. */
        struct Material
        {
            std::optional<Vector3f32> ambientColor; ///< Ka = rgb{ 0.0 - 1.0, ... }
            std::optional<Vector3f32> diffuseColor; ///< Kd = rgb{ 0.0 - 1.0, ... }
            std::optional<Vector3f32> specularColor; ///< Ks = rgb{ 0.0 - 1.0, ... }
            std::optional<float> specularWeight; ///< Ns = 0.0 - 1000.0
            std::optional<float> transparency; ///< d = 0.0 - 1.0 or Tr = (1.0 - d)
            std::optional<float> opticalDensity; ///< Ni = Refractive index
            std::optional<MaterialTexture> ambientTexture; ///< map_Ka
            std::optional<MaterialTexture> diffuseTexture; ///< map_Kd - Often same as map_Ka
            std::optional<MaterialTexture> specularTexture; ///< map_Ks
            std::optional<MaterialTexture> specularWeightTexture; ///< map_Ns
            std::optional<MaterialTexture> alphaTexture; ///< map_d
            //std::optional<MaterialTexture> bumpTexture; ///< map_bump - Ignored for now.
            //std::optional<MaterialTexture> luminanceBumpTexture; ///< bump - Ignored for now.
            std::optional<MaterialTexture> displacementTexture; ///< disp = Normal map
            //std::optional<MaterialTexture> stencilDecalTexture; ///< decal - Ignored for now.

            /** PBR materials*/
            /**@{*/
            std::optional<float> roughness; ///< Pr
            std::optional<float> metallic; ///< Pm
            std::optional<MaterialTexture> roughnessTexture; ///< map_Pr
            std::optional<MaterialTexture> metallicTexture; ///< map_Pm
            /**@}*/
        };

        using MaterialSharedPointer = std::shared_ptr<Material>;
        using MaterialSharedPointers = std::vector<MaterialSharedPointer>;

        /** Triangle indices, pointing to Object vertices/normals/textureCoordinates. Index is set to std::numeric_limits<uint32_t>::max() if unused. */
        struct Triangle
        {
            Triangle();

            std::array<uint32_t, 3> vertexIndices;
            std::array<uint32_t, 3> textureCoordinateIndices;
            std::array<uint32_t, 3> normalIndices;           
        };

        using Triangles = std::vector<Triangle>;

        struct SmoothingGroup
        {
            SmoothingGroup();

            uint32_t id; ///< (> 0) == on, (== 0) == off.
            Triangles triangles;

            [[nodiscard]] bool IsEmpty() const;
        };

        using SmoothingGroupSharedPointer = std::shared_ptr<SmoothingGroup>;
        using SmoothingGroupSharedPointers = std::vector<SmoothingGroupSharedPointer>;


        struct Group
        {
            std::string name; ///< Group name. Empty if material changed without a group change.
            std::string material; ///< White material if empty, else a material from an imported material file.
            SmoothingGroupSharedPointers smoothingGroups;

            [[nodiscard]] bool IsEmpty() const;
        };

        using GroupSharedPointer = std::shared_ptr<Group>;
        using GroupSharedPointers = std::vector<GroupSharedPointer>;


        using Vertices = std::vector<Vector3f32>;
        using Normals = std::vector<Vector3f32>;
        using Uv = std::vector<Vector2f32>;


        /** Obj object. */
        struct Object
        {
            std::string name;
            Vertices vertices;
            Normals normals;
            Uv textureCoordinates;
            GroupSharedPointers groups;
        };

        using ObjectSharedPointer = std::shared_ptr<Object>;
        using ObjectSharedPointers = std::vector<ObjectSharedPointer>;


        MaterialSharedPointers materials; ///< List of materials.
        ObjectSharedPointers objects; ///< List of objects.


        //ObjMeshFile();
        ObjMeshFile() = default;
        ~ObjMeshFile() = default;

        ObjMeshFile(const ObjMeshFile&) = default;
        ObjMeshFile(ObjMeshFile&&) = default;
        ObjMeshFile& operator = (const ObjMeshFile&) = default;
        ObjMeshFile& operator = (ObjMeshFile&&) = default;

        /** Read and parses obj mesh file using multiple threads from provided thread pool.
         *  Clear() is automatically called in the beginning of this method, so no need to call it manually before.
         */
        [[nodiscard]] TextFileFormatResult ReadFromFile(
            const std::filesystem::path& filename,
            ThreadPool& threadPool);

        /** Same as ReadFromFile(const std::string&, ThreadPool&), but lets the user provide its own reader object.
         *  Re-using a reader can speed up bulk loading.
         */
        [[nodiscard]] TextFileFormatResult ReadFromFile(
            const std::filesystem::path& filename,
            ObjMeshFileReader& reader);

        /** Clear all data and internal */
        void Clear();

    };


    /** Helper class for reading obj mesh files.
     *  This class is internally used by ObjMeshFile::ReadFromFile(...).
     */
    class MOLTEN_CORE_API ObjMeshFileReader
    {

    public:

        Signal<double> onProgress;

        ObjMeshFileReader();       
        ~ObjMeshFileReader() = default;

        ObjMeshFileReader(ObjMeshFileReader&&) = delete;
        ObjMeshFileReader& operator = (ObjMeshFileReader&&) = delete;

        ObjMeshFileReader(const ObjMeshFileReader&) = delete;
        ObjMeshFileReader& operator = (const ObjMeshFileReader&) = delete;

        /** Read and parse obj mesh file on a single thread.
        *  Clear() is automatically called on objMeshFile,
        *  so no need to call it manually before calling this function.
        */
        [[nodiscard]] TextFileFormatResult ReadFromFile(
            ObjMeshFile& objMeshFile,
            const std::filesystem::path& filename);

        /** Read and parse obj mesh file using multiple threads from provided thread pool.
        *  Clear() is automatically called on objMeshFile,
        *  so no need to call it manually before calling this function.
        */
        [[nodiscard]] TextFileFormatResult ReadFromFile(
            ObjMeshFile& objMeshFile,
            const std::filesystem::path& filename,
            ThreadPool& threadPool);

    private:

        enum class ObjectCommandType
        {
            Object, ///< o
            Vertex, ///< v
            Normal, ///< vn
            Uv, ///< vt
            Group, ///< g
            SmoothingGroup, ///< s
            Face, ///< f
            UseMaterial ///< usemtl
        };

        struct MaterialCommand
        {
            MaterialCommand(
                const size_t lineNumber,
                std::string line);

            size_t lineNumber; ///< Line number, used for error messages.
            std::string line; ///< Full line data.
        };

        struct ObjectCommand
        {
            ObjectCommand(
                const size_t lineNumber,
                const ObjectCommandType type,
                const std::string_view line);

            ObjectCommand(ObjectCommand&&) = default;
            ObjectCommand& operator = (ObjectCommand&&) = default;

            ObjectCommand(const ObjectCommand&) = delete;
            ObjectCommand& operator = (const ObjectCommand&) = delete;

            size_t lineNumber; ///< Line number, used for error messages.
            ObjectCommandType type; ///< Type of line.
            std::string_view line; ///< Line of command, trimmed, but includes command tokens.
        };
        
        using ObjectCommands = std::vector<ObjectCommand>;
        using Buffer = std::shared_ptr<char[]>;
        using Buffers = std::vector<Buffer>;

        struct ObjectBuffer
        {
            Buffers buffers;
            ObjectCommands commands;
        };

        using ObjectBufferSharedPointer = std::shared_ptr<ObjectBuffer>;

        using Material = ObjMeshFile::Material;
        using MaterialSharedPointer = std::shared_ptr<Material>;
        using ProcessMaterialResult = std::variant<MaterialSharedPointer, TextFileFormatResult::Error>;
        using ProcessMaterialFuture = std::future<ProcessMaterialResult>;
        using ProcessMaterialFutures = std::vector<ProcessMaterialFuture>;

        using SmoothingGroup = ObjMeshFile::SmoothingGroup;
        using Group = ObjMeshFile::Group;
        using Object = ObjMeshFile::Object;
        using ObjectSharedPointer = std::shared_ptr<Object>;
        using ProcessObjectResult = std::variant<ObjectSharedPointer, TextFileFormatResult::Error>;
        using ProcessObjectFuture = std::future<ProcessObjectResult>;
        using ProcessObjectFutures = std::vector<ProcessObjectFuture>;

        /** This function is called by ReadFromFile and performs all reading and parsing tasks.
         *  Called of this function must wait for all tasks before proceeding execution.
         */
        [[nodiscard]] TextFileFormatResult InternalReadFromFile(
            ObjMeshFile& objMeshFile,
            const std::filesystem::path& filename);

        void Prepare(
            ObjMeshFile& objMeshFile, 
            const std::filesystem::path& filename);

        [[nodiscard]] TextFileFormatResult ExecuteProcessMaterial(MaterialCommand&& materialCommand);
        [[nodiscard]] ProcessMaterialResult ProcessMaterial(std::string&& filename);
        [[nodiscard]] ProcessMaterialFuture ProcessMaterialAsync(std::string&& filename);

        [[nodiscard]] TextFileFormatResult ExecuteProcessObject(ObjectBufferSharedPointer objectBuffer);
        [[nodiscard]] ProcessObjectResult ProcessObject(ObjectBufferSharedPointer objectBuffer);
        [[nodiscard]] ProcessObjectFuture ProcessObjectAsync(ObjectBufferSharedPointer objectBuffer);

        [[nodiscard]] TextFileFormatResult HandleFutures();
        [[nodiscard]] TextFileFormatResult HandleMaterialFutures();
        [[nodiscard]] TextFileFormatResult HandleObjectFutures();

        [[nodiscard]] TextFileFormatResult TryHandleFutures();
        [[nodiscard]] TextFileFormatResult TryHandleMaterialFutures();
        [[nodiscard]] TextFileFormatResult TryHandleObjectFutures();

        ThreadPool* m_threadPool;
        ObjMeshFile* m_objMeshFile;
        std::filesystem::path m_objMeshDirectory;
        std::vector<std::string> m_materialFilenames;
        ProcessMaterialFutures m_materialFutures;
        ProcessObjectFutures m_objectFutures;

    };

}

#endif