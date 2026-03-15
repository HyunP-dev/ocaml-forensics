open Core
open Core_unix
open Forensics
open Hex

module CommonAnalysis = struct
  let xxd file =
    file
    |> Tsk.fs_file_read ~offset:0L ~size:(-1L)
    |> Hex.of_bytes
    |> Hex.hexdump
  ;;
end

module RegistryAnalysis = struct
  let show path file =
    let raw = Tsk.fs_file_read ~offset:0L ~size:(-1L) file in
    Out_channel.with_file
      "TEMP_REG.raw"
      ~binary:true
      ~append:false
      ~f:(fun oc -> Out_channel.output_bytes oc raw);
    let%bind.Result registry = Registry.File.open_file "TEMP_REG.raw" in
    let%bind.Result key = Registry.File.get_key_by_utf8_path registry path in
    key
    |> Registry.Key.get_value_names
    |> Result.iter
         ~f:
           (Array.iter ~f:(fun name ->
              Registry.Key.get_value_by_name key ~name
              |> Result.iter ~f:(fun (value_type, raw) ->
                print_endline
                  (name ^ ": " ^ Registry.ValueType.to_string value_type);
                (match value_type with
                 | String -> print_endline (Bytes.to_string raw)
                 | Int32LE ->
                   Bytes.unsafe_get_int32 raw 0
                   |> Int.of_int32
                   |> Option.map ~f:Float.of_int
                   |> Option.map ~f:Time_float.Span.of_sec
                   |> Option.map ~f:Time_float.of_span_since_epoch
                   |> Option.map ~f:Time_float.to_string_utc
                   |> Option.iter ~f:print_endline
                 | _ -> Hex.hexdump (Hex.of_bytes raw));
                print_endline "")))
    |> ignore;
    Ok ()
  ;;
end

let main () =
  let open Result.Let_syntax in

  (* Read partitions from a image. *)
  let path = "/Users/rpark/Downloads/Hunter XP.E01" in
  let%bind image_handle = Tsk.img_open path in
  let%bind partitions = Tsk.get_partitions image_handle in
  partitions
  |> Array.map ~f:Tsk.sexp_of_partition
  |> Array.map ~f:Sexp.to_string_hum
  |> Array.iter ~f:print_endline;

  (* Extract a hive file. *)
  let%bind fs = Tsk.fs_open_img image_handle ~offset:63L in
  let%bind file = Tsk.fs_file_open fs "/Windows/System32/Config/SOFTWARE" in
  let raw = Tsk.fs_file_read ~offset:0L ~size:(-1L) file in
  let temp_filename = Filename_unix.temp_file "ocaml.forensics.hive" ".tmp" in
  Out_channel.with_file temp_filename ~binary:true ~f:(fun oc ->
    Out_channel.output_bytes oc raw);
    
  (* Get product name from the hive file. *)
  let%bind registry = Registry.File.open_file temp_filename in
  let key_path = "\\Microsoft\\Windows NT\\CurrentVersion" in
  let%bind key = Registry.File.get_key_by_utf8_path registry key_path in
  let%bind value_type, value_raw = Registry.Key.get_value_by_name key ~name:"ProductName" in
  print_endline (Bytes.to_string value_raw);

  Core_unix.unlink temp_filename;
  Ok ()
;;

main ()
