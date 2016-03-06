#pragma once

#include <boost/locale.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>



// Convert Unicode big endian to Unicode little endian
__inline unsigned Ucs2BeToUcs2Le(unsigned short *ucs2bige, unsigned int size)
{
    if (!ucs2bige) {
        return 0;
    }
    
    unsigned int length = size;
    unsigned short *tmp = ucs2bige;
    
    while (*tmp && length) {
        
        length--;
        unsigned char val_high = *tmp >> 8;
        unsigned char val_low = (unsigned char)*tmp;
        
        *tmp = val_low << 8 | val_high;
        
        tmp++;
    }
    
    return size - length;
}

// Convert Ucs-2 to Utf-8
__inline unsigned int Ucs2ToUtf8(unsigned short *ucs2, unsigned int ucs2_size, 
        unsigned char *utf8, unsigned int utf8_size)
{
    unsigned int length = 0;
    
    if (!ucs2) {
        return 0;
    }
    
    unsigned short *inbuf = ucs2;
    unsigned char *outbuf = utf8;
    
    if (*inbuf == 0xFFFE) {
        Ucs2BeToUcs2Le(inbuf, ucs2_size);
    }
    
    if (!utf8) {
        unsigned int insize = ucs2_size;
        
        while (*inbuf && insize) {
            insize--;
            
/*            if (*inbuf == 0xFEFF) {
                inbuf++;
                continue;
            }*/
            
            if (0x0080 > *inbuf) {
                length++;
            } else if (0x0800 > *inbuf) {
                length += 2;                
            } else {
                length += 3;
            }
            
            inbuf++;
        }
        return length;
        
    } else {        
        unsigned int insize = ucs2_size;
        
        while (*inbuf && insize && length < utf8_size) {            
            insize--;
            
            if (*inbuf == 0xFFFE) {
                inbuf++;
                continue;
            }
            
            if (0x0080 > *inbuf) {
                /* 1 byte UTF-8 Character.*/
                *outbuf++ = (unsigned char)(*inbuf);
                length++;
            } else if (0x0800 > *inbuf) {
                /*2 bytes UTF-8 Character.*/
                *outbuf++ = 0xc0 | ((unsigned char)(*inbuf >> 6));
                *outbuf++ = 0x80 | ((unsigned char)(*inbuf & 0x3F));
                length += 2;

            } else {
                /* 3 bytes UTF-8 Character .*/
                *outbuf++ = 0xE0 | ((unsigned char)(*inbuf >> 12));
                *outbuf++ = 0x80 | ((unsigned char)((*inbuf >> 6) & 0x3F));
                *outbuf++ = 0x80 | ((unsigned char)(*inbuf & 0x3F));
                length += 3; 
            }
            
            inbuf++;
        }
        
        return length;
    }
}

// Convert Utf-8 to Ucs-2 
__inline unsigned int Utf8ToUcs2(unsigned char *utf8, unsigned int utf8_size, 
        unsigned short *ucs2, unsigned int ucs2_size)
{
    int length = 0;
    unsigned int insize = utf8_size;
    unsigned char *inbuf = utf8;

    if(!utf8)
        return 0;

    if(!ucs2) {
        while(*inbuf && insize) {
            unsigned char c = *inbuf;
            if((c & 0x80) == 0) {
                length += 1;
                insize -= 1;
                inbuf++;
            }
            else if((c & 0xE0) == 0xC0) {
                length += 1;
                insize -= 2;
                inbuf += 2;
            } else if((c & 0xF0) == 0xE0) {
                length += 1;
                insize -= 3;
                inbuf += 3;
            }
        }
        return length;

    } else {
        unsigned short *outbuf = ucs2;
        unsigned int outsize = ucs2_size;

        while(*inbuf && insize && length < outsize) {
            unsigned char c = *inbuf;
            if((c & 0x80) == 0) {
                *outbuf++ = c;
                inbuf++;
                length++;
                insize--;
            } else if((c & 0xE0) == 0xC0) {
                unsigned short val;

                val = (c & 0x3F) << 6;
                inbuf++;
                c = *inbuf;
                val |= (c & 0x3F);
                inbuf++;

                length++;
                insize -= 2;

                *outbuf++ = val;
            } else if((c & 0xF0) == 0xE0) {
                unsigned short val;

                val = (c & 0x1F) << 12;
                inbuf++;
                c = *inbuf;
                val |= (c & 0x3F) << 6;
                inbuf++;
                c = *inbuf;
                val |= (c & 0x3F);
                inbuf++;

                insize -= 3;
                length++;

                *outbuf++ = val;
            }
        }
        return length;
    }
    return 0;
}

class util_string
{
public:
	static std::string w2a(const std::wstring& str_w)
	{
		return boost::locale::conv::from_utf(str_w, "GBK");
	}
	static std::wstring a2w(const std::string& str_a)
	{
		return boost::locale::conv::to_utf<wchar_t>(str_a, "GBK");
	}
	static std::string w2a(const wchar_t* sz_w)
	{
		return w2a(std::wstring(sz_w));
	}
	static std::wstring a2w(const char* sz_a)
	{
		return a2w(std::string(sz_a));
	}
	static int a2i(const std::string& str_a)
	{
		std::string str_secuirty_num = str_a;
		for (int i = 0; i != str_a.size(); i++)
		{
			if (str_a[i] < '0' || str_a[i] > '9')
			{
				str_secuirty_num = str_a.substr(0, i);
				break;
			}
		}
		return boost::lexical_cast<int>(str_secuirty_num.c_str());	
	}
	static int a2i(const char* sz_a)
	{
		return boost::lexical_cast<int>(std::string(sz_a));
	}
	static int w2i(const std::wstring& str_w)
	{
		return a2i(w2a(str_w));
	}
	static int w2i(const wchar_t* sz_w)
	{
		return a2i(w2a(sz_w));
	}
	static double a2d(const std::string& str_a)
	{
		return boost::lexical_cast<double>(str_a.c_str());
	}
	static double a2d(const char* sz_a)
	{
		return boost::lexical_cast<double>(sz_a);
	}
	static double w2d(const std::wstring& str_w)
	{
		return a2d(w2a(str_w));
	}
	static double w2d(const wchar_t* sz_w)
	{
		return a2d(w2a(sz_w));
	}
	static std::wstring i2w(int i)
	{
		return boost::lexical_cast<std::wstring>(i);
	}
	static std::string i2a(int i)
	{
		return boost::lexical_cast<std::string>(i);
	}
	static std::string d2a(double d)
	{
		return boost::lexical_cast<std::string>(d);
	}
	static std::wstring low(const std::wstring& str_w)
	{
		return boost::to_lower_copy(str_w);
	}

	static std::wstring usc2_to_utf8(wchar_t* ucs2_code, unsigned int len)
	{
		char* utf8 = new char[len * 4];
		memset(utf8, 0, len * 4);
		Ucs2ToUtf8((unsigned short*)ucs2_code, len/2 , (unsigned char*)utf8, len * 4);
		std::string str_utf = std::string(utf8);
		delete utf8;
		std::string str_a = boost::locale::conv::from_utf(str_utf, "GBK");
		return a2w(str_a);
	}
	static bool is_ascii(const char* sz_a)
	{
		unsigned int len = strlen(sz_a);
		for (int i = 0; i != len; i++)
		{
			unsigned int n = (unsigned int)sz_a[i];
			if (n >127)
			{
				return false;
			}
		}
		return true;
	}
	static void replace_all(std::wstring& str, const std::wstring& be_replace, const std::wstring& to_replace)
	{
		int pos = str.find(be_replace);
		while (pos != std::wstring::npos)
		{
			str.replace(pos, to_replace.size(), to_replace);
			pos = str.find(be_replace);
		}
	}
};