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

#include "Test.hpp"
#include "Curse/System/Exception.hpp"

static void DoThrow()
{
    throw Curse::Exception();
}

static void DoThrow(const char * message)
{
    throw Curse::Exception(message);
}

static void DoThrow(const std::string& message)
{
    throw Curse::Exception(message);
}

namespace Curse
{
    TEST(System, Exception)
    {
        {
            const char cMessage[] = "Char error.";
            const std::string sMessage = "String error.";

            EXPECT_THROW(DoThrow(), Curse::Exception);
            EXPECT_THROW(DoThrow(cMessage), Curse::Exception);
            EXPECT_THROW(DoThrow(sMessage), Curse::Exception);
        }
    }
    TEST(System, Exception_Std)
    {
        {
            const char cMessage[] = "Char error.";
            const std::string sMessage = "String error.";

            {
                bool throwed = false;
                try
                {
                    DoThrow();
                }
                catch (std::exception&)
                {
                    throwed = true;
                }
                EXPECT_TRUE(throwed);
            }
            {
                bool throwed = false;
                try
                {
                    DoThrow(cMessage);
                }
                catch (std::exception&)
                {
                    throwed = true;
                }
                EXPECT_TRUE(throwed);
            }
            {
                bool throwed = false;
                try
                {
                    DoThrow(sMessage);
                }
                catch (std::exception&)
                {
                    throwed = true;
                }
                EXPECT_TRUE(throwed);
            }
            
        }
    }

    TEST(System, Exception_Message)
    {
        {
            bool throwed = false;
            try
            {
                DoThrow();
            }
            catch (Exception & e)
            {
                throwed = true;
                EXPECT_STREQ(e.what(), "");
                EXPECT_STREQ(e.GetMessage().c_str(), "");
            }
            EXPECT_TRUE(throwed);
        }       
        {
            bool throwed = false;
            try
            {
                const char cMessage[] = "Character array error message.";
                DoThrow(cMessage);
            }
            catch (Exception & e)
            {
                throwed = true;
                EXPECT_STREQ(e.what(), "Character array error message.");
                EXPECT_STREQ(e.GetMessage().c_str(), "Character array error message.");
            }
            EXPECT_TRUE(throwed);
        }
        {
            bool throwed = false;
            try
            {
                const std::string sMessage = "String error message.";
                DoThrow(sMessage);
            }
            catch (Exception & e)
            {
                throwed = true;
                EXPECT_STREQ(e.what(), "String error message.");
                EXPECT_STREQ(e.GetMessage().c_str(), "String error message.");
            }
            EXPECT_TRUE(throwed);
        }
    }
}
