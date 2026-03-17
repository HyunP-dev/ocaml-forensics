open Core
open Core_unix
open Forensics

let main () =
  let open Result.Let_syntax in
  let image_path = "/mnt/d/Personal/Hunter XP.E01" in
  let%bind image_handle = Forensics.Tsk.img_open image_path in
  let%bind fs = Tsk.fs_open_img image_handle ~offset:63L in
  let%bind root = Tsk.fs_file_open fs "/" in
  let%bind children = Tsk.File.get_children root in
  children
  |> Array.map ~f:Tsk.File.get_name
  |> Array.iter ~f:(Result.iter ~f:print_endline);
  Ok ()
;;

let () =
  match main () with
  | Ok _ -> ()
  | Error msg -> prerr_endline msg
;;
