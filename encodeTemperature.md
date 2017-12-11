#::Encoding temperatures each minute for a week::

We need `60*24*7 = 10080` entries, each which can encode "normal" temperatures with one decimal place.

"Normal" temperatures are in the range 0 to 64.0, or 8 bits of information
(where the 2 lower bits are decimal places)

Thus, a whole week can be encoded in `8*10080=80640` bits, which is roughly 10080 bytes or 10k of memory.

Thus, an array of `char` integers (8bit each) with length `10080` should be sufficient.