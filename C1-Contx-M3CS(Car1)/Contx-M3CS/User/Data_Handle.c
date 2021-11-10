#include "Data_Handle.h"

int n,m;

void Data_Replace (char *Source,char *Target,char Data,char Length)
{
	for(n = 0;n < Length;n++)
	{
		if(Data == 'F')
		{
			*(Target + n) = *(Source + n);
		}
		else 
		{
			*(Target + n) = Data;
		}
	}
}
