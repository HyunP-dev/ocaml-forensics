#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>

#include <stdlib.h>
#include <string.h>

#include <libregf.h>

CAMLprim value caml_libregf_file_initialize()
{
    CAMLparam0();
    CAMLlocal1(v_result);

    libregf_file_t *file = NULL;
    libregf_error_t *error = NULL;

    if (libregf_file_initialize(&file, &error) != 1)
    {
        char message[BUFSIZ];
        libregf_error_sprint(error, message, BUFSIZ);

        v_result = caml_alloc(1, 1);
        Store_field(v_result, 0, String_val(message));
        CAMLreturn(v_result);
    }

    v_result = caml_alloc(1, 0);
    Store_field(v_result, 0, caml_copy_nativeint((intnat)file));
    CAMLreturn(v_result);
}

CAMLprim value caml_libregf_file_open(value v_handle, value v_path)
{
    CAMLparam2(v_handle, v_path);
    CAMLlocal1(v_result);

    libregf_file_t *file = (libregf_file_t *)Nativeint_val(v_handle);
    libregf_error_t *error = NULL;

    if (libregf_file_open(file, String_val(v_path), LIBREGF_ACCESS_FLAG_READ, error) != 1)
    {
        char message[BUFSIZ];
        libregf_error_sprint(error, message, BUFSIZ);

        v_result = caml_alloc(1, 1);
        Store_field(v_result, 0, String_val(message));
        CAMLreturn(v_result);
    }

    v_result = caml_alloc(1, 0);
    Store_field(v_result, 0, caml_copy_nativeint((intnat)file));
    CAMLreturn(v_result);
}
