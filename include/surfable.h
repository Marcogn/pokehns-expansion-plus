#ifndef GUARD_SURFABLE_H
#define GUARD_SURFABLE_H

bool32 HasSurfablePokemonSprite(u16 species);
u8 CreateSurfablePokemonSpriteForOWE(u16 species, bool32 isShiny, SpriteCallback callback);
void FreeOWESwimSpritePaletteIfUnused(u16 tag);
#endif