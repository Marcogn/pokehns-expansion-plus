#ifndef GUARD_HASEP_SYMBOLS_H
#define GUARD_HASEP_SYMBOLS_H

// A table of named addresses in the ROM, for companion apps that read the
// running game (the PokeDaisy fork, Marcogn/pokedaisy-hasep-support). They
// used to recognise a ROM by its SHA-1 and carry hand-found addresses for
// that one build; every Hasep build moves the RAM and the code, so instead
// the linker writes the addresses here and the app reads them by name.
//
// Layout (all little endian), right after the RHH header (ld_script_modern.ld):
//   struct HasepSymbolsHeader, then every struct HasepSymbol up to `end`.
// The header's layoutVersion pins the struct layouts the app assumes (see the
// STATIC_ASSERTs in src/hasep_symbols.c): a change that trips one of them
// bumps HASEP_SYMBOLS_LAYOUT_VERSION, and the app needs updating.

#define HASEP_SYMBOLS_FORMAT_VERSION 1
#define HASEP_SYMBOLS_LAYOUT_VERSION 1
#define HASEP_SYMBOL_NAME_LENGTH     44

struct HasepSymbol
{
    char name[HASEP_SYMBOL_NAME_LENGTH]; // NUL padded
    u32 value;
};

// An address (or a constant) the app may ask for by name. Usable at file
// scope in any source file, so a static function or table is exported from
// the file that owns it without changing its linkage. Exported entries end up
// contiguous in .text.hasep_symbols, which the linker script keeps.
#define HASEP_EXPORT_VALUE(id, label, val)                                       \
    __attribute__((section(".text.hasep_symbols"), used, aligned(4)))           \
    static const struct HasepSymbol sHasepSymbol_##id = { label, (u32)(val) }

#define HASEP_EXPORT(sym) HASEP_EXPORT_VALUE(sym, #sym, &sym)

#endif // GUARD_HASEP_SYMBOLS_H
