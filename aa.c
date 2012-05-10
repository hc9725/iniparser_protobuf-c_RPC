#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int string_to_number_array(char *buf,int *a)
{
int i=0;
char *p;

p=buf;
while(isspace(*p)) p++;
while(*p)
{
	if(a != NULL) if(isdigit(*p)) a[i]=atoi(p);
	while(isdigit(*p++) && isdigit(*p)) p++; 
	i++;
	while(isspace(*p)) p++; 
}
return i;
}

main()
{
char buf[]="  11111   22   3 4 5  ";
int i=0;
int n;
int *a;

n=string_to_number_array(buf,NULL);
a=malloc(n*sizeof(int));
n=string_to_number_array(buf,a);
for(i=0;i<n;i++) printf("%d\n",a[i]);
free(a);

}

