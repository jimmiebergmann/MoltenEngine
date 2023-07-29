/*
* MIT License
*
* Copyright (c) 2023 Jimmie Bergmann
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

#ifndef MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_MESH_OBJMESHFILE_HPP
#define MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_MESH_OBJMESHFILE_HPP

#include "Molten/EditorFramework/Build.hpp"
#include "Molten/FileFormat/TextFileFormatResult.hpp"
#include "Molten/Utility/Expected.hpp"
#include "Molten/Math/Vector.hpp"
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
}

namespace Molten::EditorFramework
{

    /** Obj mesh file format.
     * The obj file format specification supports many different types of ordered and in-place commands,
     * but this implementation only supports the most basic ones for loading a scene or objects with multiple sub-meshes and material attachments.
     * Worth noting is that this implementation does not supports all different kind of grouping combinations or in-place commands.
     * For example, quads are not supported, but automatically converted into two triangles, simply because Molten does not care about quads.
     *
     * ObjMeshFile structure:
     *  - Object[n]
     *    - Name
     *    - vertex[n]
     *    - normal[n]
     *    - TextureCoordinate[n]
     *    - Group[n]
     *       - SmoothingGroup[n]
     *         - Id (flag)
     *         - Triangle[n]
     *           - vertexIndex[3]
     *           - textureCoordinateIndex[3]
     *           - normalIndex[3]
     *
     * Supported commands:
     *  - # - Comments.
     *  - o - Object.
     *  - g - Group.
     *  - s - Smoothing group
     *  - usemtl - Using material.
     *  - mtllib - Loading material file.
     *  - v - Vertex coordinate.
     *  - vn - Vertex normal.
     *  - vt - Vertex texture coordinate.
     *  - f - Face (Quads are split into two triangles)
     *
     * Documentation:
     *  - http://www.martinreddy.net/gfx/3d/OBJ.spec
     */
    class MOLTEN_EDITOR_FRAMEWORK_API ObjMeshFile
    {

    public:

        /** Triangle indices, pointing to Object vertices/normals/textureCoordinates. 
         *  Index is set to std::numeric_limits<uint32_t>::max() if unused. 
         */
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

            uint32_t id; ///< id > 0 == on, id == 0 == off.
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

        using MaterialFiles = std::vector<std::string>;

        ObjectSharedPointers objects; ///< List of objects.
        MaterialFiles materialFiles; ///< List of associated material filenames.

        ObjMeshFile() = default;
        ~ObjMeshFile() = default;

        ObjMeshFile(const ObjMeshFile&) = default;
        ObjMeshFile(ObjMeshFile&&) = default;
        ObjMeshFile& operator = (const ObjMeshFile&) = default;
        ObjMeshFile& operator = (ObjMeshFile&&) = default;

        void Clear();

    };

    /** Obj mesh material file format.
     *
     * Documentation:
     *  - http://paulbourke.net/dataformats/mtl/
     *  - https://www.fileformat.info/format/material/
     */
    class MOLTEN_EDITOR_FRAMEWORK_API ObjMaterialFile
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
            TextureOptions options; ///< Not parsed at the moment.
        };

        /** Material with optional properties. */
        struct Material
        {
            /** Standard properties. */
            /**@{*/
            std::string name; ///< Name of material.
            std::optional<Vector3f32> ambientColor; ///< Ka = rgb{ 0.0 - 1.0, ... }
            std::optional<Vector3f32> diffuseColor; ///< Kd = rgb{ 0.0 - 1.0, ... }
            std::optional<Vector3f32> specularColor; ///< Ks = rgb{ 0.0 - 1.0, ... }
            std::optional<float> specularExponent; ///< Ns = 0.0 - 1000.0
            std::optional<float> dissolve; ///< d = 0.0 - 1.0 or Tr = (1.0 - d)
            std::optional<float> opticalDensity; ///< Ni = Refractive index
            std::optional<MaterialTexture> ambientTexture; ///< map_Ka
            std::optional<MaterialTexture> diffuseTexture; ///< map_Kd - Often same as map_Ka
            std::optional<MaterialTexture> specularTexture; ///< map_Ks
            std::optional<MaterialTexture> specularExponentTexture; ///< map_Ns
            std::optional<MaterialTexture> dissolveTexture; ///< map_d
            std::optional<MaterialTexture> bumpTexture; ///< map_bump / bump
            std::optional<MaterialTexture> displacementTexture; ///< disp = Normal map
            /**@}*/

            /** PBR properties. */
            /**@{*/
            std::optional<float> roughness; ///< Pr
            std::optional<float> metallic; ///< Pm
            std::optional<Vector3f32> emissiveColor; ///< Ke
            std::optional<MaterialTexture> roughnessTexture; ///< map_Pr
            std::optional<MaterialTexture> metallicTexture; ///< map_Pm
            std::optional<MaterialTexture> emissiveTexture; ///< map_Ke
            /**@}*/
        };

        using MaterialSharedPointer = std::shared_ptr<Material>;
        using MaterialSharedPointers = std::vector<MaterialSharedPointer>;

        MaterialSharedPointers materials; ///< List of materials.

        void Clear();
    };

    struct ObjMeshReaderWarning
    {
        size_t line = 0;
        std::string message = {};
    };

    template<typename T>
    struct ObjMeshReaderResult
    {
        T file = {};
        std::vector<ObjMeshReaderWarning> warnings = {};
    };


    using ObjMeshFileReaderResult = Expected<ObjMeshFile, TextFileFormatError>;


    /** Helper class for reading obj mesh files. */
    class MOLTEN_EDITOR_FRAMEWORK_API ObjMeshFileReader
    {

    public:

        ObjMeshFileReader();       
        ~ObjMeshFileReader() = default;

        ObjMeshFileReader(ObjMeshFileReader&&) = delete;
        ObjMeshFileReader& operator = (ObjMeshFileReader&&) = delete;

        ObjMeshFileReader(const ObjMeshFileReader&) = delete;
        ObjMeshFileReader& operator = (const ObjMeshFileReader&) = delete;

        /** Read and parse obj mesh file on a single thread. */
        [[nodiscard]] ObjMeshFileReaderResult Read(const std::filesystem::path& filename);

        /** Read and parse obj mesh file using multiple threads from provided thread pool. */
        [[nodiscard]] ObjMeshFileReaderResult Read(
            const std::filesystem::path& filename,
            ThreadPool& threadPool);

        /** Read and parse obj mesh file on a single thread. */
        [[nodiscard]] ObjMeshFileReaderResult Read(std::istream& stream);

        /** Read and parse obj mesh file using multiple threads from provided thread pool. */
        [[nodiscard]] ObjMeshFileReaderResult Read(
            std::istream& stream,
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

        using SmoothingGroup = ObjMeshFile::SmoothingGroup;
        using Group = ObjMeshFile::Group;
        using Object = ObjMeshFile::Object;
        using ObjectSharedPointer = std::shared_ptr<Object>;

        using ProcessObjectResult = Expected<void, TextFileFormatError>;
        using ParseObjectResult = Expected<ObjectSharedPointer, TextFileFormatError>;
        using ParseObjectFuture = std::future<ParseObjectResult>;
        using ParseObjectFutures = std::vector<ParseObjectFuture>;

        /** This function is called by Read and performs all reading and parsing tasks.
         *  Called of this function must wait for all tasks before proceeding execution.
         */
        [[nodiscard]] ObjMeshFileReaderResult InternalRead(const std::filesystem::path& filename);
        [[nodiscard]] ObjMeshFileReaderResult InternalRead(std::istream& stream);

        [[nodiscard]] ProcessObjectResult ProcessObject(ObjMeshFile& objMeshFile, ObjectBufferSharedPointer objectBuffer);
        [[nodiscard]] ParseObjectResult ParseObjectCommands(ObjectBufferSharedPointer objectBuffer);
        [[nodiscard]] ParseObjectFuture ParseObjectCommandsAsync(ObjectBufferSharedPointer objectBuffer);

        [[nodiscard]] ProcessObjectResult HandleObjectFutures(ObjMeshFile& objMeshFile);
        [[nodiscard]] ProcessObjectResult TryHandleObjectFutures(ObjMeshFile& objMeshFile);

        ThreadPool* m_threadPool;
        ParseObjectFutures m_objectFutures;

    };

    MOLTEN_EDITOR_FRAMEWORK_API [[nodiscard]] ObjMeshFileReaderResult ReadObjMeshFile(
        const std::filesystem::path& filename);

    MOLTEN_EDITOR_FRAMEWORK_API [[nodiscard]] ObjMeshFileReaderResult ReadObjMeshFile(
        const std::filesystem::path& filename,
        ThreadPool& threadPool);

    MOLTEN_EDITOR_FRAMEWORK_API [[nodiscard]] ObjMeshFileReaderResult ReadObjMeshFile(
        std::istream& stream);

    MOLTEN_EDITOR_FRAMEWORK_API [[nodiscard]] ObjMeshFileReaderResult ReadObjMeshFile(
        std::istream& stream,
        ThreadPool& threadPool);


    using ObjMaterialReaderResult = ObjMeshReaderResult<ObjMaterialFile>;
    using ObjMaterialFileReaderResult = Expected<ObjMaterialReaderResult, TextFileFormatError>;

    struct ObjMaterialFileReaderOptions
    {
        bool useWarnings = false;
        bool warningsAsErrors = false;
        bool ignoreUnknownCommands = true;
        bool ignoreDuplicateCommands = true;
    };

    /** Helper class for reading obj mesh files. */
    class MOLTEN_EDITOR_FRAMEWORK_API ObjMaterialFileReader
    {

    public:

        ObjMaterialFileReader() = default;
        ~ObjMaterialFileReader() = default;

        ObjMaterialFileReader(ObjMaterialFileReader&&) = delete;
        ObjMaterialFileReader& operator = (ObjMaterialFileReader&&) = delete;

        ObjMaterialFileReader(const ObjMaterialFileReader&) = delete;
        ObjMaterialFileReader& operator = (const ObjMaterialFileReader&) = delete;

        /** Read and parse obj mesh file on a single thread. */
        [[nodiscard]] ObjMaterialFileReaderResult Read(
            const std::filesystem::path& filename,
            const ObjMaterialFileReaderOptions& options = {});

        /** Read and parse obj mesh file on a single thread. */
        [[nodiscard]] ObjMaterialFileReaderResult Read(
            std::istream& stream,
            const ObjMaterialFileReaderOptions& options = {});

    private:

        enum class MaterialCommandType
        {
            // Standard properties:
            NewMaterial,                ///< newmtl
            AmbientColor,               ///< Ka
            DiffuseColor,               ///< Kd 
            SpecularColor,              ///< Ks
            SpecularExponent,           ///< Ns
            Dissolve,                   ///< d
            OpticalDensity,             ///< Ni
            AmbientTexture,             ///< map_Ka
            DiffuseTexture,             ///< map_Kd
            SpecularTexture,            ///< map_Ks
            SpecularExponentTexture,    ///< map_Ns
            DissolveTexture,            ///< map_Ns
            BumpTexture,                ///< map_bump / bump
            DisplacementTexture,        ///< disp

            // PBR extensions:
            Roughness,                  ///< Pr
            Metallic,                   ///< Pm
            EmissiveColor,              ///< Ke
            RoughnessTexture,           ///< map_Pr
            MetallicTexture,            ///< map_Pm
            EmissiveTexture,            ///< map_Ke
        };

        struct MaterialCommand
        {
            MaterialCommand(
                const size_t lineNumber,
                const MaterialCommandType type,
                const std::string_view data = {});

            MaterialCommand(MaterialCommand&&) = default;
            MaterialCommand& operator = (MaterialCommand&&) = default;

            MaterialCommand(const MaterialCommand&) = delete;
            MaterialCommand& operator = (const MaterialCommand&) = delete;

            size_t lineNumber; ///< Line number, used for error messages.
            MaterialCommandType type; ///< Type of line.
            std::string_view data; ///< Command data.
        };

        using MaterialCommands = std::vector<MaterialCommand>;
        using Buffer = std::shared_ptr<char[]>;
        using Buffers = std::vector<Buffer>;

        struct MaterialBuffer
        {
            Buffers buffers;
            MaterialCommands commands;
        };

        using MaterialBufferSharedPointer = std::shared_ptr<MaterialBuffer>;

        using MaterialTexture = ObjMaterialFile::MaterialTexture;
        using TextureOptions = ObjMaterialFile::TextureOptions;
        using Material = ObjMaterialFile::Material;

        using ProcessMaterialResult = Expected<void, TextFileFormatError>;

        [[nodiscard]] ProcessMaterialResult ProcessMaterial(
            ObjMaterialReaderResult& readerResult,
            const ObjMaterialFileReaderOptions& options,
            ObjMaterialFile& objMaterialFile,
            MaterialBufferSharedPointer materialBuffer);

    };

    MOLTEN_EDITOR_FRAMEWORK_API [[nodiscard]] ObjMaterialFileReaderResult ReadObjMaterialFile(
        const std::filesystem::path& filename,
        const ObjMaterialFileReaderOptions& options = {});

    MOLTEN_EDITOR_FRAMEWORK_API [[nodiscard]] ObjMaterialFileReaderResult ReadObjMaterialFile(
        std::istream& stream,
        const ObjMaterialFileReaderOptions& options = {});

}

#endif