### PNG Decoder
This uses PNGLE (commit: 789289547651ea4e69cf6fec2b054c083774d56d) from https://github.com/kikuchan/pngle which is under MIT License.

## Modifications
As the TFTs microcontrollers only have small amount of RAM available, only a single PNG may be decoded at a time.
Thus, the `pngle_new` will always return a static allocated pngle_t structure and not a dynamically allocated one.
If the static structure is in use and one is calling `pngle_new` the function will return `NULL`.