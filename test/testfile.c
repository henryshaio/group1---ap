#include <stdio.h>
#include <string.h>
int main()
{
	char buf[1000];
	FILE * fp_read = fopen("testfedit.txt", "r");
	FILE * fp_write = fopen("testfedit.txt", "r+");
	char test[1000] = "IM a pig";
	snprintf(test, sizeof(test), "%s yes\n", test);
	printf("%s", test);
	if(fp_read && fp_write)
	{
		while(fgets(buf, sizeof(buf), fp_read))
		{
			int len = strlen(buf);
			buf[len-1] = 0;
			//strncat(buf, "Remember me\n", strlen("Remember me\n"));
			fprintf(fp_write, "%s", buf);
		}
	}
}
