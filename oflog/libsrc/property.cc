// Module:  Log4CPLUS
// File:    property.cxx
// Created: 2/2002
// Author:  Tad E. Smith
//
//
// Copyright (C) Tad E. Smith  All rights reserved.
//
// This software is published under the terms of the Apache Software
// License version 1.1, a copy of which has been included with this
// distribution in the LICENSE.APL file.
//

#define INCLUDE_CSTRING
#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/oflog/helpers/property.h"
#include "dcmtk/oflog/fstreams.h"


namespace log4cplus
{


const tchar helpers::Properties::PROPERTIES_COMMENT_CHAR = LOG4CPLUS_TEXT('#');


namespace
{


static
int
is_space (tchar ch)
{
    return STD_NAMESPACE isspace (ch);
}


static
void
trim_leading_ws (tstring & str)
{
    tstring::iterator it = str.begin ();
    for (; it != str.end (); ++it)
    {
        if (! is_space (*it))
            break;
    }
    str.erase (0, it - str.begin());
}


static
void
trim_trailing_ws (tstring & str)
{
    tstring::iterator it = str.end();
    for (; it != str.begin(); --it)
    {
        if (! is_space (*(it - 1)))
            break;
    }
    str.resize(it - str.begin());
}


static
void
trim_ws (tstring & str)
{
    trim_trailing_ws (str);
    trim_leading_ws (str);
}


} // namespace



namespace helpers
{

///////////////////////////////////////////////////////////////////////////////
// Properties ctors and dtor
///////////////////////////////////////////////////////////////////////////////

Properties::Properties()
{
}



Properties::Properties(tistream& input)
{
    init(input);
}



Properties::Properties(const tstring& inputFile)
{
    if (inputFile.length() == 0)
        return;

    tifstream file (LOG4CPLUS_TSTRING_TO_STRING(inputFile).c_str());
    init(file);
}



void
Properties::init(tistream& input)
{
    if (! input)
        return;

    // FIXME
    //tstring buffer;
    STD_NAMESPACE string _buffer;
    while (STD_NAMESPACE getline (input, _buffer))
    {
        tstring buffer(_buffer.c_str());
        trim_leading_ws (buffer);

        // Check if we have a trailing \r because we are
        // reading a properties file produced on Windows.
        tstring::size_type const buffLen = buffer.size ();

        if (buffLen > 0 && buffer[0] == PROPERTIES_COMMENT_CHAR)
            continue;

        if (buffLen > 0 && buffer[buffLen-1] == LOG4CPLUS_TEXT('\r'))
            // Remove trailing 'Windows' \r.
            buffer.resize (buffLen - 1);
        tstring::size_type const idx = buffer.find('=');
        if (idx != OFString_npos)
        {
            tstring key = buffer.substr(0, idx);
            tstring value = buffer.substr(idx + 1);
            trim_trailing_ws (key);
            trim_ws (value);
            setProperty(key, value);
        }
    }
}



Properties::~Properties()
{
}



///////////////////////////////////////////////////////////////////////////////
// Properties public methods
///////////////////////////////////////////////////////////////////////////////

tstring
Properties::getProperty(const tstring& key) const
{
    StringMap::const_iterator it (data.find(key));
    if (it == data.end())
        return LOG4CPLUS_TEXT("");
    else
        return it->second;
}



tstring
Properties::getProperty(const tstring& key,
                                 const tstring& defaultVal) const
{
    StringMap::const_iterator it (data.find (key));
    if (it == data.end ())
        return defaultVal;
    else
        return it->second;
}


OFList<tstring>
Properties::propertyNames() const
{
    OFList<tstring> tmp;
    for (StringMap::const_iterator it=data.begin(); it!=data.end(); ++it)
        tmp.push_back(it->first);

    return tmp;
}



void
Properties::setProperty(const tstring& key, const tstring& value)
{
    data[key] = value;
}


bool
Properties::removeProperty(const tstring& key)
{
    return (data.erase(key) > 0);
}


Properties
Properties::getPropertySubset(
    const tstring& prefix) const
{
    Properties ret;
    OFList<tstring> const keys = propertyNames();
    size_t const prefix_len = prefix.size ();
    for (OFListConstIterator(tstring) it = keys.begin();
        it != keys.end(); ++it)
    {
        int result = it->compare (0, prefix_len, prefix);
        if (result == 0)
            ret.setProperty (it->substr (prefix_len), getProperty(*it));
    }

    return ret;
}

} // namespace helpers

} // namespace log4cplus
