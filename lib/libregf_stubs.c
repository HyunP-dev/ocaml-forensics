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
        Store_field(v_result, 0, caml_copy_string(message));
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

    if (libregf_file_open(file, String_val(v_path), LIBREGF_ACCESS_FLAG_READ, &error) != 1)
    {
        char message[BUFSIZ];
        libregf_error_sprint(error, message, BUFSIZ);

        v_result = caml_alloc(1, 1);
        Store_field(v_result, 0, caml_copy_string(message));
        CAMLreturn(v_result);
    }

    v_result = caml_alloc(1, 0);
    Store_field(v_result, 0, caml_copy_nativeint((intnat)file));
    CAMLreturn(v_result);
}

CAMLprim value caml_libregf_file_get_key_by_utf8_path(value v_handle, value v_path)
{
    CAMLparam2(v_handle, v_path);
    CAMLlocal1(v_result);

    libregf_file_t *file = (libregf_file_t *)Nativeint_val(v_handle);
    libregf_key_t *key = NULL;
    libregf_error_t *error = NULL;

    const char *path = String_val(v_path);
    if (libregf_file_get_key_by_utf8_path(file, (const uint8_t *)path, strlen(path), &key, &error) != 1)
    {
        char message[BUFSIZ];
        libregf_error_sprint(error, message, BUFSIZ);

        v_result = caml_alloc(1, 1);
        Store_field(v_result, 0, caml_copy_string(message));
        CAMLreturn(v_result);
    }

    v_result = caml_alloc(1, 0);
    Store_field(v_result, 0, caml_copy_nativeint((intnat)key));
    CAMLreturn(v_result);
}

CAMLprim value caml_libregf_key_get_utf8_name(value v_key)
{
    CAMLparam1(v_key);
    CAMLlocal1(v_result);

    libregf_key_t *key = (libregf_key_t *)Nativeint_val(v_key);
    libregf_error_t *error = NULL;

    size_t name_size = -1;
    if (libregf_key_get_utf8_name_size(key, &name_size, &error) != 1)
    {
        goto error;
    }

    uint8_t *key_name = malloc(name_size);
    if (libregf_key_get_utf8_name(key, key_name, name_size, &error) != 1)
    {
        goto error;
    }

    goto ok;

error:
    char message[BUFSIZ];
    libregf_error_sprint(error, message, BUFSIZ);

    v_result = caml_alloc(1, 1);
    Store_field(v_result, 0, caml_copy_string((const char *)message));
    CAMLreturn(v_result);

ok:
    v_result = caml_alloc(1, 0);
    Store_field(v_result, 0, caml_copy_string((const char *)key_name));
    CAMLreturn(v_result);
}

CAMLprim value caml_get_subkey_names(value v_key)
{
    CAMLparam1(v_key);
    CAMLlocal2(v_result, v_subkey_names);

    libregf_key_t *key = (libregf_key_t *)Nativeint_val(v_key);
    libregf_error_t *error = NULL;

    int number_of_sub_keys;
    if (libregf_key_get_number_of_sub_keys(key, &number_of_sub_keys, &error) != 1)
    {
        goto error;
    }

    {
        v_subkey_names = caml_alloc(number_of_sub_keys, 0);
        for (int i = 0; i < number_of_sub_keys; i++)
        {
            libregf_key_t *subkey = NULL;

            if (libregf_key_get_sub_key_by_index(key, i, &subkey, &error) != 1)
            {
                goto error;
            }

            size_t name_size;
            if (libregf_key_get_utf8_name_size(subkey, &name_size, &error) != 1){
                goto error;
            }

            char* name = malloc(name_size);

            if (libregf_key_get_utf8_name(subkey, (uint8_t*) name, name_size, &error) != 1) {
                goto error;
            }
            Store_field(v_subkey_names, i, caml_copy_string((const char *) name));
        }

        v_result = caml_alloc(1, 0);
        Store_field(v_result, 0, v_subkey_names);
        CAMLreturn(v_result);
    }

error:
    char message[BUFSIZ];
    libregf_error_sprint(error, message, BUFSIZ);

    v_result = caml_alloc(1, 1);
    Store_field(v_result, 0, caml_copy_string((const char *)message));
    CAMLreturn(v_result);
}

CAMLprim value caml_get_value_names(value v_key) {
    CAMLparam1(v_key);
    CAMLlocal2(v_result, v_value_names);

    libregf_key_t *key = (libregf_key_t *)Nativeint_val(v_key);
    libregf_error_t *error = NULL;

    int number_of_values;
    if (libregf_key_get_number_of_values(key, &number_of_values, &error) != 1) {
        goto error;
    }

    {
        v_value_names = CAML_alloc(number_of_values, 0);
        for (int i=0; i<number_of_values; i++) {
            libregf_value_t* value = NULL;
            if (libregf_key_get_value_by_index(key, i, &value, &error) != 1) {
                goto error;
            }

            // TODO: export value names to CAML
        }
    }

error:
    char message[BUFSIZ];
    libregf_error_sprint(error, message, BUFSIZ);

    v_result = caml_alloc(1, 1);
    Store_field(v_result, 0, caml_copy_string((const char *)message));
    CAMLreturn(v_result);
}