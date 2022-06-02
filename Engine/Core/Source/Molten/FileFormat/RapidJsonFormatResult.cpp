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



#include "Molten/FileFormat/RapidJsonFormatResult.hpp"


namespace Molten
{

    static JsonParseErrorCode ConvertJsonErrorCode(const rapidjson::ParseErrorCode rapidJsonErrorCode)
    {
        switch (rapidJsonErrorCode)
        {
            case rapidjson::kParseErrorNone: return JsonParseErrorCode::None;
            case rapidjson::kParseErrorDocumentEmpty: return JsonParseErrorCode::DocumentEmpty;
            case rapidjson::kParseErrorDocumentRootNotSingular: return JsonParseErrorCode::DocumentRootNotSingular;
            case rapidjson::kParseErrorValueInvalid: return JsonParseErrorCode::ValueInvalid;
            case rapidjson::kParseErrorObjectMissName: return JsonParseErrorCode::ObjectMissName;
            case rapidjson::kParseErrorObjectMissColon: return JsonParseErrorCode::ObjectMissColon;
            case rapidjson::kParseErrorObjectMissCommaOrCurlyBracket: return JsonParseErrorCode::ObjectMissCommaOrCurlyBracket;
            case rapidjson::kParseErrorArrayMissCommaOrSquareBracket: return JsonParseErrorCode::ArrayMissCommaOrSquareBracket;
            case rapidjson::kParseErrorStringUnicodeEscapeInvalidHex: return JsonParseErrorCode::StringUnicodeEscapeInvalidHex;
            case rapidjson::kParseErrorStringUnicodeSurrogateInvalid: return JsonParseErrorCode::StringUnicodeSurrogateInvalid;
            case rapidjson::kParseErrorStringEscapeInvalid: return JsonParseErrorCode::StringEscapeInvalid;
            case rapidjson::kParseErrorStringMissQuotationMark: return JsonParseErrorCode::StringMissQuotationMark;
            case rapidjson::kParseErrorStringInvalidEncoding: return JsonParseErrorCode::StringInvalidEncoding;
            case rapidjson::kParseErrorNumberTooBig: return JsonParseErrorCode::NumberTooBig;
            case rapidjson::kParseErrorNumberMissFraction: return JsonParseErrorCode::NumberMissFraction;
            case rapidjson::kParseErrorNumberMissExponent: return JsonParseErrorCode::NumberMissExponent;
            case rapidjson::kParseErrorTermination: return JsonParseErrorCode::Termination;
            case rapidjson::kParseErrorUnspecificSyntaxError: return JsonParseErrorCode::UnspecificSyntaxError;
        }

        return JsonParseErrorCode::None;
    }

    JsonParseError CreateRapidJsonParseError(
        const size_t position,
        const rapidjson::ParseErrorCode parseErrorCode)
    {
        return {
            position,
            CreateRapidJsonErrorCode(parseErrorCode)
        };
    }

}
