open Core
open Core_unix
open Forensics

let main () =
  let open Result.Let_syntax in
  (* Read partitions from a image. *)
  let path = "/mnt/d/Personal/Hunter XP.E01" in
  let%bind image_handle = Tsk.img_open path in
  let%bind partitions = Tsk.get_partitions image_handle in
  partitions
  |> Array.map ~f:Tsk.sexp_of_partition
  |> Array.map ~f:Sexp.to_string_hum
  |> Array.iter ~f:print_endline;
  (* Extract a hive file. *)
  let%bind fs = Tsk.fs_open_img image_handle ~offset:63L in
  let%bind file = Tsk.fs_file_open fs "/Windows/System32/Config/SOFTWARE" in
  let%bind filename = Tsk.File.get_name file in
  let raw = Tsk.fs_file_read ~offset:0L ~size:(-1L) file in
  let temp_filename = Filename_unix.temp_file "ocaml.forensics.hive" ".tmp" in
  Out_channel.with_file temp_filename ~binary:true ~f:(fun oc ->
    Out_channel.output_bytes oc raw);
  Printf.printf "'%s' is extracted into %s\n" filename temp_filename;
  (* Get product name from the hive file. *)
  let%bind registry = Registry.File.open_file temp_filename in
  let key_path = "\\Microsoft\\Windows NT\\CurrentVersion" in
  let%bind key = Registry.File.get_key_by_utf8_path registry key_path in
  let%bind value_type, value_raw =
    Registry.Key.get_value_by_name key ~name:"ProductName"
  in
  print_endline (Bytes.to_string value_raw);
  Core_unix.unlink temp_filename;
  Ok ()
;;

let () =
  match main () with
  | Ok _ -> ()
  | Error msg -> prerr_endline msg
;;
