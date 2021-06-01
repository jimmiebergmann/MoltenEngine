/*
* MIT License
*
* Copyright (c) 2019 Jimmie Bergmann
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

#include "Molten/FileFormat/Image/BmpFormat.hpp"

namespace Molten::Formats::Bmp
{

 
    // Header implementations
    Header::Header() :
        m_signature(0),
        m_fileSize(0),
        m_reserved(0),
        m_dataOffset(0)
    {}

    uint16_t& Header::GetSignature()
    {
        return m_signature;
    }
    uint16_t Header::GetSignature() const
    {
        return m_signature;
    }

    uint32_t& Header::GetFileSize()
    {
        return m_fileSize;
    }
    uint32_t Header::GetFileSize() const
    {
        return m_fileSize;
    }

    uint32_t& Header::GetReserved()
    {
        return m_reserved;
    }
    uint32_t Header::GetReserved() const
    {
        return m_reserved;
    }

    uint32_t& Header::GetDataOffset()
    {
        return m_dataOffset;
    }
    uint32_t Header::GetDataOffset() const
    {
        return m_dataOffset;
    }


    // File implementations.
    File::File() :
        m_header{},
        m_infoHeader{},
        m_colorTable{},
        m_data{}
    {}

    Header& File::GetHeader()
    {
        return m_header;
    }
    const Header& File::GetHeader() const
    {
        return m_header;
    }

    InfoHeader& File::GetInfoHeader()
    {
        return m_infoHeader;
    }
    const InfoHeader& File::GetInfoHeader() const
    {
        return m_infoHeader;
    }

    ColorTable& File::GetColorTable()
    {
        return m_colorTable;
    }
    const ColorTable& File::GetColorTable() const
    {
        return m_colorTable;
    }

    Data& File::GetData()
    {
        return m_data;
    }
    const Data& File::GetData() const
    {
        return m_data;
    }


    // Header implementations.



    // Info header implementations.



    // Color table implementations.


    // Reader result implementations.
    ReaderResult::ReaderResult() :
        m_file(),
        m_resultCode(ResultCode::Successful)
    {}

    bool ReaderResult::IsSuccessful() const
    {
        return m_resultCode == ResultCode::Successful;
    }

    ResultCode ReaderResult::GetCode() const
    {
        return m_resultCode;
    }

    File& ReaderResult::GetFile()
    {
        return m_file;
    }
    const File& ReaderResult::GetFile() const
    {
        return m_file;
    }


    // Reader implementations.
    ReaderResult ReadFromFile(const std::string& /*filename*/)
    {
        ReaderResult result;
        return result;
    }

    ReaderResult ReadFromBytes(const std::vector<uint8_t>& /*bytes*/)
    {
        ReaderResult result;
        return result;
    }

    ReaderResult ReadFromStream(const std::istream& /*stream*/)
    {
        ReaderResult result;
        return result;
    }

}