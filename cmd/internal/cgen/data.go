// Code generated by running "go generate". DO NOT EDIT.

// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

package cgen

const preamble = "" +
	"// Use of this source code is governed by a BSD-style license that can be found\n// in the LICENSE file.\n\n#include <stdbool.h>\n#include <stdint.h>\n#include <string.h>\n\n// Puffs requires a word size of at least 32 bits because it assumes that\n// converting a u32 to usize will never overflow. For example, the size of a\n// decoded image is often represented, explicitly or implicitly in an image\n// file, as a u32, and it is convenient to compare that to a buffer size.\n#if __WORDSIZE < 32\n#error \"Puffs requires a word size of at least 32 bits\"\n#endif\n" +
	""
