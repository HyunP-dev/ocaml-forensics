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

    const char *path = String_val(v_path);
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
        const char *message = tsk_error_get();
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
    const char *path = String_val(v_path);

    TSK_FS_FILE *file = tsk_fs_file_open(fs, NULL, path);
    if (file == NULL)
    {
        const char *message = tsk_error_get();
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
    int64_t a_size = Int64_val(v_size);

    size_t size = a_size >= 0 ? MIN(a_size, file->meta->size) : file->meta->size;
    uint8_t *buffer = malloc(size);

    tsk_fs_file_read(file, offset, (char *)buffer, size, TSK_FS_FILE_READ_FLAG_NONE);
    v_raw = caml_alloc_string(size);
    memcpy((void *)Bytes_val(v_raw), (const void *)buffer, size);
    CAMLreturn(v_raw);
}

CAMLprim value caml_get_partitions(value v_tsk_img_info)
{
    CAMLparam1(v_tsk_img_info);
    CAMLlocal3(v_result, v_parts, v_part);

    TSK_IMG_INFO *img_info = (TSK_IMG_INFO *)Nativeint_val(v_tsk_img_info);
    TSK_VS_INFO *vs_info = tsk_vs_open(img_info, 0, TSK_VS_TYPE_DETECT);
    if (vs_info == NULL)
        goto error;

    v_parts = caml_alloc(vs_info->part_count, 0);
    for (int i = 0; i < vs_info->part_count; i++)
    {
        const TSK_VS_PART_INFO *part = tsk_vs_part_get(vs_info, i);

        if (part == NULL)
            break;

        v_part = caml_alloc(4, 0);
        Store_field(v_part, 0, caml_copy_int64(part->addr));
        Store_field(v_part, 1, caml_copy_int64(part->start));
        Store_field(v_part, 2, caml_copy_int64(part->len));
        Store_field(v_part, 3, caml_copy_string(part->desc));

        Store_field(v_parts, i, v_part);
    }

    tsk_vs_close(vs_info);
    v_result = caml_alloc(1, 0);
    Store_field(v_result, 0, v_parts);
    CAMLreturn(v_result);

error:
    v_result = caml_alloc(1, 1);
    Store_field(v_result, 0, caml_copy_string(tsk_error_get()));
    CAMLreturn(v_result);
}

CAMLprim value caml_get_sub_entries(value v_tsk_fs_file)
{
    CAMLparam1(v_tsk_fs_file);
    CAMLlocal2(v_result, v_files);

    TSK_FS_FILE *file = (TSK_FS_FILE *)Nativeint_val(v_tsk_fs_file);
    if (!file || !file->meta || file->meta->type != TSK_FS_META_TYPE_DIR)
    {
        v_result = caml_alloc(1, 1);
        Store_field(v_result, 0, caml_copy_string("This entry is not a directory."));
        CAMLreturn(v_result);
    }

    TSK_FS_DIR *dir = tsk_fs_dir_open_meta(file->fs_info, file->meta->addr);
    size_t count = tsk_fs_dir_getsize(dir);
    v_files = caml_alloc(count, 0);
    for (size_t i = 0; i < count; i++)
    {
        TSK_FS_FILE *child = tsk_fs_dir_get(dir, i);
        Store_field(v_files, i, caml_copy_nativeint((intnat) child));
    }

    v_result = caml_alloc(1, 0);
    Store_field(v_result, 0, v_files);
    CAMLreturn(v_result);
}

CAMLprim value caml_get_filename(value v_tsk_fs_file) 
{
    CAMLparam1(v_tsk_fs_file);
    CAMLlocal1(v_result);

    TSK_FS_FILE *file = (TSK_FS_FILE *)Nativeint_val(v_tsk_fs_file);
    if (file && file->name && file->name->name)
    {
        v_result = caml_alloc(1, 0);
        Store_field(v_result, 0, caml_copy_string(file->name->name));
        CAMLreturn(v_result);
    }
    v_result = caml_alloc(1, 1);
    Store_field(v_result, 0, caml_copy_string("This entry is not named."));
    CAMLreturn(v_result);
}
