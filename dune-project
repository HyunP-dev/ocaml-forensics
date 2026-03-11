(lang dune 3.21)

(name ocaml-forensics)

(generate_opam_files true)

(source
 (github HyunP-dev/ocaml-forensics))

(authors "Hyun Park <hyun.forensics@gmail.com>")

(maintainers "Hyun Park <hyun.forensics@gmail.com>")

(license LICENSE)

(documentation https://hyunp-dev.github.io/ocaml-forensics)

(package
 (name ocaml-forensics)
 (synopsis "Digital forensics library for OCaml")
 (description "Library for easy writing of digital forensic analysis scripts")
 (depends ocaml)
 (tags
  ("Digital Forensics")))

; See the complete stanza docs at https://dune.readthedocs.io/en/stable/reference/dune-project/index.html
