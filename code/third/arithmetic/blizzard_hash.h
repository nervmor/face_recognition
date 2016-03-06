#pragma once


class blizzard_hash
{
public:
	struct crypt_table
	{
		crypt_table()
		{
			unsigned long seed = 0x00100001, index1 = 0, index2 = 0, i;

			for( index1 = 0; index1 < 0x100; index1++ )
			{
				for( index2 = index1, i = 0; i < 5; i++, index2 += 0x100 )
				{
					unsigned long temp1, temp2;
					seed = (seed * 125 + 3) % 0x2AAAAB;
					temp1 = (seed & 0xFFFF) << 0x10;
					seed = (seed * 125 + 3) % 0x2AAAAB;
					temp2 = (seed & 0xFFFF);
					_crypt_table[index2] = ( temp1 | temp2 );
				}
			}
		}
		unsigned int _crypt_table[0x500];
	};
public:
	static unsigned int hash(const unsigned char* data, unsigned int size)
	{
		static crypt_table table;
		const unsigned char *key = data;
		unsigned int seed1 = 0x7FED7FED, seed2 = 0xEEEEEEEE; 
		int ch;

		for (int i = 0; i != size; i++)
		{
			unsigned char c = *key++;
			ch = (int)c;

			seed1 = table._crypt_table[(1 << 8) + ch] ^ (seed1 + seed2); 
			seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3; 
		}
		return seed1; 
	}
};
