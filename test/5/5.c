#include <stdio.h>
int main()
{
	int a[10]={3,2,4,6,8,1,5,7,9,0};
	int x=5;
	for(int i=0;i < 10;i++)
	{
		if(a[i]==x)
			break;
	}
	return 0;
}
