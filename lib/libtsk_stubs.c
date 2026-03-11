#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tsk/libtsk.h>

CAMLprim value caml_tsk_img_open(value v_path)
{
    CAMLparam1(v_path);
    CAMLlocal1(v_result);

    char *path = String_val(v_path);
    const char *images[] = {path};

    TSK_IMG_INFO *img = tsk_img_open(1, (const TSK_TCHAR **)images, TSK_IMG_TYPE_DETECT, 0);

    if (img == NULL)
    {
        v_result = caml_alloc(1, 1);
        Store_field(v_result, 0, caml_copy_string(tsk_error_get()));
        CAMLreturn(v_result);
    }

    v_result = caml_alloc(1, 0);
    Store_field(v_result, 0, caml_copy_nativeint((intnat)img));
    CAMLreturn(v_result);
}

CAMLprim value caml_tsk_fs_open_img(value v_tsk_img_info, value v_offset)
{
    CAMLparam1(v_tsk_img_info);
    CAMLlocal1(v_result);

    TSK_OFF_T offset = Int64_val(v_offset);
    TSK_IMG_INFO *img = (TSK_IMG_INFO *)Nativeint_val(v_tsk_img_info);
    TSK_FS_INFO *fs;

    fs = tsk_fs_open_img(img, offset * img->sector_size, TSK_FS_TYPE_DETECT);
    if (fs == NULL)
    {
        char *message = tsk_error_get();
        img->close(img);

        v_result = caml_alloc(1, 1);
        Store_field(v_result, 0, caml_copy_string(message));
        CAMLreturn(v_result);
    }

    v_result = caml_alloc(1, 0);
    Store_field(v_result, 0, caml_copy_nativeint((intnat)fs));
    CAMLreturn(v_result);
}

CAMLprim value caml_tsk_fs_file_open(value v_fs, value v_path)
{
    CAMLparam2(v_fs, v_path);
    CAMLlocal1(v_result);

    TSK_FS_INFO *fs = (TSK_FS_INFO *)Nativeint_val(v_fs);
    char *path = String_val(v_path);

    TSK_FS_FILE *file = tsk_fs_file_open(fs, NULL, path);
    if (file == NULL)
    {
        char *message = tsk_error_get();
        v_result = caml_alloc(1, 1);
        Store_field(v_result, 0, caml_copy_string(message));
        CAMLreturn(v_result);
    }

    v_result = caml_alloc(1, 0);
    Store_field(v_result, 0, caml_copy_nativeint((intnat)file));
    CAMLreturn(v_result);
}

CAMLprim value caml_tsk_fs_file_read(value v_offset, value v_size, value v_file)
{
    CAMLparam3(v_offset, v_size, v_file);
    CAMLlocal1(v_raw);

    TSK_FS_FILE *file = (TSK_FS_FILE *)Nativeint_val(v_file);
    TSK_OFF_T offset = Int64_val(v_offset);
    size_t size = MIN(Int64_val(v_size), file->meta->size);
    uint8_t *buffer = malloc(size);

    tsk_fs_file_read(file, offset, buffer, size, TSK_FS_FILE_READ_FLAG_NONE);
    v_raw = caml_alloc_string(size);
    memcpy((void *)Bytes_val(v_raw), (const void *)buffer, size);
    CAMLreturn(v_raw);
}
