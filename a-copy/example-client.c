#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
//#include "generated-code/test.pb-c.h"
#include "test.pb-c.h"
#include <google/protobuf-c/protobuf-c-rpc.h>
//#include "test2.pb-c.h"
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
    die("usage: example-client [--tcp=HOST:PORT | --unix=PATH]\n"
	"\n"
	"Run a protobuf client as specified by the DirLookup service\n"
	"in the test.proto file in the protobuf-c distribution.\n"
	"\n"
	"Options:\n"
	"  --tcp=HOST:PORT  Port to listen on for RPC clients.\n"
	"  --unix=PATH      Unix-domain socket to listen on.\n");
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

static void
handle_query_response(const Foo__LookupResult * result, void *closure_data)
{
    if (result == NULL)
	printf("Error processing request.\n");
    else if (result->person == NULL)
	printf("Not found.\n");
    else {
	Foo__Person *person = result->person;
	unsigned i;
	printf("%s\n" " %u\n", person->name, person->id);
	if (person->email)
	    printf(" %s\n", person->email);
	if (person->arry)
	    printf(" %s\n", person->arry);
	for (i = 0; i < person->n_phone; i++) {
	    const ProtobufCEnumValue *ev;
	    ev = protobuf_c_enum_descriptor_get_value
		(&foo__person__phone_type__descriptor,
		 person->phone[i]->type);
	    printf(" %s %s\n", ev ? ev->name : "???",
		   person->phone[i]->number);
	}
    }

    *(protobuf_c_boolean *) closure_data = 1;
}

static void handle_query_response2(const Foo__LookupResult2 * result,
				   void *closure_data)
{
    	if (result == NULL) {
		printf("Error processing request.\n");
	    }
	 else if (result->array == NULL)
	 {
		printf("not found.\n");
    	  }
    	else {
		Foo__Array *Array = result->array;
		unsigned i;
		printf("name: {%s}", Array->name);
		for (i = 0; i < Array->n_number2; i++) 
		{
	    		 printf(" %d", Array->number2[i]);
		}
		printf("\n");
    }
    *(protobuf_c_boolean *) closure_data = 1;
}

int main(int argc, char **argv)
{
    ProtobufCService *service;
    ProtobufCService *service2;
    ProtobufC_RPC_Client *client;
    ProtobufC_RPC_Client *client2;
    ProtobufC_RPC_AddressType address_type = 0;
    ProtobufC_RPC_AddressType address_type2 = 0;
    const char *name = NULL;
    unsigned i;

    for (i = 1; i < (unsigned) argc; i++) {
	if (starts_with(argv[i], "--tcp=")) {
	    address_type = PROTOBUF_C_RPC_ADDRESS_TCP;
	    name = strchr(argv[i], '=') + 1;
	} else if (starts_with(argv[i], "--unix=")) {
	    address_type2 = PROTOBUF_C_RPC_ADDRESS_LOCAL;
	    name = strchr(argv[i], '=') + 1;
	} else
	    usage();
    }
	
	
	if (name == NULL )
	    die("missing --tcp=HOST:PORT or --unix=PATH");
	service =
	    protobuf_c_rpc_client_new(address_type, name,
				      &foo__dir_lookup__descriptor, NULL);

	if (service == NULL)
	    die("error creating client");
	else
	{
		client = (ProtobufC_RPC_Client *) service;
		fprintf(stderr, "Connecting... ");
		while (!protobuf_c_rpc_client_is_connected(client))
	    		protobuf_c_dispatch_run(protobuf_c_dispatch_default());
		fprintf(stderr, "done.\n");
	}

    int choice = 1;
    char buf[1024];
    char *person, *show_arr;
    for(;;)
    {
    	fprintf(stderr, ">> ");
    	//	fgets(buf, sizeof(buf), stdin)
	if (gets(buf) == NULL)
		break;
    	if (starts_with(buf, "name")) 
    	{
		choice =1;
	    	person = strchr(buf, '=') + 1;
	    	if (is_whitespace(person))
			continue;
	    	chomp_trailing_whitespace(person);
	}
	if(starts_with(buf,"array"))
	{
		choice =0;	
	    	show_arr = strchr(buf, '=') + 1;
	    	if (is_whitespace(show_arr))
			continue;
	    	chomp_trailing_whitespace(show_arr);
	}
	if(choice == 1)
	{
	    choice = -1;
	    Foo__Name query = FOO__NAME__INIT;
	    protobuf_c_boolean is_done = 0;
	    query.name = person;
	    printf("person = %s\n",person);
	    foo__dir_lookup__by_name(service, &query,
				     handle_query_response, &is_done);
	    while (!is_done)
		protobuf_c_dispatch_run(protobuf_c_dispatch_default());
  	 } 
	else if (choice == 0)
	 {
		choice = -1;
	 	Foo__Name2 query = FOO__NAME2__INIT;
	    	protobuf_c_boolean is_done = 0;
		query.name = show_arr;
		printf("show_arr name = %s\n",show_arr);
	    	foo__dir_lookup__by_name2(service, &query,
				       handle_query_response2, &is_done);
	    	while (!is_done) 
	    	{
			protobuf_c_dispatch_run(protobuf_c_dispatch_default());
			//protobuf_c_dispatch_run(protobuf_c_dispatch_default())        
	    	}
	 }
	 else
	 {
		printf("input wrong!\n");	 
	}
    }
    return 0;
}
