open Core

type libregf_file_t
type libregf_key_t
type libregf_value_t

module ValueType = struct
  type t =
    | Undefined
    | String
    | ExpandableString
    | BinaryData
    | Int32LE
    | Int32BE
    | SymbolicLink
    | MultiValueString
    | ResourceList
    | FullResourceDescriptor
    | ResourceRequirementsList
    | Int64LE

  let to_string v =
    match v with
    | Undefined -> "Undefined"
    | String -> "REG_SZ"
    | ExpandableString -> "REG_EXPAND_SZ"
    | BinaryData -> "REG_BINARY"
    | Int32LE -> "REG_DWORD_LITTLE_ENDIAN"
    | Int32BE -> "REG_DWORD_BIG_ENDIAN"
    | SymbolicLink -> "REG_LINK"
    | MultiValueString -> "REG_MULTI_SZ"
    | ResourceList -> "ResourceList"
    | FullResourceDescriptor -> "FullResourceDescriptor"
    | ResourceRequirementsList -> "ResourceRequirementsList"
    | Int64LE -> "REG_QWORD_LITTLE_ENDIAN"
  ;;
end

module File = struct
  type t = libregf_file_t

  external initialize
    :  unit
    -> (t, string) result
    = "caml_libregf_file_initialize"

  external open_file_internal
    :  t
    -> string
    -> (t, string) result
    = "caml_libregf_file_open"

  external get_key_by_utf8_path
    :  t
    -> string
    -> (libregf_key_t, string) result
    = "caml_libregf_file_get_key_by_utf8_path"

  let open_file path =
    let%bind.Result handle = initialize () in
    open_file_internal handle path
  ;;
end

module Key = struct
  type t = libregf_key_t

  external get_utf8_name
    :  t
    -> (string, string) result
    = "caml_libregf_key_get_utf8_name"

  external get_subkey_names
    :  t
    -> (string array, string) result
    = "caml_get_subkey_names"

  (* external get_values
  :  Key.t
  -> (libregf_value_t array, string) result
  = "caml_get_values" *)

  external get_value_names
    :  t
    -> (string array, string) result
    = "caml_get_value_names"

  external get_value_by_name
    :  t
    -> name:string
    -> (ValueType.t * bytes, string) result
    = "caml_get_value"
end
