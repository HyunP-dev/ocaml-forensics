#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>

#include <stdlib.h>
#include <string.h>

#include <libewf.h>

CAMLprim value caml_libewf_get_version()
{
    CAMLparam0();
    value v_result = caml_copy_string(libewf_get_version());
    CAMLreturn(v_result);
}

CAMLprim value caml_open_ewf_image(value v_paths)
{
    CAMLparam1(v_paths);

    int n = Wosize_val(v_paths);
    char **paths = malloc(sizeof(char *) * (n + 1));
    for (int i = 0; i < n; i++)
    {
        paths[i] = String_val(Field(v_paths, i));
    }
    paths[n] = NULL;

    libewf_error_t *error = NULL;
    libewf_handle_t *handle = NULL;

    if (libewf_handle_initialize(&handle, &error) != 1)
    {
        CAMLreturn(Val_none);
    }

    if (libewf_handle_open(handle, paths, 1, LIBEWF_OPEN_READ, &error) != 1)
    {
        CAMLreturn(Val_none);
    }

    CAMLreturn(caml_copy_nativeint((intnat)handle));
}

CAMLprim value caml_read_bytes_from_ewf(value v_handle, value v_offset, value v_size)
{
    CAMLparam1(v_handle);
    CAMLlocal1(v_raw);

    libewf_handle_t *handle = (libewf_handle_t *)Nativeint_val(v_handle);
    off64_t offset = Int64_val(v_offset);
    size_t size = Int64_val(v_size);

    uint8_t *buffer = malloc(size);

    libewf_error_t *error = NULL;
    if (libewf_handle_read_random(handle, buffer, size, offset, &error) < 0)
    {
        CAMLreturn(Val_none);
    }

    v_raw = caml_alloc_string(size);
    memcpy((void *)Bytes_val(v_raw), (const void *)buffer, size);

    CAMLreturn(v_raw);
}
