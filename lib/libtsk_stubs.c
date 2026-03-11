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

    char *path = String_val(v_path);

    TSK_IMG_INFO *img;
    img = tsk_img_open(1, (const TSK_TCHAR **)&path, TSK_IMG_TYPE_DETECT, 0);
    if (img == NULL)
    {
        fprintf(stderr, "이미지를 열 수 없습니다: %s\n", tsk_error_get());
        CAMLreturn(Val_none);
    }

    CAMLreturn(caml_copy_nativeint((intnat)img));
}

CAMLprim value caml_tsk_fs_open_img(value v_tsk_img_info, value v_offset)
{
    CAMLparam1(v_tsk_img_info);

    TSK_OFF_T offset = Int64_val(v_offset);
    TSK_IMG_INFO *img = (TSK_IMG_INFO *)Nativeint_val(v_tsk_img_info);
    TSK_FS_INFO *fs;

    fs = tsk_fs_open_img(img, offset * img->sector_size, TSK_FS_TYPE_DETECT);
    if (fs == NULL)
    {
        fprintf(stderr, "파일 시스템을 찾을 수 없습니다: %s\n", tsk_error_get());
        img->close(img);
        CAMLreturn(Val_none);
    }

    CAMLreturn(caml_copy_nativeint((intnat)fs));
}

CAMLprim value caml_tsk_fs_file_open(value v_fs, value v_path)
{
    CAMLparam2(v_fs, v_path);

    TSK_FS_INFO *fs = (TSK_FS_INFO *)Nativeint_val(v_fs);
    char *path = String_val(v_path);

    TSK_FS_FILE *file = tsk_fs_file_open(fs, NULL, path);
    if (file == NULL)
    {
        printf("파일 읽기 실패\n");
    }
    printf("%s\n", file->meta->name2->name);

    CAMLreturn(caml_copy_nativeint((intnat)file));
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

// int main(int argc, char **argv)
// {
//     TSK_IMG_INFO *img;
//     TSK_FS_INFO *fs;
//     TSK_FS_FILE *file;
//     char *image_path = "image.e01";
//     TSK_OFF_T offset = 2048; // mmls에서 확인한 파티션 시작 섹션 * 512
//     TSK_INUM_T inode = 64;   // 접근하려는 파일의 Inode 번호

//     // 1. 이미지 열기 (E01 지원 여부는 libewf 설치에 달림)
//     img = tsk_img_open(1, (const TSK_TCHAR **)&image_path, TSK_IMG_TYPE_DETECT, 0);
//     if (img == NULL)
//     {
//         fprintf(stderr, "이미지를 열 수 없습니다: %s\n", tsk_error_get());
//         return 1;
//     }

//     // 2. 특정 오프셋에서 파일 시스템 열기
//     fs = tsk_fs_open_img(img, offset * img->sector_size, TSK_FS_TYPE_DETECT);
//     if (fs == NULL)
//     {
//         fprintf(stderr, "파일 시스템을 찾을 수 없습니다: %s\n", tsk_error_get());
//         img->close(img);
//         return 1;
//     }

//     // 3. Inode 번호로 파일 열기
//     file = tsk_fs_file_open_meta(fs, NULL, inode);
//     if (file == NULL)
//     {
//         fprintf(stderr, "파일을 열 수 없습니다: %s\n", tsk_error_get());
//         fs->close(fs);
//         img->close(img);
//         return 1;
//     }

//     // 4. 파일 데이터 읽기 (예: 첫 100바이트)
//     char buffer[100];
//     ssize_t bytes_read = tsk_fs_file_read(file, 0, buffer, sizeof(buffer), TSK_FS_FILE_READ_FLAG_NONE);

//     if (bytes_read > 0)
//     {
//         printf("파일 읽기 성공 (%ld bytes)\n", (long)bytes_read);
//         // 여기서 buffer 데이터를 처리하거나 파일로 저장
//     }

//     // 5. 리소스 해제
//     tsk_fs_file_close(file);
//     fs->close(fs);
//     img->close(img);

//     return 0;
// }