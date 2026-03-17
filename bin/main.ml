open Core
open Core_unix
open Forensics

let main () =
  let open Result.Let_syntax in
  Ok ()
;;

let () =
  match main () with
  | Ok _ -> ()
  | Error msg -> prerr_endline msg
;;
