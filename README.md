# ocaml-forensics
Digital forensics library for OCaml

## Concepts
- For writing verifiable analysis scripts using functional programming. 
- To make writing scripts easy with the HM System.

## Examples
```OCaml
open Core
open Forensics

let main () =
  let path = "/Users/rpark/Downloads/Hunter XP.E01" in
  let image_handle =
    match Tsk.img_open path with
    | Ok r -> r
    | Error msg -> failwith msg
  in
  let partitions =
    match Tsk.get_partitions image_handle with
    | Ok r -> r
    | Error msg -> failwith msg
  in
  partitions
  |> Array.map ~f:Tsk.sexp_of_partition
  |> Array.map ~f:Sexp.to_string_hum
  |> Array.iter ~f:print_endline
;;

main ()
```
