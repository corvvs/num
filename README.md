# ft_nm

An `nm` clone that can analyze ELF-formatted object / executable files
(in the near future.)

## Build

`$ make`

## Options

- `-a`
  - display all symbols, including debugging ones
- `-g`
  - display external symbols only. this OVERCOMES `-a`.
- `-u`
  - display undefined symbols only. this OVERCOMES `-a`.
- `-r`
  - sorting in reversed order.
- `-p`
  - display without any sorting.
