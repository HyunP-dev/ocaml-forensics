external get_version : unit -> string = "caml_libewf_get_version"

type libewf_handle

external open_ewf_image : string list -> libewf_handle = "caml_open_ewf_image"

external read_bytes
  :  libewf_handle
  -> offset:int64
  -> size:int64
  -> bytes
  = "caml_read_bytes_from_ewf"
