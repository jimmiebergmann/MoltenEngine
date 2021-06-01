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

#ifndef MOLTEN_CORE_FILEFORMAT_IMAGE_BMPFORMAT_HPP
#define MOLTEN_CORE_FILEFORMAT_IMAGE_BMPFORMAT_HPP

#include "Molten/Types.hpp"
#include <vector>
#include <string>
#include <istream>

namespace Molten::Formats::Bmp
{

    class File;
    class Header;
    class InfoHeader;
    class ColorTable;
    class ReaderResult;

    using Data = std::vector<uint8_t>; ///< Data type of BMP data bytes.
    
    /** Result enumerator for describing result of reading BMP image. */
    enum class ResultCode
    {
        Successful,
        CannotOpenFile,
        UnexpectedEndOfFile
    };


    /** BMP Header class, containing basic file information. */
    class MOLTEN_API Header
    {

    public:

        Header();

        uint16_t& GetSignature();
        uint16_t GetSignature() const;

        uint32_t& GetFileSize();
        uint32_t GetFileSize() const;

        uint32_t& GetReserved();
        uint32_t GetReserved() const;

        uint32_t& GetDataOffset();
        uint32_t GetDataOffset() const;

    private:

        uint16_t m_signature;
        uint32_t m_fileSize;
        uint32_t m_reserved;
        uint32_t m_dataOffset;

    };


    /** BMP Info Header class, containing information about bitmap data. */
    class MOLTEN_API InfoHeader
    {

    public:


    };


    /** BMP Color Table class. This section is optional, but used for padding indices to color values. */
    class MOLTEN_API ColorTable
    {

    public:


    };


    /** BMP File class contains all data necessary for reading and writing BMP image files. */
    class MOLTEN_API File
    {

    public:

        File();

        /** Get const or non-const reference to header data. */
        Header& GetHeader();
        const Header& GetHeader() const;

        /** Get const or non-const reference to info(DIB) header. */
        InfoHeader& GetInfoHeader();
        const InfoHeader& GetInfoHeader() const;

        /** Get const or non-const reference to color table. */
        ColorTable& GetColorTable();
        const ColorTable& GetColorTable() const;

        /** Get const or non-const reference raw data bytes. */
        Data& GetData();
        const Data& GetData() const;

    private:

        Header m_header;
        InfoHeader m_infoHeader;
        ColorTable m_colorTable;
        Data m_data;

    };


    /** Result class, returned from ReadFrom*, containing the parsed BMP image and result code. */
    class ReaderResult
    {

    public:

        /** Constructor. */
        ReaderResult();

        /** @return true if reader result is successful. */
        bool IsSuccessful() const;

        ResultCode GetCode() const;

        /** Get reference to read BMP file. */
        File& GetFile();
        const File& GetFile() const;

    private:

        File m_file;
        ResultCode m_resultCode;

    };


    MOLTEN_API ReaderResult ReadFromFile(const std::string& filename);
    MOLTEN_API ReaderResult ReadFromBytes(const std::vector<uint8_t>& bytes);
    MOLTEN_API ReaderResult ReadFromStream(const std::istream& stream);
    
}

#endif