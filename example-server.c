#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
//#include "generated-code/test.pb-c.h"
#include "test.pb-c.h"
#include <google/protobuf-c/protobuf-c-rpc.h>
#include "inisrc/iniparser.h"
#include "inisrc/dictionary.h"
//#include "test2.pb-c.h"

static unsigned database_size;
static Foo__Person *database;	/* sorted by name */

static unsigned database2_size;
static Foo__Array *database2;	/* sorted by name */
static void die(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
    exit(1);
}

static void usage(void)
{
    die("usage: example-server [--port=NUM | --unix=PATH] --database=INPUT\n" "\n" "Run a protobuf server as specified by the DirLookup service\n" "in the test.proto file in the protobuf-c distribution.\n" "\n" "Options:\n" "  --port=NUM       Port to listen on for RPC clients.\n" "  --unix=PATH      Unix-domain socket to listen on.\n" "  --database=FILE  data which the server will use to answer requests.\n" "\n" "The database file is a sequence of stanzas, one per person:\n" "\n" "dave\n" " email who@cares.com\n" " mobile (123)123-1234\n" " id 666\n" "\n" "notes:\n" "- each stanza begins with a single unindented line, the person's name.\n");
}

static void *xmalloc(size_t size)
{
    void *rv;
    if (size == 0)
	return NULL;
    rv = malloc(size);
    if (rv == NULL)
	die("out-of-memory allocating %u bytes", (unsigned) size);
    return rv;
}

static void *xrealloc(void *a, size_t size)
{
    void *rv;
    if (size == 0) {
	free(a);
	return NULL;
    }
    if (a == NULL)
	return xmalloc(size);
    rv = realloc(a, size);
    if (rv == NULL)
	die("out-of-memory re-allocating %u bytes", (unsigned) size);
    return rv;
}

static char *xstrdup(const char *str)
{
    if (str == NULL)
	return NULL;
    return strcpy(xmalloc(strlen(str) + 1), str);
}

static char *peek_next_token(char *buf)
{
    while (*buf && !isspace(*buf))
	buf++;
    while (*buf && isspace(*buf))
	buf++;
    return buf;
}

static protobuf_c_boolean is_whitespace(const char *text)
{
    while (*text != 0) {
	if (!isspace(*text))
	    return 0;
	text++;
    }
    return 1;
}

static void chomp_trailing_whitespace(char *buf)
{
    unsigned len = strlen(buf);
    while (len > 0) {
	if (!isspace(buf[len - 1]))
	    break;
	len--;
    }
    buf[len] = 0;
}

static protobuf_c_boolean starts_with(const char *str, const char *prefix)
{
    return memcmp(str, prefix, strlen(prefix)) == 0;
}

static int compare_persons_by_name(const void *a, const void *b)
{
    return strcmp(((const Foo__Person *) a)->name,
		  ((const Foo__Person *) b)->name);
}

static int compare_array_by_name2(const void *a, const void *b)
{
    return strcmp(((const Foo__Array *) a)->name,
		  ((const Foo__Array *) b)->name);
}

static void parse_ini_file(const char *ini_name)
{
    dictionary *ini;
    unsigned n_people = 0;
    unsigned n_array = 0;
	int i;
	int j;
	int cnt;
	int num_array = 0;
	int num_sec;
	int num_sec_bsmsg;
	int array[20];
	char *sec;
	char *s;
	char str[50];
	char *name_fd;
	ini = iniparser_load(ini_name);
    if (ini  == NULL)
		die("error opening %s: %s", ini_name, strerror(errno));
	num_sec = iniparser_getnsec(ini);
	num_sec_bsmsg = iniparser_getsecnkeys(ini, "basemsg");
	n_people = num_sec_bsmsg;
    if (n_people == 0)
		die("empty database: insufficiently interesting to procede");
	Foo__Person *people = xmalloc(sizeof(Foo__Person) * num_sec_bsmsg);
	for(cnt = 0; cnt < num_sec; cnt++)
	{
		
		name_fd = iniparser_getsecname(ini,cnt);
	    if(sec == "basemsg")
		{
			Foo__Person *person;
			person = people + cnt ;
			foo__person__init(person);

			sprintf(str, "%s:name", name_fd);
			s =iniparser_getstring(ini,str,NULL);
			person->name = xstrdup(s);
		
			sprintf(str,"%s:email",name_fd);
			s =iniparser_getstring(ini,str,NULL);
			person->email = xstrdup(s);

			sprintf(str,"%s:id",name_fd);
			i = iniparser_getint(ini,str,-1);
			person->id = i;
		
			sprintf(str,"%s:array_number",name_fd);
			num_array = iniparser_getint(ini, str, -1);
			n_array = num_array;
			person->n_array = num_array;
			person->array = malloc(sizeof(int) * num_array);
			for(j = 0; j < num_array; j++)
			{
				sprintf(str, "%s:array[%d]",name_fd,j);
				i = iniparser_getint(ini, str, -1);
				person->array[j] = i;
			}
		
			Foo__Person__PhoneNumber *pn =
		    	xmalloc(sizeof(Foo__Person__PhoneNumber));
			Foo__Person__PhoneNumber tmp =
		    	FOO__PERSON__PHONE_NUMBER__INIT;
			sprintf(str,"%s:mobile",name_fd);
			s =iniparser_getstring(ini,str,NULL);
			if(s != NULL)
			{
				tmp.has_type = 1;
				tmp.type = FOO__PERSON__PHONE_TYPE__MOBILE;
				tmp.number = xstrdup(s);
				person->phone = xrealloc(person->phone,sizeof(Foo__Person__PhoneNumber *) *\
										(person->n_phone +1));
				*pn = tmp;
				person->phone[person->n_phone++] =pn;
			
			}
			sprintf(str,"%s:home",name_fd);
			s =iniparser_getstring(ini,str,NULL);
			if(s != NULL)
			{
				tmp.has_type = 1;
				tmp.type =FOO__PERSON__PHONE_TYPE__HOME;
				tmp.number = xstrdup(s);
				person->phone = xrealloc(person->phone,sizeof(Foo__Person__PhoneNumber *) *\
										(person->n_phone +1));
				*pn = tmp;
				person->phone[person->n_phone++] =pn;

			}
			sprintf(str,"%s:work",name_fd);
			s =iniparser_getstring(ini,str,NULL);
			if(s != NULL)
			{
				tmp.has_type = 1;
				tmp.type =FOO__PERSON__PHONE_TYPE__WORK;
				tmp.number = xstrdup(s);
				person->phone = xrealloc(person->phone,sizeof(Foo__Person__PhoneNumber *) *\
											(person->n_phone +1));
				*pn = tmp;
				person->phone[person->n_phone++] =pn;
			}
		}
		else
		{
			NULL;	
		}
		}
	iniparser_freedict(ini);

    //qsort(people, n_people, sizeof(Foo__Person), compare_persons_by_name);
    qsort(people, num_sec, sizeof(Foo__Person), compare_persons_by_name);

    database = people;
    database_size = n_people;
}

//static void load_database2(const char *filename)
static void parse_ini_file_array_only(const char *ini_name)
{
	dictionary *ini;
    unsigned n_array = 0;
    unsigned n_number2 = 0;
    int n = 0;
    int i = 0;
	int num_array;
	int num_sec;
	int j = 0;
	int cnt;
	int array_alloced = 32;
	char *s;
	char str[50];
	char name_fd[20];
	int array[20];
	ini = iniparser_load(ini_name);
    Foo__Array *Array ;
	if(ini == NULL)
		die("error inifile for opening %s: %s", ini_name, strerror(errno));
	num_sec = iniparser_getnsec(ini);
	n_array = num_sec;
	if (n_array == array_alloced)
	{
		array_alloced *= 2;
		Array = xrealloc(array, array_alloced * sizeof(Foo__Array));
	}	
	if(n_array ==0)
		die("empty inifile");

    Array = xmalloc(sizeof(Foo__Array) * num_sec);
	for(cnt = 0; cnt < num_sec; cnt++)
	{
		Foo__Array *array;
		array = Array + cnt ;
	    foo__array__init(array);
		sprintf(name_fd,"%s",iniparser_getsecname(ini,cnt));
		array->name = xstrdup(name_fd);

		sprintf(str,"%s:array_number",name_fd);
		num_array = iniparser_getint(ini,str,-1);
	    array->number2 = malloc(sizeof(int) * num_array);
	    array->n_number2 = num_array;
	    n_number2 = num_array;
		for(j = 0;j < num_array; j++)
		{
			sprintf(str,"%s:%s[%d]",name_fd, name_fd, j);
			i = iniparser_getint(ini,str,-1);
			array->number2[j] = i;	
		}
	}
	iniparser_freedict(ini);

    qsort(Array, n_array, sizeof(Foo__Array), compare_array_by_name2);
	/*
  	for ( i=0; i< n_array; i++ )
  	{
		int j;
  		fprintf(stderr,"%s  \n",Array[i].name);
		for ( j=0; j< Array[i].n_number2; j++ ) fprintf(stderr, "%d ", Array[i].number2[j]);
  		fprintf(stderr,"\n");
  	}
	*/
    database2 = Array;
    database2_size = n_array;
}

static int compare_name_to_person(const void *a, const void *b)
{
    return strcmp(a, ((const Foo__Person *) b)->name);
}

static int compare_name_to_array(const void *a, const void *b)
{
    return strcmp(a, ((const Foo__Array *) b)->name);
}

static void
example__by_name(Foo__DirLookup_Service * service,
		 const Foo__Name * name,
		 Foo__LookupResult_Closure closure, void *closure_data)
{
    (void) service;
    if (name == NULL || name->name == NULL)
	closure(NULL, closure_data);
    else {
	Foo__LookupResult result = FOO__LOOKUP_RESULT__INIT;
	Foo__Person *rv = bsearch(name->name, database, database_size,
				  sizeof(Foo__Person),
				  compare_name_to_person);
	if (rv != NULL)
	    result.person = rv;
	closure(&result, closure_data);
    }
}

static void
example__by_name2(Foo__DirLookup_Service * service,
		   const Foo__Name2 * name,
		   Foo__LookupResult2_Closure closure, void *closure_data)
{
    (void) service;
    if (name == NULL || name->name == NULL) 
    {
	closure(NULL, closure_data);
    }
    else
    {
	Foo__LookupResult2 result = FOO__LOOKUP_RESULT2__INIT;
	Foo__Array *rv = bsearch(name->name, database2, database2_size,
				 sizeof(Foo__Array),
				 compare_name_to_array);
	if (rv != NULL) {
	    result.array = rv;
	}
	closure(&result, closure_data);
    }
}

static Foo__DirLookup_Service the_dir_lookup_service =
FOO__DIR_LOOKUP__INIT(example__);

int main(int argc, char **argv)
{
    ProtobufC_RPC_Server *server;
    ProtobufC_RPC_Server *server2;
    ProtobufC_RPC_AddressType address_type = 0;
    const char *name = NULL;
    unsigned i;
    int flagsrv_person = 0;
    int flagsrv_array = 0;
    ProtobufCDispatch *dispatcha;

    for (i = 1; i < (unsigned) argc; i++) {
	if (starts_with(argv[i], "--port=")) 
	{
	    address_type = PROTOBUF_C_RPC_ADDRESS_TCP;
	    name = strchr(argv[i], '=') + 1;
	}
	else if (starts_with(argv[i], "--unix=")) 
	{
	    address_type = PROTOBUF_C_RPC_ADDRESS_LOCAL;
	    name = strchr(argv[i], '=') + 1;
	}
	else if (starts_with(argv[i], "--database="))
	{
	    //load_database(strchr(argv[i], '=') + 1);
		parse_ini_file(strchr(argv[i],'=') + 1);
		flagsrv_person = 1;
	} 
	else if (starts_with(argv[i], "--db2=")) 
	{
	    parse_ini_file_array_only(strchr(argv[i],'=') + 1);
		//load_database2(strchr(argv[i], '=') + 1);
	    flagsrv_array = 1;
	} else
	    usage();
    }

    if (database_size == 0 && database2_size == 0)
	die("missing --database=FILE (try --database=example.database)");
    if (name == NULL)
	die("missing --port=NUM or --unix=PATH");
	server =
	    protobuf_c_rpc_server_new(address_type, name,
				      (ProtobufCService *) &
				      the_dir_lookup_service, dispatcha);
	printf("server  is on\n");
    //}
   // if (flagsrv_array == 1) {
	    //typedef struct _ProtobufC_RPC_Server ProtobufC_RPC_Server;
	    //protobuf_c_rpc_server(address_type, name,
	//  server2 =
	// server2 =
//	ProtobufCService(address_type, name,(ProtobufCService *) &the_dir_lookup_service, dispatchb);
//	printf("server array is on\n");
  //  }

    for (;;)
    {
	protobuf_c_dispatch_run(protobuf_c_dispatch_default());
//	protobuf_c_dispatch_run(dispatcha);
//	protobuf_c_dispatch_run(dispatchb);
    }
    return 0;
}
