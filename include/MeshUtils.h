#pragma once
#include <iostream>


struct FaceIdcs
{
	unsigned short v[4];
	unsigned short vn[4];
	unsigned short vt[4];

	FaceIdcs()
	{
		for (int i = 0; i < 4; i++)
			v[i] = vn[i] = vt[i] = 0;
	}

	FaceIdcs(std::istream& a_stream)
	{
		for (int i = 0; i < 4; ++i)
			v[i] = vn[i] = vt[i] = 0;

		char c; // For reading the '/' delimiter
		for (int i = 0; i < 3; ++i)
		{
			a_stream >> std::ws >> v[i]; // Read vertex index

			if (a_stream.peek() == '/')
			{
				a_stream >> c; // Consume '/'

				if (a_stream.peek() == '/') // Case: v//vn
				{
					a_stream >> c >> vn[i]; // Consume second '/' and read vn
				}
				else // Case: v/vt or v/vt/vn
				{
					a_stream >> vt[i]; // Read vt

					if (a_stream.peek() == '/')
					{
						a_stream >> c >> vn[i]; // Consume '/' and read vn
					}
				}
			}
		}
	}
};