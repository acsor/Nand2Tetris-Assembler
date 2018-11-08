#include "tokenizer.h"


int asm_instr_to_bitstr(instr const in, char *const dest) {
	int bitmask = 1 << 15, i = 0;

	while (bitmask > 0 && i < 16) {
		dest[i] = in.bits & bitmask ? '1': '0';

		bitmask >>= 1;
		i++;
	}

	dest[i] = '\0';

	return i + 1;
}

instr_type asm_instr_type(instr const in) {
	// 0x8000 = 1 << 15 = 1000 0000 0000 0000 (binary form).
	if (in.bits & 0x8000) {
		return C;
	}

	return A;
}
