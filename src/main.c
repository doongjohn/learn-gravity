#include <stdio.h>
#include <math.h>

// gravity api
#include "gravity_compiler.h"
#include "gravity_macros.h"
#include "gravity_core.h"
#include "gravity_vm.h"


// read file
char* read_file(char *path, size_t *length) {
	FILE *fp;
	char *buffer;

	fp = fopen(path, "r");
	//               ^^^ --> read mode
	if (!fp) {
		perror(path);
		exit(1);
	}

	// get file size
	fseek(fp, 0L, SEEK_END);
	size_t size = *length = ftell(fp);

	// alloc buffer
	buffer = calloc(1, size + 1);
	if (!buffer) {
		fclose(fp);
		perror("alloc failed: ");
		exit(1);
	}

	// read file and check error
	rewind(fp);
	if (size > fread(buffer, sizeof(char), size+1, fp)) {
		fclose(fp);
		free(buffer);
		fputs("couldn't read the entire file\n", stderr);
		exit(1);
	}

	fclose(fp);
	return buffer;
}

// error callback
static void report_error(
		gravity_vm *vm,
		error_type_t error_type,
		const char *message,
		error_desc_t error_desc,
		void *xdata
	) {
	#pragma unused(vm, xdata)

	const char *type = "N/A";
	switch (error_type) {
		case GRAVITY_ERROR_NONE:
			type = "NONE";
			break;
		case GRAVITY_ERROR_SYNTAX:
			type = "SYNTAX";
			break;
		case GRAVITY_ERROR_SEMANTIC:
			type = "SEMANTIC";
			break;
		case GRAVITY_ERROR_RUNTIME:
			type = "RUNTIME";
			break;
		case GRAVITY_WARNING:
			type = "WARNING";
			break;
		case GRAVITY_ERROR_IO:
			type = "I/O";
			break;
	}

	if (error_type == GRAVITY_ERROR_RUNTIME)
		printf("RUNTIME ERROR: ");
	else
		printf("%s ERROR on %d (%d,%d): ", type, error_desc.fileid, error_desc.lineno, error_desc.colno);
	printf("%s\n", message);
}


// CMath functions

static bool math_pi(
		gravity_vm *vm,
		gravity_value_t *args,
		uint16_t nargs,
		uint32_t rindex
	) {
	#pragma unused (args, nargs)
	gravity_vm_setslot(vm, VALUE_FROM_FLOAT(3.141593f), rindex);
	return true;
}

static bool math_log(
		gravity_vm *vm,
		gravity_value_t *args,
		uint16_t nargs,
		uint32_t rindex
	) {
	// missing parameters check here
	// 1. number of args
	// 2. args type

	// assuming arg of type float (in a real example there should be a conversion if not float)
	gravity_float_t n = VALUE_AS_FLOAT(args[1]);

	// gravity can be compiled with FLOAT as 32 or 64 bit
#if GRAVITY_ENABLE_DOUBLE
	gravity_float_t result = (gravity_float_t)log(n);
#else
	gravity_float_t result = (gravity_float_t)logf(n);
#endif

	gravity_vm_setslot(vm, VALUE_FROM_FLOAT(result), rindex);
	return true;
}

static bool math_pow(
		gravity_vm *vm,
		gravity_value_t *args,
		uint16_t nargs,
		uint32_t rindex
	) {
	// missing parameters check here
	// 1. number of args
	// 2. args type

	// assuming arg 1 of type float (in a real example there should be a conversion if not float)
	gravity_float_t n1 = VALUE_AS_FLOAT(args[1]);

	// assuming arg 2 of type float (in a real example there should be a conversion if not float)
	gravity_float_t n2 = VALUE_AS_FLOAT(args[2]);

	double result = pow((double)n1, (double)n2);

	gravity_vm_setslot(vm, VALUE_FROM_FLOAT((gravity_float_t)result), rindex);
	return true;
}


static void create_class_cmath(gravity_vm *vm) {
	// setup class name
	static const char *CLASS_NAME = "CMath";

	// create a new class (a pair of classes since we are creating a class and its meta-class)
	gravity_class_t *c = gravity_class_new_pair(NULL, CLASS_NAME, NULL, 0, 0);

	// we want to register properties and methods callback to its meta-class
	// so user can access Math.property and Math.method without the need to instantiate it

	// get its meta-class
	gravity_class_t *meta = gravity_class_get_meta(c);

	// start binding methods and properties (special methods) to the meta class

	// *** LOG METHOD ***
	// 1. create a gravity_function_t from the c function
	gravity_function_t *logf = gravity_function_new_internal(NULL, NULL, math_log, 0);

	// 2. create a closure from the gravity_function_t
	gravity_closure_t *logc = gravity_closure_new(NULL, logf);

	// 3. bind closure VALUE to meta class
	gravity_class_bind(meta, "log", VALUE_FROM_OBJECT(logc));

	// *** POW METHOD ***
	// 1. create a gravity_function_t from the c function
	gravity_function_t *powf = gravity_function_new_internal(NULL, NULL, math_pow, 0);

	// 2. create a closure from the gravity_function_t
	gravity_closure_t *powc = gravity_closure_new(NULL, powf);

	// 3. bind closure VALUE to meta class
	gravity_class_bind(meta, "pow", VALUE_FROM_OBJECT(powc));

	// *** PI PROPERTY (getter only) ***
	// 1. create a gravity_function_t from the c function
	gravity_function_t *pif = gravity_function_new_internal(NULL, NULL, math_pi, 0);

	// 2. create a closure from the gravity_function_t
	gravity_closure_t *pi_getter = gravity_closure_new(NULL, pif);

	// 3. create a new special function to represents getter and setter (NULL in this case)
	gravity_function_t *f = gravity_function_new_special(vm, NULL, GRAVITY_COMPUTED_INDEX, pi_getter, NULL);

	// 4. create a closure for the special function
	gravity_closure_t *closure_property = gravity_closure_new(NULL, f);

	// 5. bind closure VALUE to meta class
	gravity_class_bind(meta, "pi", VALUE_FROM_OBJECT(closure_property));

	// LAST STEP
	// register newly defined C class into Gravity VM
	gravity_vm_setvalue(vm, CLASS_NAME, VALUE_FROM_OBJECT(c));
}


inline
static const char** get_optional_classes() {
	// these classes will not be exported automatically
	static const char *list[] = {
		"CMath",
		NULL // why is this necessary???
	};
	return list;
}


typedef struct txtfile_t {
	size_t len;
	char *txt;
} TxtFile;

TxtFile read_txtfile(char *file_path) {
	TxtFile result = { .len = 0, .txt = "" };
	result.txt = read_file(file_path, &result.len);
	return result;
}


int main(void) {
	// read gravity source file
	TxtFile source = read_txtfile("./src/test.gr");

	// setup a delegate
	gravity_delegate_t delegate = {
		.error_callback = report_error,
		// .optional_classes = get_optional_classes
	};

	// compile source into a closure
	gravity_compiler_t *compiler = gravity_compiler_create(&delegate);
	gravity_closure_t *closure = gravity_compiler_run(compiler, source.txt, source.len, 0, true, true);
	free(source.txt);
	if (!closure) {
		printf("failed to create gravity closure\n");
		exit(1);
	}

	// setup a new VM and a new fiber
	gravity_vm *vm = gravity_vm_new(&delegate);
	if (!vm) {
		printf("failed to create gravity vm\n");
		exit(1);
	}

	// transfer memory from compiler to VM and then free compiler
	gravity_compiler_transfer(compiler, vm);
	gravity_compiler_free(compiler);

	// create a new math class with methods and properties and register it to the VM
	create_class_cmath(vm);

	// CMath class is now available from Gravity code so we can start excuting previously compiled closure
	if (gravity_vm_runmain(vm, closure)) {
		// how to check `gravity_value_t`
		// https://github.com/marcobambini/gravity/blob/3f63ef231c1f6c8d8446c9651e09ca023fc41cdf/src/shared/gravity_value.c#L2057
		gravity_value_t result = gravity_vm_result(vm);
		double t = gravity_vm_time(vm);
		printf("\n");
		printf("(GRAVITY VM) EXIT CODE: %ld\n", result.n);
		printf("(GRAVITY VM) TIME TOOK: %.6f ms\n", t);
	}

	// our Math C class was not exposed to the GC (we passed NULL as vm parameter) 
	// so we would need to manually free it here
	// free class and its methods here

	// free vm and base classes
	gravity_vm_free(vm);
	gravity_core_free();

	// exit app
	return 0;
}
